open Types
open Base

let apply_matcher str matcher =
    match matcher str (String.length str) with
    | { matched_range = None; _ } ->
        "No matches found"
    | { matched_range = Some (range); groups } ->
        let match_start, match_end = range in
        let header = "Matched from " ^ Int.to_string match_start ^ " to " ^ Int.to_string (match_end - 1) ^ "\n" in
        List.foldi groups ~init:header ~f:(fun i acc (s, e) -> acc ^
            "Group #" ^ Int.to_string i ^ ": from " ^ Int.to_string s ^ " to " ^ Int.to_string (e - 1) ^ "\n")

let run regex input = Driver.run_matcher_and_dispose_dylib regex (apply_matcher input)
