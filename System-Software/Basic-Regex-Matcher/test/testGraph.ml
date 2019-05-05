open OUnit
open TestParser
open Lib

let graph_case (input : Types.expr) ~groups (expected : Types.graph_node) = fun () ->
    let actual, actual_groups = Graph.graph_with_group_count input in
    assert_equal groups actual_groups ~printer:string_of_int;
    assert_equal expected actual ~printer:Types.format_graph_node

let node : Types.graph_node = { attrs = []; edges = [] }
let finish : Types.graph_node = { node with attrs = [MatchCompleteNode] }
let stepback : Types.graph_node = { node with attrs = [StepBackNode] }
let groupstart idx : Types.graph_node = { node with attrs = [GroupStartNode idx] }
let endgroupandfinish idx : Types.graph_node = { node with attrs = [GroupEndNode idx; MatchCompleteNode] }

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
    "alternation" >::
        graph_case (alt [lit "a"; grp (lit "bc"); grp (lit "ed"); lit "fg"])  ~groups:2 { node with edges = [
            Unconditional, { node with edges = [
                CondLiteral "a", finish;
                Unconditional, stepback
            ] };
            Unconditional, { (groupstart 0) with edges = [
                CondLiteral "bc", endgroupandfinish 0;
                Unconditional, stepback
            ] };
            Unconditional, { (groupstart 1) with edges = [
                CondLiteral "ed", endgroupandfinish 1;
                Unconditional, stepback
            ] };
            Unconditional, { node with edges = [
                CondLiteral "fg", finish;
                Unconditional, stepback
            ] }
        ] }
]
