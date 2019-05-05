open Base
open Types

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

let rec emit_edge_blocks acc nodes = function
    | [] ->
        acc ^ "else { goto fail; }", nodes
    | (cond, node) :: rest ->
        let (ccond, pos_incr) = edge_condition_and_pos_incr cond in
        let node_label = "s" ^ (nodes |> List.length |> Int.to_string) in
        let block = "{ pos += " ^ Int.to_string pos_incr ^ "; goto " ^ node_label ^ "; }" in
        let nodes = append_node nodes node in
        let acc = acc ^ "if (" ^ ccond ^ ")" ^ block in
        emit_edge_blocks acc nodes rest
and append_node nodes = function
    | { attrs = []; edges } ->
        let (edge_block, nodes) = emit_edge_blocks "" nodes edges in
        let node_index = nodes |> List.length |> Int.to_string in
        let node = "s" ^ node_index ^ ": " ^ edge_block in
        node :: nodes
    | { attrs = [MatchCompleteNode]; edges = [] } ->
        let node_index = nodes |> List.length |> Int.to_string in
        let node = "s" ^ node_index ^ ": goto finish;" in
        node :: nodes
    | n -> failwith ("unhandled node " ^ Types.format_graph_node n)

let graph_to_c graph =
    let nodes = graph |> append_node [] |> String.concat ~sep:"\n" in
    "#include <stdint.h>\n" ^
    "struct match_group { int group_start; int group_end; };" ^
    "struct match_result { int match_found; int group_count; struct match_group* match_groups; };" ^
    "struct match_result match(const char* str, int len) {" ^
    "struct match_group* const groups = 0;" ^
    "const char* const end = str + len;" ^
    "const char* match_start = str;" ^
    "const char* pos;" ^
    "loop: pos = match_start;" ^
    nodes ^
    "fail: if (++match_start < end) goto loop; return (struct match_result){0,0,groups};" ^
    "finish: return (struct match_result){1,0,groups}; }"
