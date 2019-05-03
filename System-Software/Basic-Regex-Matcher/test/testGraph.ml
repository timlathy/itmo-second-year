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
        ]};
    "alternation" >::
        graph_case (alt [lit "a"; grp (lit "bc"); grp (lit "ed"); lit "fg"]) { node with edges = [
            Unconditional, { node with edges = [
                CondLiteral "a", finish;
                Unconditional, stepback
            ]};
            Unconditional, { groupstart with edges = [
                CondLiteral "bc", endgroupandfinish;
                Unconditional, stepback
            ] };
            Unconditional, { groupstart with edges = [
                CondLiteral "ed", endgroupandfinish;
                Unconditional, stepback
            ]};
            Unconditional, { node with edges = [
                CondLiteral "fg", finish;
                Unconditional, stepback
            ]}
        ]}
]
