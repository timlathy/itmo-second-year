type expr =
    Alternation of expr list
    | ZeroOrOne of expr
    | ZeroOrMore of expr
    | OneOrMore of expr
    | Grouping of expr
    | CharacterClass of char list
    | NegatedCharacterClass of char list
    | Sequence of string

type parse_result = (expr, string) result

val format_expr : expr -> string

val format_parse_result : parse_result -> string
