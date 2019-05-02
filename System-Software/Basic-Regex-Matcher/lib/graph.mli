type graph_edge_condition =
    CondLiteral of string
    | CondCharInAsciiRange of char * char
    | CondCharAscii of char
    | Unconditional

type graph_node =
    | Node of (graph_edge_condition * graph_node) list
    | Fail
    | Finish

val from_expr : Types.expr -> graph_node

val format_graph_edge : (graph_edge_condition * graph_node) -> string

val format_graph_node : graph_node -> string
