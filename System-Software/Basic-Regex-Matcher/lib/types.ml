open Base

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
