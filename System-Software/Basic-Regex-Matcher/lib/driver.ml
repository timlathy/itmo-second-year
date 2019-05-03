let clang_format_style = "{BasedOnStyle: Chromium, AllowShortBlocksOnASingleLine: false}"
let clang_format_cmd = "clang-format --style=\"" ^ clang_format_style ^ "\""

let read_all_lines ch_in : string =
    let linebuf = Buffer.create 0 in
    let _ = try
        while true do
            ch_in |> input_line |> Buffer.add_string linebuf;
            Buffer.add_string linebuf "\n"
        done
    with End_of_file -> close_in ch_in
    in Buffer.contents linebuf

let format_c_code str =
    let ch_in, ch_out = Unix.open_process clang_format_cmd in
    output_string ch_out str;
    close_out ch_out;
    let formatted = read_all_lines ch_in in
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
