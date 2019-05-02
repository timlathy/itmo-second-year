open Base
open Types

type graph_edge_condition =
    CondLiteral of string
    | CondCharInAsciiRange of char * char
    | CondCharAscii of char
    | Unconditional

type graph_node =
    | Node of (graph_edge_condition * graph_node) list
    | Fail
    | Finish

let build_up_to next = function
    | Literal lit ->
        Node ([CondLiteral lit, next])
    | _ ->
        Fail

let from_expr = build_up_to Finish

let rec format_graph_node = function
    | Fail -> "fail"
    | Finish -> "finish"
    | Node edges -> edges
        |> List.map ~f:format_graph_edge
        |> String.concat ~sep:" "
and format_graph_edge (edge : (graph_edge_condition * graph_node)) = match edge with
    | (CondLiteral lit, next) -> "\"" ^ lit ^ "\" -> " ^ (format_graph_node next)
    | _ -> "unimplemented"
