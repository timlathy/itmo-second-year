open OUnit
open Lib

let c_code_file_case (input : string) (code_path : string) = fun () ->
    let _ = print_string (Sys.getcwd ()) in
    let expected = Io.read_all_lines (open_in ("../../../test/expected_c_code/" ^ code_path))
    in assert_equal (Ok expected) (Driver.regex_to_c input ~format_code:true) ~printer:(function
    | Ok code -> code
    | Error err -> "[ERROR] " ^ err)

let suite = [
    "literal match" >::
        c_code_file_case "1234567" "literals.c";
    "capturing groups" >::
        c_code_file_case "1(test)2(test)" "groups.c";
    "quantifiers" >::
        c_code_file_case "1?2+3*" "quantifiers.c";
    "quantified groups" >::
        c_code_file_case "(the)+(flower)?wesaw" "quantified_groups.c";
]
