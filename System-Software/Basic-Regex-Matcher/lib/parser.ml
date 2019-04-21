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

type parsing_result = (Types.expr, string) Caml.result

type intermediate_result = (Types.expr * char list, string) Caml.result

let is_character_literal = function
    | 'A' .. 'Z' | 'a' .. 'z' | '0' .. '9' -> true
    | _ -> false

let subexpression = function
    | '(' :: alt -> Error "unimplemented"
    | '[' :: charcls -> Error "unimplemented"
    | charseq ->
        charseq
        |> List.split_while ~f:is_character_literal
        |> (function
            | ([], unparsed) -> Error
                ("Expected one or more character literals, got \"" ^ String.of_char_list unparsed ^ "\"")
            | (seq, rest) -> Ok
                (Types.Sequence (String.of_char_list seq), rest))

let pattern = subexpression

let parse_regex regex =
    regex
    |> String.to_list
    |> pattern
    |> (function
        | Ok (expr, []) -> Ok expr
        | Ok (_, unparsed) -> Error ("Unexpected \"" ^ String.of_char_list unparsed ^ "\" at the end of the expression")
        | Error err -> Error err)
