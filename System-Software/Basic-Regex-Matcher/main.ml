open Lib
open Lib.Types

let apply_matcher str matcher =
    match matcher str (String.length str) with
    | { matched_range = None; _ } ->
        print_endline "No matches found"
    | { matched_range = Some (range); groups } ->
        let match_start, match_end = range in
        let _ = print_endline ("Matched from " ^ string_of_int match_start ^ " to " ^ string_of_int match_end) in
        List.iteri (fun i (s, e) -> print_endline (
            "Group #" ^ string_of_int i ^ ": from " ^ string_of_int s ^ " to " ^ string_of_int e)
        ) groups

let () =
    if Array.length (Sys.argv) != 3 then
        print_endline ("Usage: " ^ Sys.argv.(0) ^ " regex string")
    else
        let regex = Sys.argv.(1) in
        let str = Sys.argv.(2) in
        match Driver.run_matcher_and_dispose_dylib regex (apply_matcher str) with
        | Ok _ -> ()
        | Error err -> failwith err
