open Base
open Types

let rec build_up_to next = function
    | Literal lit ->
        { attrs = []; edges = [CondLiteral lit, next] }
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
    | e ->
        failwith ("unimplemented expr " ^ Types.format_expr e)

let from_expr = build_up_to { attrs = [MatchCompleteNode]; edges = [] }
