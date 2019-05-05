open Ctypes

type c_match_result
val c_match_result : c_match_result structure typ

val convert_c_match_result : c_match_result structure -> Types.match_result
