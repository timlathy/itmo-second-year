open OUnit
open TestParser
open Lib

let graph_case (input : Types.expr) ~groups (expected : Types.graph_node) = fun () ->
    let actual, actual_groups = Graph.graph_with_group_count input in
    assert_equal groups actual_groups ~printer:string_of_int;
    assert_equal expected actual ~printer:Types.format_graph_node

let node : Types.graph_node = { attrs = []; edges = [] }
let finish : Types.graph_node = { node with attrs = [MatchCompleteNode] }
let groupstart idx : Types.graph_node = { node with attrs = [GroupStartNode idx] }
let endgroupandfinish idx : Types.graph_node = { node with attrs = [GroupEndNode idx; MatchCompleteNode] }
let jmpalt idx : Types.graph_node = { node with attrs = [JumpToAlternativeNode idx] }

let suite = [
    "literals" >::
        graph_case (lit "tired") ~groups:0 { node with edges = [CondLiteral "tired", finish] };
    "grouping" >::
        graph_case (grp (lit "I see")) ~groups:1 { (groupstart 0) with edges = [
            CondLiteral "I see", endgroupandfinish 0
        ] };
    "one of characters in a class" >::
        graph_case (chcls [ch 'i'; ch 'a'; ch 'm']) ~groups:0 { node with edges = [
            CondEitherOf [CondLiteral "i"; CondLiteral "a"; CondLiteral "m"], finish
        ] };
    "complex character classes" >::
        graph_case (chcls [ch 'i'; chrng 'a' 'z'; ch '9']) ~groups:0 { node with edges = [
            CondEitherOf [CondLiteral "i"; CondCharInAsciiRange ('a', 'z'); CondLiteral "9"], finish
        ] };
    "negated character classes" >::
        graph_case (notchcls [ch 'h'; chrng 'E' 'Y'; ch '^']) ~groups:0 { node with edges = [
            CondNegated (CondEitherOf [CondLiteral "h"; CondCharInAsciiRange ('E', 'Y'); CondLiteral "^"]), finish
        ] };
    "sequences" >::
        graph_case (seq [lit "th"; grp (chcls [ch 'i'; ch 's']); lit "ron"; grp (lit "withme")]) ~groups:2 {
            node with edges = [
                CondLiteral "th", { (groupstart 0) with edges = [
                    CondEitherOf [CondLiteral "i"; CondLiteral "s"], { attrs = [GroupEndNode 0]; edges = [
                        Unconditional, { node with edges = [
                            CondLiteral "ron", { (groupstart 1) with edges = [
                                CondLiteral "withme", endgroupandfinish 1
                            ] }
                        ] }
                    ] }
                ] }
            ] };
    "quantifiers" >::
        graph_case (seq [zeroone (lit "?"); zeromany (lit "*"); onemany (lit "+")]) ~groups:0 {
            attrs = [OptionalNode]; edges = [
                CondLiteral "?", { attrs = [OptionalNode; RepeatingNode]; edges = [
                    CondLiteral "*", { attrs = [RepeatingNode]; edges = [CondLiteral "+", finish] }
                ] }
            ] };
    "quantified groups" >::
        graph_case (seq [zeromany (grp (seq [lit "*"; chcls [ch 'a']])); onemany (grp (lit "+"))]) ~groups:2 {
            attrs = [GroupStartNode 0]; edges = [
                CondLiteral "*", { attrs = []; edges = [
                    CondEitherOf [CondLiteral "a"], { attrs = [GroupEndNode 0; OptionalNode; RepeatingNode]; edges = [
                        Unconditional, { attrs = [GroupStartNode 1]; edges = [
                            CondLiteral "+", { attrs = [GroupEndNode 1; RepeatingNode; MatchCompleteNode]; edges = [] }
                        ] }
                    ] }
                ] }
            ]
        };
    "alternation" >::
        graph_case (alt [lit "a"; grp (lit "bc"); grp (lit "ed"); lit "fg"]) ~groups:2 { node with edges = [
            Unconditional, { attrs = [EnterAlternativeNode 0]; edges = [
                CondLiteral "a", finish;
                Unconditional, jmpalt 1
            ] };
            Unconditional, { attrs = [GroupStartNode 0; EnterAlternativeNode 1]; edges = [
                CondLiteral "bc", endgroupandfinish 0;
                Unconditional, jmpalt 2
            ] };
            Unconditional, { attrs = [GroupStartNode 1; EnterAlternativeNode 2]; edges = [
                CondLiteral "ed", endgroupandfinish 1;
                Unconditional, jmpalt 3
            ] };
            Unconditional, { attrs = [EnterAlternativeNode 3]; edges = [
                CondLiteral "fg", finish
            ] }
        ] };
    "messy nested alternation" >::
        graph_case (alt [grp (alt [lit "a"; lit "b"; lit "c"]); alt [lit "d"; lit "e"]]) ~groups:1 { node with edges = [
            Unconditional, { attrs = [GroupStartNode 0; EnterAlternativeNode 0]; edges = [
                Unconditional, { attrs = [EnterAlternativeNode 2]; edges = [
                    CondLiteral "a", endgroupandfinish 0;
                    Unconditional, jmpalt 3
                ] };
                Unconditional, { attrs = [EnterAlternativeNode 3]; edges = [
                    CondLiteral "b", endgroupandfinish 0;
                    Unconditional, jmpalt 4
                ] };
                Unconditional, { attrs = [EnterAlternativeNode 4]; edges = [
                    CondLiteral "c", endgroupandfinish 0;
                    Unconditional, jmpalt 1
                ] };
                Unconditional, jmpalt 1 (* this is superfluous but hard to remove *)
            ] };
            Unconditional, { attrs = [EnterAlternativeNode 1]; edges = [
                Unconditional, { attrs = [EnterAlternativeNode 5]; edges = [
                    CondLiteral "d", finish;
                    Unconditional, jmpalt 6
                ] };
                Unconditional, { attrs = [EnterAlternativeNode 6]; edges = [
                    CondLiteral "e", finish;
                ] };
            ] };
        ]};
    "quantified alternation" >::
        graph_case (zeroone (grp (alt [seq [chcls [ch 'a']; lit "b"]; lit "c"; lit "d"])))  ~groups:1 (
            { attrs = [GroupStartNode 0]; edges = [
                Unconditional, { attrs = [EnterAlternativeNode 0]; edges = [
                     CondEitherOf [CondLiteral "a"], { attrs = []; edges = [
                        CondLiteral "b", { attrs = [GroupEndNode 0; OptionalNode; MatchCompleteNode]; edges = [] };
                        Unconditional, jmpalt 1
                    ] };
                    Unconditional, jmpalt 1
                ] };
                Unconditional, { attrs = [EnterAlternativeNode 1]; edges = [
                    CondLiteral "c", { attrs = [GroupEndNode 0; OptionalNode; MatchCompleteNode]; edges = [] };
                    Unconditional, jmpalt 2
                ] };
                Unconditional, { attrs = [EnterAlternativeNode 2]; edges = [
                    CondLiteral "d", { attrs = [GroupEndNode 0; OptionalNode; MatchCompleteNode]; edges = [] };
                ] };
            ] })
]
