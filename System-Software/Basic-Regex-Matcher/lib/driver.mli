val regex_to_c : string -> format_code:bool -> (string, string) Caml.result

type match_handler = (string -> int -> Types.match_result) -> unit

val run_matcher_and_dispose_dylib : string -> match_handler -> (unit, string) Caml.result
