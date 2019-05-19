open Base
open Types

type intermediate_state = { node_idx: int; nodes: string list; group_stack: int list }

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
        let cond = "end - pos >= " ^ (Int.to_string charcnt) ^ " && " ^ (literal_comparison 0 (String.to_list lit)) in
        cond, charcnt
    | _ ->
        failwith "unsupported condition"

let rec emit_node_prelude state = function
    | MatchCompleteNode :: _ ->
        "goto finish;", state
    | GroupStartNode gidx :: [] ->
        let state = { state with group_stack = gidx :: state.group_stack } in
        "g" ^ Int.to_string gidx ^ ": groups[" ^ Int.to_string gidx ^ "].group_start = pos - str;", state
    | GroupEndNode gidx :: rest ->
        let is_repeating, is_optional, rest = (match rest with
            | RepeatingNode :: OptionalNode :: rest -> true, true, rest
            | RepeatingNode :: rest -> true, false, rest
            | OptionalNode :: rest -> false, true, rest
            | _ -> false, false, rest) in
        let acc, state = emit_node_prelude state rest in
        let state = (match state.group_stack with
            | _ :: rest -> { state with group_stack = rest }
            | _ -> state) in
        let gidx = Int.to_string gidx in
        "groups[" ^ gidx ^ "].group_end = pos - str;" ^
        (if is_repeating
            then "groups[" ^ gidx ^ "].reserved_prev_start = groups[" ^ gidx ^ "].group_start; goto g" ^ gidx ^ ";"
            else "goto g" ^ gidx ^ "_success;") ^
        "g" ^ gidx ^ "_fail:" ^
        (if is_optional
            then ""
            else if is_repeating
                then "if (groups[" ^ gidx ^ "].group_end == 0) goto fail;"
                else "goto fail;") ^
        (if is_repeating
            then "groups[" ^ gidx ^ "].group_start = groups[" ^ gidx ^ "].reserved_prev_start;"
            else "g" ^ gidx ^ "_success:") ^
        acc, state
    | _ :: rest ->
        emit_node_prelude state rest
    | _ ->
        "", state

let rec emit_edge_branches (state : intermediate_state) = function
    | [] ->
        "", state
    | conditions ->
        let conditions, epilogue_lazy = (match List.last_exn conditions with
            | Unconditional, node ->
                let conditions = List.take conditions ((List.length conditions) - 1) in
                conditions, (fun s ->
                    "goto s" ^ Int.to_string s.node_idx ^ ";", append_node s node)
            | _ ->
                conditions, (match state.group_stack with
                    | gidx :: _ -> fun s -> "goto g" ^ Int.to_string gidx ^ "_fail;", s
                    | _ -> fun s -> "goto fail;", s
            )) in
        let cond_blocks = List.map conditions ~f:(fun (cond, node) ->
            let (c_cond, pos_incr) = edge_condition_and_pos_incr cond in
            "if (" ^ c_cond ^ ")", pos_incr, node
        ) in
        let blocks, state = List.fold cond_blocks ~init:([], state) ~f:(fun (blocks, state) (cond_expr, pos_incr, node) ->
            let branch = "{ pos += " ^ Int.to_string pos_incr ^ "; goto s" ^ Int.to_string state.node_idx ^ "; }" in
            (cond_expr ^ branch) :: blocks, append_node state node
        ) in
        let epilogue, state = epilogue_lazy state in
        String.concat (blocks @ [epilogue]) ~sep:" else ", state
and append_node state { attrs; edges } =
    let node_idx = state.node_idx in
    let prelude, state = emit_node_prelude state attrs in
    let branches, state = emit_edge_branches { state with node_idx = node_idx + 1 } edges in
    { state with nodes = ("s" ^ Int.to_string node_idx ^ ": " ^ prelude ^ branches) :: state.nodes }

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
