open Base
open Types

type intermediate_state = { node_idx: int; nodes: string list }

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

let rec emit_node_prelude = function
    | [] ->
        ""
    | MatchCompleteNode :: _ ->
        "goto finish;"
    | GroupStartNode gidx :: rest ->
        "groups[" ^ Int.to_string gidx ^ "].group_start = pos - str;" ^ emit_node_prelude rest
    | GroupEndNode gidx :: rest ->
        "groups[" ^ Int.to_string gidx ^ "].group_end = pos - str;" ^ emit_node_prelude rest
    | _ ->
        failwith "unsupported node attribute"

let rec emit_edge_branches acc (state : intermediate_state) = function
    | [] when not (String.is_empty acc) ->
        acc ^ "else { goto fail; }", state
    | [] ->
        acc, state
    | (Unconditional, node) :: _ ->
        let branch = "goto s" ^ Int.to_string state.node_idx ^ ";" in
        acc ^ branch, append_node state node
    | (cond, node) :: rest ->
        let (ccond, pos_incr) = edge_condition_and_pos_incr cond in
        let branch = "{ pos += " ^ Int.to_string pos_incr ^ "; goto s" ^ Int.to_string state.node_idx ^ "; }" in
        let state = append_node state node in
        let acc = acc ^ "if (" ^ ccond ^ ")" ^ branch in
        emit_edge_branches acc state rest
and append_node state { attrs; edges } =
    let prelude = emit_node_prelude attrs in
    let node_idx = state.node_idx in
    let (branches, state) = emit_edge_branches "" { state with node_idx = node_idx + 1 } edges in
    { state with nodes = ("s" ^ Int.to_string node_idx ^ ": " ^ prelude ^ branches) :: state.nodes }

let graph_with_groups_to_c group_count graph =
    let { nodes; _ } = graph |> append_node { nodes = []; node_idx = 0 } in
    let c_body = String.concat ~sep:"\n" nodes in
    "#include <stdint.h>\n#include <stdlib.h>\n" ^
    "struct match_group { int group_start; int group_end; };" ^
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
