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
