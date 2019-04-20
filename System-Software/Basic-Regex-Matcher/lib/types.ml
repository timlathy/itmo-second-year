type expr =
    Alternation of expr list
    | ZeroOrOne of expr
    | ZeroOrMore of expr
    | OneOrMore of expr
    | Grouping of expr
    | CharacterClass of char list
    | NegatedCharacterClass of char list
    | Character of char

let rec format_expr = function
    | Alternation (es) -> es |> List.map format_expr |> String.concat " | "
    | ZeroOrOne (e) -> "(? " ^ format_expr e ^ ")"
    | ZeroOrMore (e) -> "(* " ^ format_expr e ^ ")"
    | OneOrMore (e) -> "(+ " ^ format_expr e ^ ")"
    | Grouping (e) -> "(group " ^ format_expr e ^ ")"
    | CharacterClass (cs) -> "(charclass " ^ Base.String.of_char_list cs  ^ ")"
    | NegatedCharacterClass (cs) -> "(not charclass " ^ Base.String.of_char_list cs ^ ")"
    | Character (c) ->  Base.String.of_char c
