open Base
open Types

type intermediate_state = { node_idx: int; nodes: string list; group_stack: int list }

type node_prologue = Code of string | Repetition | Optional | OptionalRepetition | None

let init_state = { node_idx = 0; nodes = []; group_stack = [] }

let rec charseq_as_little_endian_hex chars =
    chars
    |> List.rev_map ~f:Char.to_int (* rev map to account for endianness *)
    |> charseq_to_hex_recurse (Buffer.create (List.length chars * 2))
    |> Buffer.contents
and charseq_to_hex_recurse buf = function
    | [] -> buf
    | c :: rest ->
        Printf.bprintf buf "%x" c;
        charseq_to_hex_recurse buf rest

let rec literal_comparison charptr = function
    | c :: [] ->
        "pos[" ^ Int.to_string charptr ^ "] == '" ^ String.of_char c ^ "'"
    | chars when List.length chars >= 8 ->
        literal_comparison_fused_chars 8 "int64_t" charptr chars
    | chars when List.length chars >= 4 ->
        literal_comparison_fused_chars 4 "int32_t" charptr chars
    | chars when List.length chars >= 2 ->
        literal_comparison_fused_chars 2 "int16_t" charptr chars
    | _ ->
        failwith "unable to emit literal comparison"
and literal_comparison_fused_chars cnt ccast charptr chars =
    let (fused, rest) = List.split_n chars cnt in
    let fused_chars_hex = "0x" ^ charseq_as_little_endian_hex fused in
    let fused_comp = "*(" ^ ccast ^ "*)(pos + " ^ Int.to_string charptr ^ ") == " ^ fused_chars_hex in
    match rest with
    | [] -> fused_comp
    | rest -> fused_comp ^ " && " ^ literal_comparison (charptr + cnt) rest

let edge_condition_and_pos_incr = function
    | CondLiteral lit ->
        let charcnt = String.length lit in
        let cond = "end - pos >= " ^ Int.to_string charcnt ^ " && " ^ literal_comparison 0 (String.to_list lit) in
        cond, charcnt
    | CondEitherOf conds ->
        let rec charcnt = (function
            | [] -> 1
            | CondCharInAsciiRange _ :: rest -> charcnt rest
            | CondLiteral lit :: rest when String.length lit = 1 -> charcnt rest
            | _ -> failwith "either-of condition must only include single-char literals and ranges") in
        let charcnt = charcnt conds in
        let comparisons = List.map conds ~f:(function
            | CondLiteral lit -> "pos[0] == '" ^ lit ^ "'"
            | CondCharInAsciiRange (a, b) -> "(pos[0] >= '" ^ Char.to_string a ^ "' && pos[0] <= '" ^ Char.to_string b ^ "')"
            | _ -> failwith "either-of condition must only include literals of the same length") in
        let disjunction = String.concat comparisons ~sep:" || " in
        let cond = "end - pos >= " ^ Int.to_string charcnt ^ " && (" ^ disjunction ^ ")" in
        cond, charcnt
    | c ->
        failwith ("unsupported condition " ^ Types.format_graph_edge (c, { attrs = []; edges = []}))

let enter_alternative_attr = function | EnterAlternativeNode _ -> true | _ -> false

let rec emit_node_prologue state = function
    | [RepeatingNode] -> Repetition, state
    | [OptionalNode] -> Optional, state
    | [OptionalNode; RepeatingNode] -> OptionalRepetition, state
    | [RepeatingNode; EnterAlternativeNode _] -> Repetition, state
    | [OptionalNode; EnterAlternativeNode _] -> Optional, state
    | [OptionalNode; RepeatingNode; EnterAlternativeNode _] -> OptionalRepetition, state
    | MatchCompleteNode :: _ ->
        Code "goto finish;", state
    | GroupStartNode gidx :: _ ->
        let state = { state with group_stack = gidx :: state.group_stack } in
        Code ("g" ^ Int.to_string gidx ^ ": groups[" ^ Int.to_string gidx ^ "].group_start = pos - str;"), state
    | GroupEndNode gidx :: rest ->
        let is_repeating, is_optional, rest = (match rest with
            | RepeatingNode :: OptionalNode :: rest -> true, true, rest
            | RepeatingNode :: rest -> true, false, rest
            | OptionalNode :: rest -> false, true, rest
            | _ -> false, false, rest) in
        let acc, state = emit_node_prologue state rest in
        let state = (match state.group_stack with
            | _ :: rest -> { state with group_stack = rest }
            | _ -> state) in
        let gidx = Int.to_string gidx in
        let code = "groups[" ^ gidx ^ "].group_end = pos - str;" ^
        (if is_repeating
            then "groups[" ^ gidx ^ "].reserved_prev_start = groups[" ^ gidx ^ "].group_start; goto g" ^ gidx ^ ";"
            else if is_optional
                then "groups[" ^ gidx ^ "].reserved_prev_start = groups[" ^ gidx ^ "].group_start;goto g" ^ gidx ^ "_success;"
                else "goto g" ^ gidx ^ "_success;") ^
        "g" ^ gidx ^ "_fail:" ^
        (if is_optional
            then ""
            else if is_repeating
                then "if (groups[" ^ gidx ^ "].group_end == 0) goto fail;"
                else "goto fail;") ^
        (if is_repeating || is_optional
            then "groups[" ^ gidx ^ "].group_start = groups[" ^ gidx ^ "].reserved_prev_start;" else "") ^
        (if not is_repeating
            then "g" ^ gidx ^ "_success:" else "") ^ (match acc with | Code c -> c | _ -> "") in
        Code code, state
    | _ :: rest ->
        emit_node_prologue state rest
    | _ ->
        None, state

