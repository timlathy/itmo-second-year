open OUnit
open Lib

let integration_case (regex : string) (input : string) (expected : string) = fun () ->
    assert_equal (Ok expected) (Cli.run regex input) ~printer:(function
    | Ok result -> result
    | Error err -> "[ERROR] " ^ err)

let suite = [
    "literals" >::
        integration_case "1234567" "asdf1234567" "Matched from 4 to 11\n";
    "character classes" >::
        integration_case "[UO]w[A-Z]" "OwG" "Matched from 0 to 3\n";
]
