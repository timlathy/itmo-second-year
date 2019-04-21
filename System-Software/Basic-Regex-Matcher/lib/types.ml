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

let rec format_expr = function
    | Alternation es -> es |> List.map format_expr |> String.concat " | "
    | ZeroOrOne e -> "(? " ^ format_expr e ^ ")"
    | ZeroOrMore e -> "(* " ^ format_expr e ^ ")"
    | OneOrMore e -> "(+ " ^ format_expr e ^ ")"
    | Grouping e -> "(group " ^ format_expr e ^ ")"
    | CharacterClass cs -> "(charclass " ^ Base.String.of_char_list cs  ^ ")"
    | NegatedCharacterClass cs -> "(not charclass " ^ Base.String.of_char_list cs ^ ")"
    | Sequence s -> "\"" ^ s ^ "\""

let format_parse_result = function
    | Ok expr -> format_expr expr
    | Error err -> err
