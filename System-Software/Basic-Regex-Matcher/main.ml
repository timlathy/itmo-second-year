open Lib

let apply_matcher str matcher =
    if matcher str (String.length str) then
        print_string "Matched!\n"
    else
        print_string "No matches found\n"

let () =
    if Array.length (Sys.argv) != 3 then
        print_string ("Usage: " ^ Sys.argv.(0) ^ " regex string\n")
    else
        let regex = Sys.argv.(1) in
        let str = Sys.argv.(2) in
        match Driver.run_matcher_and_dispose_dylib regex (apply_matcher str) with
        | Ok _ -> ()
        | Error err -> failwith err
