open OUnit
open TestParser
open Lib

let graph_case (input : Types.expr) (expected : Types.graph_node) = fun () ->
    let actual = Graph.from_expr input in
    assert_equal expected actual ~printer:Types.format_graph_node

let node: Types.graph_node = { attrs = []; edges = [] }
let finish: Types.graph_node = { node with attrs = [MatchCompleteNode] }
let stepback: Types.graph_node = { node with attrs = [StepBackNode] }
let groupstart: Types.graph_node = { node with attrs = [GroupStartNode] }
let endgroupandfinish: Types.graph_node = { node with attrs = [GroupEndNode; MatchCompleteNode] }

let suite = [
    "literals" >::
        graph_case (lit "tired") { node with edges = [CondLiteral "tired", finish] };
    "grouping" >::
        graph_case (grp (lit "I see")) { groupstart with edges = [
            CondLiteral "I see", endgroupandfinish
        ] };
    "one of characters in a class" >::
        graph_case (chcls [ch 'i'; ch 'a'; ch 'm']) { node with edges = [
            CondEitherOf [CondLiteral "i"; CondLiteral "a"; CondLiteral "m"], finish
        ] };
    "complex character classes" >::
        graph_case (chcls [ch 'i'; chrng 'a' 'z'; ch '9']) { node with edges = [
            CondEitherOf [CondLiteral "i"; CondCharInAsciiRange ('a', 'z'); CondLiteral "9"], finish
        ] };
    "negated character classes" >::
        graph_case (notchcls [ch 'h'; chrng 'E' 'Y'; ch '^']) { node with edges = [
            CondNegated (CondEitherOf [CondLiteral "h"; CondCharInAsciiRange ('E', 'Y'); CondLiteral "^"]), finish
        ] };
    "sequences" >::
        graph_case (seq [lit "what"; chcls [ch 't']; lit "f"]) { node with edges = [
            CondLiteral "what", { node with edges = [
                CondEitherOf [CondLiteral "t"], { node with edges = [
                    CondLiteral "f", finish
                ] }
            ] }
        ] };
    "quantifiers" >::
        graph_case (seq [zeroone (lit "?"); zeromany (lit "*"); onemany (lit "+")]) { attrs = [OptionalNode]; edges = [
            CondLiteral "?", { attrs = [OptionalNode; RepeatingNode]; edges = [
                CondLiteral "*", { attrs = [RepeatingNode]; edges = [CondLiteral "+", finish] }
            ] }
        ] };
    "alternation" >::
        graph_case (alt [lit "a"; grp (lit "bc"); grp (lit "ed"); lit "fg"]) { node with edges = [
            Unconditional, { node with edges = [
                CondLiteral "a", finish;
                Unconditional, stepback
            ] };
            Unconditional, { groupstart with edges = [
                CondLiteral "bc", endgroupandfinish;
                Unconditional, stepback
            ] };
            Unconditional, { groupstart with edges = [
                CondLiteral "ed", endgroupandfinish;
                Unconditional, stepback
            ] };
            Unconditional, { node with edges = [
                CondLiteral "fg", finish;
                Unconditional, stepback
            ] }
        ] }
]
