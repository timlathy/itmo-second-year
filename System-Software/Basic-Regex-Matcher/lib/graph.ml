open Base
open Types

type intermediate_state = { group_idx: int; attr_stack: (graph_node_attribute list) list; global_alt_idx: int }

let init_state = { group_idx = 0; attr_stack = []; global_alt_idx = 0 }

let char_class_entries_condition (entries: Types.char_class_entry list) = entries
    |> List.map ~f:(function
        | CharLiteral c -> CondLiteral (String.of_char c)
        | CharRange (a, b) -> CondCharInAsciiRange (a, b))
    |> (fun entries -> CondEitherOf entries)

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
        build_nodes_with_stacked_attrs [RepeatingNode] state tail_lazy expr
    | ZeroOrOne expr ->
        build_nodes_with_stacked_attrs [OptionalNode] state tail_lazy expr
    | ZeroOrMore expr ->
        build_nodes_with_stacked_attrs [OptionalNode; RepeatingNode] state tail_lazy expr
    | Sequence [] ->
        tail_lazy state
    | Sequence (head :: rest) ->
        build_tail_nodes state (fun s -> build_tail_nodes s tail_lazy (Sequence rest)) head
    | Grouping grp ->
        let group_end_attr = GroupEndNode state.group_idx in
        let group_end_attrs, state = (match state.attr_stack with
            | attrs :: rest -> group_end_attr :: attrs, { state with attr_stack = rest }
            | _ -> [group_end_attr], state) in
        let group_end = (fun s -> match tail_lazy s with
            | { edges = []; attrs }, s -> { attrs = group_end_attrs @ attrs; edges = [] }, s
            | next, s -> { attrs = group_end_attrs; edges = [Unconditional, next] }, s)
        in let group, s = build_tail_nodes { state with group_idx = state.group_idx + 1 } group_end grp
        in (match group with
            | { attrs = []; edges } ->
                { attrs = [GroupStartNode state.group_idx]; edges }, s
            | _ ->
                { attrs = [GroupStartNode state.group_idx]; edges = [Unconditional, group] }, s)
    | Alternation alts ->
        let alts_num = List.length alts
        in let global_alt_idx, state = state.global_alt_idx, { state with global_alt_idx = state.global_alt_idx + alts_num }
        in let edges, s = List.foldi alts ~init:([], state) ~f:(fun i (acc, s) e ->
            let tail_node, s = build_tail_nodes s tail_lazy e
            in let has_alts_left = i < alts_num - 1
            in let tail_node = if has_alts_left
                then insert_alt_backtrack (global_alt_idx + i + 1) tail_node
                else tail_node
            in ((Unconditional, { tail_node with attrs = tail_node.attrs @ [EnterAlternativeNode (global_alt_idx + i)] }) :: acc), s
        )
        in { attrs = []; edges = List.rev edges }, s
and edges_need_alt_backtrack = function
    | [] -> true
    | [Unconditional, { attrs = [JumpToAlternativeNode _]; _ }] -> false
    | [Unconditional, { attrs; _ }] -> not (Stdlib.List.mem MatchCompleteNode attrs)
    | _ :: rest -> edges_need_alt_backtrack rest
and insert_alt_backtrack alt_idx = function
    | { edges = []; _ } as node ->
        node
    | { edges = [Unconditional, inner]; _ } as node ->
        { node with edges = [Unconditional, insert_alt_backtrack alt_idx inner] }
    | { edges; attrs } ->
        if edges_need_alt_backtrack edges
        then let edges = List.map edges ~f:(fun (c, n) -> c, insert_alt_backtrack alt_idx n)
             in { edges = edges @ [Unconditional, { attrs = [JumpToAlternativeNode alt_idx]; edges = [] }]; attrs }
        else { edges; attrs }
and build_nodes_with_stacked_attrs attrs state tail_lazy expr =
    let state = { state with attr_stack = attrs :: state.attr_stack } in
    let { attrs; edges }, state = build_tail_nodes state tail_lazy expr
    in (match state.attr_stack with
        | unapplied :: rest -> { attrs = unapplied @ attrs; edges }, { state with attr_stack = rest }
        | _ -> { attrs; edges }, state)

let graph_with_group_count e =
    let lazy_finish = fun s -> { attrs = [MatchCompleteNode]; edges = [] }, s in
    let graph, { group_idx; _ } = build_tail_nodes init_state lazy_finish e in
    graph, group_idx
