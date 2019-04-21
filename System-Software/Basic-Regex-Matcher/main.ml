open Lib.Types

let () =
    Sequence "h"
    |> format_expr
    |> print_endline
