open OUnit
open Lib

let regex_case (input : string) (expected : string) = fun () ->
    assert_equal (Ok expected) (Driver.regex_to_c input) ~printer:(function
    | Ok code -> code
    | Error err -> "[ERROR] " ^ err)

let suite = [
    "literal match" >::
        regex_case "fight me" "fail"
]
