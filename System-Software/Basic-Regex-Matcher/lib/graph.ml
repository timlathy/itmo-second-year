open Base
open Types

let char_class_entries_condition (entries: Types.char_class_entry list) = entries
    |> List.map ~f:(function
        | CharLiteral c -> CondLiteral (String.of_char c)
        | CharRange (a, b) -> CondCharInAsciiRange (a, b))
    |> (fun entries -> CondEitherOf entries)

let rec build_up_to next ~group_idx = function
    | Literal lit ->
        { attrs = []; edges = [CondLiteral lit, next] }, group_idx
    | CharClass entries ->
        { attrs = []; edges = [char_class_entries_condition entries, next] }, group_idx
    | NegatedCharClass entries ->
        { attrs = []; edges = [CondNegated (char_class_entries_condition entries), next]}, group_idx
    | Grouping grp ->
        let group_end = match next with
        | { edges = []; attrs } -> { next with attrs = GroupEndNode group_idx :: attrs }
        | _ -> { attrs = [GroupEndNode group_idx]; edges = [Unconditional, next] }
        in let group, new_group_idx = build_up_to group_end ~group_idx:(group_idx + 1) grp
        in (match group with
        | { attrs = []; edges } ->
            { attrs = [GroupStartNode group_idx]; edges }, new_group_idx
        | _ ->
            { attrs = [GroupStartNode group_idx]; edges = [Unconditional, group] }, group_idx)
    | Alternation alt ->
        let return_node = { attrs = [StepBackNode]; edges = [] }
        in let edges, group_idx = alt |> List.fold ~init:([], group_idx) ~f:(fun (acc, group_idx) e ->
            let { attrs; edges }, group_idx = build_up_to next ~group_idx e
            in ((Unconditional, { attrs; edges = edges @ [Unconditional, return_node] }) :: acc), group_idx
        )
        in { attrs = []; edges = List.rev edges }, group_idx
    | Sequence (exp :: []) ->
        build_up_to next ~group_idx exp
    | Sequence (head :: rest) ->
        let tail_expr, group_idx = build_up_to next ~group_idx (Sequence rest)
        in build_up_to tail_expr ~group_idx head
    | OneOrMore expr ->
        let { attrs; edges }, group_idx = build_up_to next ~group_idx expr
        in { attrs = RepeatingNode :: attrs; edges }, group_idx
    | ZeroOrOne expr ->
        let { attrs; edges }, group_idx = build_up_to next  ~group_idx expr
        in { attrs = OptionalNode :: attrs; edges }, group_idx
    | ZeroOrMore expr ->
        let { attrs; edges }, group_idx = build_up_to next  ~group_idx expr
        in { attrs = OptionalNode :: RepeatingNode :: attrs; edges }, group_idx
    | e ->
        failwith ("unimplemented expr " ^ Types.format_expr e)

let graph_with_group_count = build_up_to { attrs = [MatchCompleteNode]; edges = [] } ~group_idx:0
