val regex_to_c : string -> (string, string) Caml.result

val run_matcher_and_dispose_dylib : string -> ((string -> int -> bool) -> unit) -> (unit, string) Caml.result
