let read_all_lines ch_in : string =
    let linebuf = Buffer.create 0 in
    let _ = try
        while true do
            ch_in |> input_line |> Buffer.add_string linebuf;
            Buffer.add_string linebuf "\n"
        done
    with End_of_file -> close_in ch_in
    in Buffer.contents linebuf
