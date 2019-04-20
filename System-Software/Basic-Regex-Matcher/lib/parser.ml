(* We support a very restricted subset of regular expression syntax based on PCRE:

regex = alternation ;

alternation = quantified expression , { "|" , quantified expression } ;

quantified expression = subexpression , [ quantifier ] ;

quantifier = "?" | "*" | "+" ;

subexpression = "(" , alternation , ")"
              | "[" , character class , "]"
              | character ;

character class = "^" , { character }
                | { character } ;

character = A-Za-z0-9 (* todo: punctuation & special chars *)

*)

let parse_regex _ = failwith "unimplemented"
