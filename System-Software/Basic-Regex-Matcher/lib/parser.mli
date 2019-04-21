type intermediate_result = (Types.expr * char list, string) result

val parse_regex : string -> Types.parse_result

val pattern : char list -> intermediate_result

val subexpression : char list -> intermediate_result
