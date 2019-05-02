open Types

let rec build_up_to next = function
    | Grouping grp ->
        GroupStart (build_up_to (GroupEnd next) grp)
    | Literal lit ->
        Node ([CondLiteral lit, next])
    | _ ->
        failwith "unimplemented"

let from_expr = build_up_to Finish
