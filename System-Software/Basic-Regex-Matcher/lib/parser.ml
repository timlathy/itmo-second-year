open Base

(* We support a very restricted subset of regular expression syntax based on PCRE:

pattern = alternation ;

alternation = sequence , { "|" , sequence } ;

sequence = { subexpression , [ quantifier ] }

quantifier = "?" | "*" | "+" ;

subexpression = "(" , alternation , ")"
              | "[" , character class , "]"
              | { character literal } ;

character class = "^" , { character literal | character range }
                | { character literal | character range } ;

character range = character literal , "-" , character literal

character literal = A-Za-z0-9 (* todo: punctuation & special chars *)

*)

type intermediate =
    NotMatched
    | Matched of Types.expr * char list
    | Failed of string;;

module Intermediate = struct
    let map (f: Types.expr -> Types.expr) = function
        | Matched (expr, input) -> Matched (f expr, input)
        | other -> other
end

let is_character_literal = function
    | 'A' .. 'Z' | 'a' .. 'z' | '0' .. '9' -> true
    | _ -> false

let required char = function
    | Matched (expr, c :: rest) when Char.equal c char ->
        Matched (expr, rest)
    | Matched (_, input) ->
        Failed ("Expected \"" ^ String.of_char char ^ "\" starting at \"" ^ String.of_char_list input ^ "\"")
    | failed ->
        failed

let literal input =
    input
    |> List.split_while ~f:is_character_literal
    |> (function
        | ([], _) -> NotMatched
        | (seq, rest) -> Matched (Types.Literal (String.of_char_list seq), rest))

let character_class input =
    let rec character_class_entries = function
        | (acc, a :: '-' :: b :: rest) when not (Char.equal a ']') ->
            let (next_entries, class_end) = character_class_entries (acc, rest)
            in ((Types.CharRange (a, b)) :: next_entries, class_end)
        | (acc, a :: rest) when not (Char.equal a ']') ->
            let (next_entries, class_end) = character_class_entries (acc, rest)
            in ((Types.CharLiteral a) :: next_entries, class_end)
        | (acc, other) ->
            (acc, other)
    in match character_class_entries ([], input) with
    | ([], _) ->
        Failed ("Empty character class encountered at \"" ^ String.of_char_list input ^ "\"")
    | (entries, rest) ->
        Matched ((Types.CharClass entries), rest)

let rec alternation input = match sequence input with
    | Matched (lhs, '|' :: alt) ->
        (match alternation alt with
        | Matched (Alternation rhss, rest) ->
            (* Flatten binary alternation expressions into a single list *)
            Matched (Alternation (lhs :: rhss), rest)
        | Matched (rhs, rest) ->
            Matched (Alternation ([lhs; rhs]), rest)
        | _ ->
            Failed ("Expected an expression after the alternation (|) operator at \"" ^ String.of_char_list alt ^ "\""))
    | other -> other
and sequence input =
    let rec gather_exprs acc input' = match subexpression input' with
        | Matched (expr, rest) ->
            gather_exprs (expr :: acc) rest
        | _ ->
            (acc, input')
    in match gather_exprs [] input with
    | ([], _) ->
        NotMatched
    | (expr :: [], rest) ->
        Matched (expr, rest)
    | (exprs, rest) ->
        Matched (Types.Sequence (List.rev exprs), rest)
and subexpression = function
    | '(' :: a ->
        a |> alternation |> required ')' |> Intermediate.map (fun e -> Grouping e)
    | '[' :: cs ->
        cs |> character_class |> required ']'
    | input ->
        literal input

let pattern = alternation

let parse_regex regex =
    regex
    |> String.to_list
    |> pattern
    |> (function
        | Matched (expr, []) -> Ok expr
        | Matched (_, unparsed) -> Error ("Unexpected \"" ^ String.of_char_list unparsed ^ "\" at the end of the expression")
        | NotMatched -> Error "Invalid syntax"
        | Failed err -> Error err)
