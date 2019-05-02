open OUnit

let suite = "Tests" >::: [
    "Parser" >::: TestParser.suite;
    "Graph" >::: TestGraph.suite
]

let _ = run_test_tt_main suite
