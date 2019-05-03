open OUnit
open TestParser
open Lib

let graph_case (input : Types.expr) (expected : Types.graph_node) = fun () ->
    let actual = Graph.from_expr input in
    assert_equal expected actual ~printer:Types.format_graph_node

let node: Types.graph_node = { attrs = []; edges = [] }
let finish: Types.graph_node = { attrs = [MatchCompleteNode]; edges = [] }

let suite = [
    "literals" >::
        graph_case (lit "tired") { node with edges = [CondLiteral "tired", finish] };
    "grouping" >::
        graph_case (grp (lit "I see")) { attrs = [GroupStartNode]; edges = [
            Unconditional, { node with edges = [CondLiteral "I see", {
                attrs = [GroupEndNode; MatchCompleteNode]; edges = []
            }] }
        ]}
]
