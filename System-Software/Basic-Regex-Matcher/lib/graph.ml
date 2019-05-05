open Base
open Types

let char_class_entries_condition (entries: Types.char_class_entry list) = entries
    |> List.map ~f:(function
        | CharLiteral c -> CondLiteral (String.of_char c)
        | CharRange (a, b) -> CondCharInAsciiRange (a, b))
    |> (fun entries -> CondEitherOf entries)

let rec build_up_to next = function
    | Literal lit ->
        { attrs = []; edges = [CondLiteral lit, next] }
    | CharClass entries ->
        { attrs = []; edges = [char_class_entries_condition entries, next] }
    | NegatedCharClass entries ->
        { attrs = []; edges = [CondNegated (char_class_entries_condition entries), next]}
    | Grouping grp ->
        let group_end = match next with
        | { edges = []; attrs } -> { next with attrs = GroupEndNode :: attrs }
        | _ -> { attrs = [GroupEndNode]; edges = [Unconditional, next] }
        in let group = build_up_to group_end grp
        in (match group with
        | { attrs = []; edges } -> { attrs = [GroupStartNode]; edges }
        | _ -> { attrs = [GroupStartNode]; edges = [Unconditional, group] })
    | Alternation alt ->
        let return_node = { attrs = [StepBackNode]; edges = [] }
        in let edges = alt |> List.map ~f:(fun e ->
            let { attrs; edges } = build_up_to next e
            in Unconditional, { attrs; edges = edges @ [Unconditional, return_node] }
        )
        in { attrs = []; edges }
    | Sequence (exp :: []) ->
        build_up_to next exp
    | Sequence (head :: rest) ->
        let tail_expr = build_up_to next (Sequence rest)
        in build_up_to tail_expr head
    | OneOrMore expr ->
        let { attrs; edges } = build_up_to next expr
        in { attrs = RepeatingNode :: attrs; edges }
    | ZeroOrOne expr ->
        let { attrs; edges } = build_up_to next expr
        in { attrs = OptionalNode :: attrs; edges }
    | ZeroOrMore expr ->
        let { attrs; edges } = build_up_to next expr
        in { attrs = OptionalNode :: RepeatingNode :: attrs; edges }
    | e ->
        failwith ("unimplemented expr " ^ Types.format_expr e)

let from_expr = build_up_to { attrs = [MatchCompleteNode]; edges = [] }

let rec recurse_nodes_and_count_groups (count : int) = function
    | { edges = []; _ } ->
        count
    | { attrs; edges; } when Stdlib.List.mem GroupStartNode attrs ->
        recurse_edges_and_count_groups (count + 1) edges
    | { edges; _ } ->
        recurse_edges_and_count_groups count edges
and recurse_edges_and_count_groups count_inside = function
    | [] ->
        count_inside
    | (_, node) :: rest ->
        let count_inside = recurse_nodes_and_count_groups count_inside node in
        recurse_edges_and_count_groups count_inside rest

let group_count graph = recurse_nodes_and_count_groups 0 graph
