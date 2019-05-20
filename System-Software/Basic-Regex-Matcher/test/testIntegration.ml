open OUnit
open Lib

let integration_case (regex : string) (input : string) (expected : string) = fun () ->
    assert_equal (Ok expected) (Cli.run regex input) ~printer:(function
    | Ok result -> result
    | Error err -> "[ERROR] " ^ err)

let suite = [
    "literals" >::
        integration_case "1234567" "asdf1234567" "Matched from 4 to 10\n";
    "character classes" >::
        integration_case "[UO]w[A-Z]" "OwG" "Matched from 0 to 2\n";
    "groups" >::
        integration_case "(A(ba)?)+" "AbaA" "Matched from 0 to 3\nGroup #0: from 3 to 3\nGroup #1: from 1 to 2\n";
]
