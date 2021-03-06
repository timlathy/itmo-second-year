open OUnit
open Lib

let parser_case regex (expected : Types.expr) = fun () ->
    let parsed = Parser.parse_regex regex in
    let expected_result : (Types.expr, string) result = Ok expected in
    assert_equal expected_result parsed ~printer:Types.format_parse_result

let alt a : Types.expr = Alternation a
let seq s : Types.expr = Sequence s
let grp g : Types.expr = Grouping g
let lit l : Types.expr = Literal l
let chcls cc : Types.expr  = CharClass cc
let notchcls cc : Types.expr = NegatedCharClass cc
let ch c : Types.char_class_entry = CharLiteral c
let chrng a b : Types.char_class_entry = CharRange (a, b)

let zeroone e : Types.expr = ZeroOrOne e
let zeromany e : Types.expr = ZeroOrMore e
let onemany e : Types.expr = OneOrMore e

let suite = [
    "char sequence" >::
        parser_case "greps" (lit "greps");
    "simple alternation" >::
        parser_case "rewind|that|its|so|cold" (alt [
            lit "rewind"; lit "that"; lit "its"; lit "so"; lit "cold"
        ]);
    "grouping" >::: [
        "simple" >::
            parser_case "(it|goes)|(it|goes|it|goes)|it|goes" (alt [
                grp (alt [lit "it"; lit "goes"]);
                grp (alt [lit "it"; lit "goes"; lit "it"; lit "goes"]);
                lit "it";
                lit "goes"
            ]);
        "quantified" >::
            parser_case "everything is (up|my?|slee+ves*)+ nothing" (seq [
                lit "everything is ";
                onemany (grp (alt [
                    lit "up";
                    seq [lit "m"; zeroone (lit "y")];
                    seq [lit "sle"; onemany (lit "e"); lit "ve"; zeromany (lit "s")];
                ]));
                lit " nothing"
            ])
        ];
    "character classes" >::: [
        "literals" >::
            parser_case "ch[oa]se|that|l[aieou]fe" (alt [
                seq [lit "ch"; chcls [ch 'o'; ch 'a']; lit "se"];
                lit "that";
                seq [lit "l"; chcls [ch 'a'; ch 'i'; ch 'e'; ch 'o'; ch 'u']; lit "fe"]
            ]);
        "ranges" >::
            parser_case "[A-Hya-h0-89y]" (chcls [
                chrng 'A' 'H'; ch 'y'; chrng 'a' 'h'; chrng '0' '8'; ch '9'; ch 'y'
            ]);
        "negated" >::
            parser_case "[^^A-Z0-9.]" (notchcls [
                ch '^'; chrng 'A' 'Z'; chrng '0' '9'; ch '.'
            ])
        ];
    "quantifiers" >::: [
        "single chars" >::
            parser_case "mid?night+lamp*" (seq [
                lit "mi"; zeroone (lit "d"); lit "nigh"; onemany (lit "t"); lit "lam"; zeromany (lit "p")
            ]);
        "expressions" >::
            parser_case "(stucco)?cav[ei]*|a+" (alt [
                seq [zeroone (grp (lit "stucco")); lit "cav"; zeromany (chcls [ch 'e'; ch 'i'])];
                onemany (lit "a")
            ])
        ];
    ]