let rec emit_edge_branches (state : intermediate_state) prologue = function
    | [] ->
        (match prologue with
            | Code c -> c, state
            | _ -> "", state)
    | conditions ->
        let curr_node_idx = state.node_idx - 1 in
        let curr_node = Int.to_string curr_node_idx in
        let start_code = (match prologue with
            | Code c -> c
            | Repetition -> "{ int repeats = 0; s" ^ curr_node ^ "_repeat:"
            | Optional -> "{"
            | OptionalRepetition -> "{ s" ^ curr_node ^ "_repeat:"
            | _ -> ""
        ) in
        let gen_branch = (match prologue with
            | Repetition -> fun _ -> "repeats = 1; goto s" ^ curr_node ^ "_repeat;"
            | OptionalRepetition -> fun _ -> "goto s" ^ curr_node ^ "_repeat;"
            | _ -> fun s -> "goto s" ^ Int.to_string s.node_idx ^ ";"
        ) in
        let conditions, epilogue_lazy = (match List.last_exn conditions with
            | Unconditional, { attrs = [JumpToAlternativeNode idx]; _ } ->
                let conditions = List.take conditions ((List.length conditions) - 1) in
                conditions, (fun s -> "goto alt_" ^ Int.to_string idx ^ ";", s)
            | Unconditional, node when not (List.exists node.attrs ~f:enter_alternative_attr) ->
                let conditions = List.take conditions ((List.length conditions) - 1) in
                conditions, (fun s -> "goto s" ^ Int.to_string s.node_idx ^ ";", append_node s node)
            | _ ->
                conditions, (fun s ->
                    let fail_label, s = (match state.group_stack with
                        | gidx :: _ -> "g" ^ Int.to_string gidx ^ "_fail", s
                        | _ -> "fail", s) in
                    match prologue with
                        | Repetition ->
                            "if (repeats == 1) goto s" ^ Int.to_string (curr_node_idx + 1) ^ ";" ^
                            "else goto " ^ fail_label ^ "; }", s
                        | Optional | OptionalRepetition ->
                            "goto s" ^ Int.to_string (curr_node_idx + 1) ^ "; }", s
                        | _ ->
                            "goto " ^ fail_label ^ ";", s
            )) in
        let blocks, state = List.fold conditions ~init:([], state) ~f:(fun (blocks, state) -> function
            | Unconditional, ({ attrs; _ } as node) ->
                let idx = List.find attrs ~f:enter_alternative_attr
                    |> (function | Some(EnterAlternativeNode i) -> i | _ -> 0) |> Int.to_string in
                let branch = "alt_" ^ idx ^ ": int alt_" ^ idx ^ "_pos = pos; goto s" ^ Int.to_string state.node_idx ^ ";" in
                branch :: blocks, append_node state node
            | cond, node ->
                let (c_cond, pos_incr) = edge_condition_and_pos_incr cond in
                let branch = "if (" ^ c_cond ^ ") { pos += " ^ Int.to_string pos_incr ^ ";" ^ gen_branch state ^ "}" in
                match blocks with
                    | [] -> [branch], append_node state node
                    | _ -> branch :: " else " :: blocks, append_node state node
        ) in
        let epilogue, state = epilogue_lazy state in
        let body = String.concat ((List.rev blocks) @ [epilogue]) ~sep:""
        in start_code ^ body, state
and append_node state { attrs; edges } =
    let node_idx = state.node_idx in
    let prologue, state = emit_node_prologue state attrs in
    let branches, state = emit_edge_branches { state with node_idx = node_idx + 1 } prologue edges in
    { state with nodes = ("s" ^ Int.to_string node_idx ^ ": " ^ branches) :: state.nodes }

let graph_with_groups_to_c group_count graph =
    let { nodes; _ } = graph |> append_node init_state in
    let c_body = String.concat ~sep:"\n" nodes in
    "#include <stdint.h>\n#include <stdlib.h>\n" ^
    "struct match_group { int group_start; int group_end; int reserved_prev_start; };" ^
    "struct match_result { int match_start; int match_end; int group_count; struct match_group* match_groups; };" ^
    "struct match_result match(const char* str, int len) {" ^
    "struct match_group* const groups = " ^ (if group_count = 0
        then "0;"
        else "calloc(" ^ Int.to_string group_count ^ ", sizeof(struct match_group));"
    ) ^
    "const char* const end = str + len;" ^
    "const char* match_start = str;" ^
    "const char* pos;" ^
    "loop: pos = match_start;" ^
    c_body ^
    "fail: if (++match_start < end) { goto loop; } return (struct match_result){0,0,0,groups};" ^
    "finish: return (struct match_result){match_start - str,pos - str," ^ Int.to_string group_count ^ ",groups}; }"
