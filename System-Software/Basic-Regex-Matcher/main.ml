let () =
    if Array.length (Sys.argv) != 3 then
        print_endline ("Usage: " ^ Sys.argv.(0) ^ " regex string")
    else
        let regex = Sys.argv.(1) and input = Sys.argv.(2) in
        match Lib.Cli.run regex input with
        | Ok result -> print_string result
        | Error err -> failwith err
