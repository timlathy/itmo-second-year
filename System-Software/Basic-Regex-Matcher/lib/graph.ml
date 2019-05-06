open Base
open Types

let char_class_entries_condition (entries: Types.char_class_entry list) = entries
    |> List.map ~f:(function
        | CharLiteral c -> CondLiteral (String.of_char c)
        | CharRange (a, b) -> CondCharInAsciiRange (a, b))
    |> (fun entries -> CondEitherOf entries)

type intermediate_state = { group_idx: int }

let rec build_tail_nodes (state : intermediate_state) (tail_lazy : intermediate_state -> graph_node * intermediate_state) = function
    | Literal lit ->
        let tail, state = tail_lazy state in
        { attrs = []; edges = [CondLiteral lit, tail] }, state
    | CharClass entries ->
        let tail, state = tail_lazy state in
        { attrs = []; edges = [char_class_entries_condition entries, tail] }, state
    | NegatedCharClass entries ->
        let tail, state = tail_lazy state in
        { attrs = []; edges = [CondNegated (char_class_entries_condition entries), tail] }, state
    | OneOrMore expr ->
        let { attrs; edges }, state = build_tail_nodes state tail_lazy expr
        in { attrs = RepeatingNode :: attrs; edges }, state
    | ZeroOrOne expr ->
        let { attrs; edges }, state = build_tail_nodes state tail_lazy expr
        in { attrs = OptionalNode :: attrs; edges }, state
    | ZeroOrMore expr ->
        let { attrs; edges }, state = build_tail_nodes state tail_lazy expr
        in { attrs = OptionalNode :: RepeatingNode :: attrs; edges }, state
    | Sequence [] ->
        tail_lazy state
    | Sequence (head :: rest) ->
        build_tail_nodes state (fun s -> build_tail_nodes s tail_lazy (Sequence rest)) head
    | Grouping grp ->
        let group_end = (fun s -> match tail_lazy s with
        | { edges = []; attrs }, s -> { edges = []; attrs = GroupEndNode state.group_idx :: attrs }, s
        | next, s -> { attrs = [GroupEndNode state.group_idx]; edges = [Unconditional, next] }, s)
        in let group, s = build_tail_nodes { group_idx = state.group_idx + 1 } group_end grp
        in (match group with
        | { attrs = []; edges } ->
            { attrs = [GroupStartNode state.group_idx]; edges }, s
        | _ ->
            { attrs = [GroupStartNode state.group_idx]; edges = [Unconditional, group] }, s)
    | Alternation alt ->
        let return_node = { attrs = [StepBackNode]; edges = [] }
        in let edges, s = alt |> List.fold ~init:([], state) ~f:(fun (acc, s) e ->
            let { attrs; edges }, s = build_tail_nodes s tail_lazy e
            in ((Unconditional, { attrs; edges = edges @ [Unconditional, return_node] }) :: acc), s
        )
        in { attrs = []; edges = List.rev edges }, s

let graph_with_group_count e =
    let lazy_finish = fun s -> { attrs = [MatchCompleteNode]; edges = [] }, s in
    let graph, { group_idx } = build_tail_nodes { group_idx = 0 } lazy_finish e in
    graph, group_idx
