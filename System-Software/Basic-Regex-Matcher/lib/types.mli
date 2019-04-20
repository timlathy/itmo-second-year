type expr =
    Alternation of expr list
    | ZeroOrOne of expr
    | ZeroOrMore of expr
    | OneOrMore of expr
    | Grouping of expr
    | CharacterClass of char list
    | NegatedCharacterClass of char list
    | Character of char

val format_expr : expr -> string
