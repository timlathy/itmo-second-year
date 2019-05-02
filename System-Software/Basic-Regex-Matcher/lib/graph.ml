open Types

let build_up_to next = function
    | Literal lit ->
        Node ([CondLiteral lit, next])
    | _ ->
        Fail

let from_expr = build_up_to Finish
