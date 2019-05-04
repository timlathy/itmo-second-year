open OUnit
open Lib

let c_code_file_case (input : string) (code_path : string) = fun () ->
    let _ = print_string (Sys.getcwd ()) in
    let expected = Io.read_all_lines (open_in ("../../../test/expected_c_code/" ^ code_path))
    in assert_equal (Ok expected) (Driver.regex_to_c input) ~printer:(function
    | Ok code -> code
    | Error err -> "[ERROR] " ^ err)

let suite = [
    "literal match" >::
        c_code_file_case "test?" "literals-optional.c"
]
