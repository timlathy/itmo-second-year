type expr =
    Symbol of string

let format_expr = function
    Symbol (s) -> Printf.sprintf "(symbol %s)" s