open Base

(* Compiled function result type *)

type match_result =
    { matched_range: (int * int) option;
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
    | GroupStartNode of int
    | GroupEndNode of int
    | StepBackNode
    | OptionalNode
    | RepeatingNode

type graph_node =
    { attrs: graph_node_attribute list;
      edges: (graph_edge_condition * graph_node) list; }

let rec format_graph_node { attrs; edges } =
    let attrs_display = attrs |> List.map ~f:format_graph_attr |> String.concat ~sep: "+" in
    if List.is_empty edges
        then "(" ^ attrs_display ^ ")"
        else let edge_display = edges |> List.map ~f:format_graph_edge |> String.concat ~sep: ", " in
            if List.is_empty attrs
                then "(" ^ edge_display ^ ")"
                else "(" ^ attrs_display ^ ": " ^ edge_display ^ ")"
and format_graph_edge ((cond, next) : (graph_edge_condition * graph_node)) =
    format_grap_edge_cond cond ^ " -> " ^ format_graph_node next
and format_grap_edge_cond = function
    | CondLiteral lit -> "\"" ^ lit ^ "\""
    | CondEitherOf conds ->
        let conds_display = conds |> List.map ~f:format_grap_edge_cond |> String.concat ~sep: " | "
        in "(" ^ conds_display ^ ")"
    | CondNegated cond -> "not (" ^ format_grap_edge_cond cond ^ ")"
    | Unconditional -> "always"
    | _ -> "???"
and format_graph_attr = function
    | OptionalNode -> "option"
    | RepeatingNode -> "repeat"
    | MatchCompleteNode -> "finish"
    | GroupStartNode idx -> "group " ^ Int.to_string idx
    | GroupEndNode idx -> "endgroup " ^ Int.to_string idx
    | StepBackNode -> "stepback"

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

type parse_result = (expr, string) Caml.result

let format_char_class entries =
    entries
    |> List.map ~f:(function
        | CharLiteral c -> "(lit " ^ String.of_char c ^ ")"
        | CharRange (a, b) -> "(range " ^ String.of_char a ^ "-" ^ String.of_char b ^ ")")
    |> String.concat

let rec format_expr = function
    | Alternation es -> es |> List.map ~f:format_expr |> String.concat ~sep:" | "
    | Sequence es ->
        let seq = es |> List.map ~f:format_expr |> String.concat ~sep:" "
        in "(" ^ seq ^ ")"
    | ZeroOrOne e -> "(? " ^ format_expr e ^ ")"
    | ZeroOrMore e -> "(* " ^ format_expr e ^ ")"
    | OneOrMore e -> "(+ " ^ format_expr e ^ ")"
    | Grouping e -> "(group " ^ format_expr e ^ ")"
    | CharClass cc -> "(charclass " ^ format_char_class cc  ^ ")"
    | NegatedCharClass cc -> "(not charclass " ^ format_char_class cc ^ ")"
    | Literal s -> "\"" ^ s ^ "\""

let format_parse_result = function
    | Ok expr -> format_expr expr
    | Error err -> err
