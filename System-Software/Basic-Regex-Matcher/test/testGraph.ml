open OUnit
open TestParser
open Lib

let graph_case (input : Types.expr) (expected : Types.graph_node) = fun () ->
    let actual = Graph.from_expr input in
    assert_equal expected actual ~printer:Types.format_graph_node

let suite = [
    "literal" >:: graph_case (lit "tired") (Node [(CondLiteral "tired", Finish)])
]
