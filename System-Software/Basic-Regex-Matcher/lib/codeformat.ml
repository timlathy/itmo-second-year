let clang_format_style = "{BasedOnStyle: Chromium, AllowShortBlocksOnASingleLine: false}"
let clang_format_cmd = "clang-format --style=\"" ^ clang_format_style ^ "\""

let read_all_lines ch_in : string list =
    let lines = ref [] in
    let _ = try
        while true do
            lines := input_line ch_in :: !lines
        done
    with End_of_file ->
        close_in ch_in
    in List.rev !lines

let format_code str =
    let ch_in, ch_out = Unix.open_process clang_format_cmd in
    output_string ch_out str;
    close_out ch_out;
    let formatted = read_all_lines ch_in in
    close_in ch_in;
    Base.String.concat ~sep:"\n" formatted
