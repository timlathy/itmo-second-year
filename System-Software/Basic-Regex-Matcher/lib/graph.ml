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
        let tail_expr = build_up_to next (Sequence rest) in build_up_to tail_expr head
    | e ->
        failwith ("unimplemented expr " ^ Types.format_expr e)

let from_expr = build_up_to { attrs = [MatchCompleteNode]; edges = [] }
