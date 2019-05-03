open OUnit

let suite = "Tests" >::: [
    "Parser" >::: TestParser.suite;
    "Graph" >::: TestGraph.suite;
    "Driver" >::: TestDriver.suite
]

let _ = run_test_tt_main suite
