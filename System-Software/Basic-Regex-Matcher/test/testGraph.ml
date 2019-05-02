open OUnit
open TestParser
open Lib

let graph_case (input : Types.expr) (expected : Graph.graph_node) = fun () ->
    let actual = Graph.from_expr input in
    assert_equal expected actual ~printer:Graph.format_graph_node

let suite = [
    "literal" >:: graph_case (lit "tired") (Node [(CondLiteral "tired", Finish)])
]
