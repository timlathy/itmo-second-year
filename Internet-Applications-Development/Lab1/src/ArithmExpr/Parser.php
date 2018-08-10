<?php
declare(strict_types=1);
namespace ArithmExpr;

class ParseException extends \Exception { }

final class Parser {

  /* equation   = add_sub
   *            ;
   * add_sub    = mul_div , { ( "+" | "-" ) , mul_div }
   *            ;
   * mul_div    = factor , { ( "*" | "/" ), factor }
   *            ;
   * factor     = "(" , add_sub , ")"
   *            | number
   *            | variable
   *            ;
   * number     = [-]?[0-9]+([.][0-9]+)?
   *            ;
   * variable   = [A-Za-z_]+
   *            ;
   */

  public static function sexpr(string $input): array {
    list($parsed, $remaining_str) = self::add_sub(ltrim($input));
    if (!empty($remaining_str)) throw new ParseException(
      "Unexpected '$remaining_str' at the end of the equation");

    return $parsed;
  }

  private static function add_sub(string $input): array {
    return self::binary_op($input, '\ArithmExpr\Parser::mul_div', ["+", "-"]);
  }

  private static function mul_div(string $input): array {
    return self::binary_op($input, '\ArithmExpr\Parser::factor', ["*", "/"]);
  }

  private static function factor(string $input): array {
    if (!empty($input) && $input[0] == "(") {
      list($inner_expr, $rem) = self::add_sub(ltrim(substr($input, 1)));

      $rem = ltrim($rem);

      if (!empty($rem) && $rem[0] == ")")
        return [$inner_expr, ltrim(substr($rem, 1))];
      else
        throw new ParseException("Unbalanced parentheses");
    }

    $parsed = self::literal($input) ?? self::variable($input);
    if (!$parsed) throw new ParseException("Expected a number, a variable, " .
      "or a parenthesized expression " .
      (empty($input) ? "at the end of the equation" : "starting at '$input'"));
    return $parsed;
  }

  private static function literal($input) {
    return self::regex_parse($input, '/^[-]?[0-9]+([.][0-9]+)?/',
      function($parsed) { return ['literal', (float) $parsed]; });
  }

  private static function variable($input) {
    return self::regex_parse($input, '/^[A-Za-z_]+/',
      function($parsed) { return ['variable', $parsed]; });
  }

  private static function binary_op($input, $operand_fn, $ops) {
    list($lhs, $rem) = $operand_fn($input);
    return self::repeat_rhs($rem, $lhs, $operand_fn, $ops) ?? [$lhs, $rem];
  }

  /* Recursively builds up a left-leaning tree
   * given a left hand side operation, an operand parser,
   * and a list of valid single-character operators. */
  private static function repeat_rhs($input, $lhs, $operand_fn, $ops) {
    if (empty($input) || !in_array($input[0], $ops)) return;

    $op = $input[0];
    $rem = ltrim(substr($input, 1));

    list($rhs, $rem) = $operand_fn($rem);
    $lhs = [$op, $lhs, $rhs];

    return self::repeat_rhs($rem, $lhs, $operand_fn, $ops) ?? [$lhs, $rem];
  }

  private static function regex_parse($input, $regex, callable $value_transform) {
    if (preg_match($regex, $input, $matches)) {
      $parsed = $value_transform($matches[0]);
      $remainder = substr($input, strlen($matches[0]));

      return [$parsed, ltrim($remainder)];
    }
  }
}
