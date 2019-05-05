(* Compiled function result type *)

type match_result =
    { match_found: bool;
      groups: (int * int) list }

(* Regex graph types *)

type graph_edge_condition =
    | CondEitherOf of graph_edge_condition list
    | CondNegated of graph_edge_condition
    | CondLiteral of string
    | CondCharInAsciiRange of char * char
    | Unconditional

type graph_node_attribute =
    | MatchCompleteNode
    | GroupStartNode
    | GroupEndNode
    | StepBackNode
    | OptionalNode
    | RepeatingNode

type graph_node =
    { attrs: graph_node_attribute list;
      edges: (graph_edge_condition * graph_node) list; }

val format_graph_edge : (graph_edge_condition * graph_node) -> string

val format_graph_node : graph_node -> string

(* Regex syntax tree types *)

type char_class_entry =
    CharLiteral of char
    | CharRange of char * char

type expr =
    Alternation of expr list
    | Sequence of expr list
    | ZeroOrOne of expr
    | ZeroOrMore of expr
    | OneOrMore of expr
    | Grouping of expr
    | CharClass of char_class_entry list
    | NegatedCharClass of char_class_entry list
    | Literal of string

type parse_result = (expr, string) result

val format_expr : expr -> string

val format_parse_result : parse_result -> string
