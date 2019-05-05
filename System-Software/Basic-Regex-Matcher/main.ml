open Lib
open Lib.Types

let apply_matcher str matcher =
    let result = matcher str (String.length str) in
    if result.match_found then
        print_endline "Matched!"
    else
        print_endline "No matches found"

let () =
    if Array.length (Sys.argv) != 3 then
        print_endline ("Usage: " ^ Sys.argv.(0) ^ " regex string")
    else
        let regex = Sys.argv.(1) in
        let str = Sys.argv.(2) in
        match Driver.run_matcher_and_dispose_dylib regex (apply_matcher str) with
        | Ok _ -> ()
        | Error err -> failwith err
