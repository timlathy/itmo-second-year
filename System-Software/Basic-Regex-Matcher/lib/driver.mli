val regex_to_c : string -> format_code:bool -> (string, string) Caml.result

type 'a match_handler = (string -> int -> Types.match_result) -> 'a

val run_matcher_and_dispose_dylib : string -> 'a match_handler -> ('a, string) Caml.result
