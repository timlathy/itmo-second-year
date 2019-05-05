open Ctypes

type c_match_group
let c_match_group : c_match_group structure typ = structure "match_group"
let c_group_start = field c_match_group "group_start" int
let c_group_end = field c_match_group "group_end" int
let () = seal c_match_group

type c_match_result
let c_match_result : c_match_result structure typ = structure "match_result"
let c_match_start = field c_match_result "match_start" int
let c_match_end = field c_match_result "match_end" int
let c_group_count = field c_match_result "group_count" int
let c_groups = field c_match_result "groups" (ptr c_match_group)
let () = seal c_match_result

let rec read_groups acc remaining = function
    | _ when remaining = 0 ->
        acc
    | c_ptr ->
        let c_group = !@ c_ptr in
        let group_start = getf c_group c_group_start in
        let group_end = getf c_group c_group_end in
        let acc = (group_start, group_end) :: acc in
        read_groups acc (remaining - 1) (c_ptr +@ 1)

let convert_c_match_result c_result =
    let match_start = getf c_result c_match_start
    and match_end = getf c_result c_match_end in
    match (match_start, match_end) with
    | (0, 0) -> Types.({ matched_range = None; groups = [] })
    | range ->
        let group_count = getf c_result c_group_count in
        let groups = read_groups [] group_count (getf c_result c_groups) in
        Types.({ matched_range = Some range; groups })
