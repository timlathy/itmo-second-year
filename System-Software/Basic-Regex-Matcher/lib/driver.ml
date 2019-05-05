open Base

let clang_format_style = "{BasedOnStyle: Chromium, AllowShortBlocksOnASingleLine: false}"
let clang_format_cmd = "clang-format --style=\"" ^ clang_format_style ^ "\""

let clang_cmd path = "clang -x c -std=c99 -O2 -shared -o " ^ path ^ " -"

let compiled_regex_dylib_path =
    Stdlib.Filename.get_temp_dir_name() ^ "/bre-compiled-" ^ Int.to_string (Unix.getpid()) ^ ".so"

let format_c_code str =
    let ch_in, ch_out = Unix.open_process clang_format_cmd in
    Stdlib.output_string ch_out str;
    Stdlib.close_out ch_out;
    let formatted = Io.read_all_lines ch_in in
    Stdlib.close_in ch_in;
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

let compile_c_to_temp_so code =
    let cmd = clang_cmd compiled_regex_dylib_path in
    let clang_stdin = Unix.open_process_out cmd in
    let _ = Stdlib.output_string clang_stdin code in
    match Unix.close_process_out clang_stdin with
    | Unix.WEXITED 0 -> Ok compiled_regex_dylib_path
    | _ -> Error "clang has exited with a non-zero status code"

let compile_regex regex =
    match regex_to_c regex with
    | Ok c_code -> (match compile_c_to_temp_so c_code with
        | Ok so_path ->
            Ok (Dl.dlopen ~filename:so_path ~flags:[Dl.RTLD_NOW])
        | Error err ->
            Error err)
    | Error err ->
        Error err

let run_matcher_and_dispose_dylib regex matcher =
    match compile_regex regex with
    | Ok library ->
        let match_fun = Foreign.foreign ~from:library "match" Ctypes.(string @-> int @-> returning bool) in
        matcher match_fun;
        Dl.dlclose ~handle:library;
        Unix.unlink compiled_regex_dylib_path;
        Ok ()
    | Error err -> Error err
