open OUnit
open Lib.Types

let assert_expr_equal expected actual =
    assert_equal expected actual ~printer:format_expr

let suite =
    "Parser" >::: [
        "symbols" >:: (fun _ ->
            let sym = Character 'h'
            in assert_expr_equal sym (Character 'h')
        )
    ]

let _ = run_test_tt_main suite
