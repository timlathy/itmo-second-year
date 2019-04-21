open Base

(* We support a very restricted subset of regular expression syntax based on PCRE:

pattern = alternation ;

alternation = quantified expression , { "|" , quantified expression } ;

quantified expression = subexpression , [ quantifier ] ;

quantifier = "?" | "*" | "+" ;

subexpression = "(" , alternation , ")"
              | "[" , character class , "]"
              | { character literal } ;

character class = "^" , { character literal }
                | { character literal } ;

character literal = A-Za-z0-9 (* todo: punctuation & special chars *)

*)

type intermediate =
    NotMatched
    | Matched of Types.expr * char list
    | Failed of string;;

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

let char_sequence input =
    input
    |> List.split_while ~f:is_character_literal
    |> (function
        | ([], _) -> NotMatched
        | (seq, rest) -> Matched (Types.Sequence (String.of_char_list seq), rest))

let rec alternation input = match subexpression input with
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
    and subexpression = function
    | '(' :: a -> a |> alternation |> required ')'
    | '[' :: _ -> Failed "unimplemented"
    | input -> char_sequence input

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
