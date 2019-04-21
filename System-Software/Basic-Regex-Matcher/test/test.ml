open OUnit
open Lib

let parser_case regex (expected : Types.expr) = fun () ->
    let parsed = Parser.parse_regex regex in
    let expected_result : (Types.expr, string) result = Ok expected in
    assert_equal expected_result parsed ~printer:Types.format_parse_result

let suite =
    "Parser" >::: [
        "char sequence" >:: parser_case "abcd" (Sequence "abcd")
    ]

let _ = run_test_tt_main suite
