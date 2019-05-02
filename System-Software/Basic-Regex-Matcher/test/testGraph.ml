open OUnit
open TestParser
open Lib

let graph_case (input : Types.expr) (expected : Types.graph_node) = fun () ->
    let actual = Graph.from_expr input in
    assert_equal expected actual ~printer:Types.format_graph_node

let suite = [
    "literals" >::
        graph_case (lit "tired") (Node [(CondLiteral "tired", Finish)]);
    "grouping" >::
        graph_case (grp (lit "I see"))
            (GroupStart
                (Node [(CondLiteral "I see",
                    (GroupEnd Finish))]))
]
