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
let ch c : Types.char_class_entry = CharLiteral c

let suite =
    "Parser" >::: [
        "char sequence" >::
            parser_case "greps" (seq [lit "greps"]);
        "simple alternation" >::
            parser_case "rewind|that|its|so|cold" (alt [
                seq [lit "rewind"]; seq [lit "that"]; seq [lit "its"]; seq [lit "so"]; seq [lit "cold"]
            ]);
        "simple grouping" >::
            parser_case "(it|goes)|(it|goes|it|goes)|it|goes" (alt [
                seq [grp (alt [seq [lit "it"]; seq [lit "goes"]])];
                seq [grp (alt [seq [lit "it"]; seq [lit "goes"]; seq [lit "it"]; seq [lit "goes"]])];
                seq [lit "it"]; seq [lit "goes"]
            ]);
        "character classes" >::: [
            "literals" >::
                parser_case "ch[oa]se|that|l[aieou]fe" (alt [
                    seq [lit "ch"; chcls [ch 'o'; ch 'a']; lit "se"];
                    seq [lit "that"];
                    seq [lit "l"; chcls [ch 'a'; ch 'i'; ch 'e'; ch 'o'; ch 'u']; lit "fe"]
                ])
        ]
    ]

let _ = run_test_tt_main suite
