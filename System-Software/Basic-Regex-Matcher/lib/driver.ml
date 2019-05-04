let clang_format_style = "{BasedOnStyle: Chromium, AllowShortBlocksOnASingleLine: false}"
let clang_format_cmd = "clang-format --style=\"" ^ clang_format_style ^ "\""

let format_c_code str =
    let ch_in, ch_out = Unix.open_process clang_format_cmd in
    output_string ch_out str;
    close_out ch_out;
    let formatted = Io.read_all_lines ch_in in
    close_in ch_in;
    formatted

let regex_to_c regex =
    match Parser.parse_regex regex with
    | Ok syntax_tree ->
        let c_code = syntax_tree
        |> Graph.from_expr
        |> Codegen.graph_to_c
        |> format_c_code
        in Ok c_code
    | Error err -> Error err
