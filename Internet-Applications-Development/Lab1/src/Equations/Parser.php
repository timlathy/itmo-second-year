<?php
declare(strict_types=1);
namespace Equations;

class ParseException extends \Exception { }

final class Parser {

  /* equation   = mul_div
   *            ;
   * mul_div    = add_sub , [ ( "*" | "/" ) , mul_div ]
   *            ;
   * add_sub    = factor , [ ( "+" | "-" ), add_sub ]
   *            ;
   * factor     = "(" , mul_div , ")"
   *            | number
   *            | variable
   *            ;
   * number     = [-]?[0-9]+([.][0-9]+)?
   *            ;
   * variable   = [A-Za-z]+
   *            ;
   */

  public static function sexpr(string $input): array {
    list($parsed, $remaining_str) = self::mul_div($input);
    if (trim($remaining_str) != "") throw new ParseException(
      "Unexpected '$input' at the end of the equation");

    return $parsed;
  }

  private static function mul_div(string $input): array {
    $lhs = '\Equations\Parser::add_sub';
    $rhs = '\Equations\Parser::mul_div';
    return self::binary_operation($input, $lhs, $rhs,
      ["*" => "multiplication", "/" => "division"]);
  }

  private static function add_sub(string $input): array {
    $lhs = '\Equations\Parser::factor';
    $rhs = '\Equations\Parser::add_sub';
    return self::binary_operation($input, $lhs, $rhs,
      ["+" => "addition", "-" => "subtraction"]);
  }

  private static function factor(string $input): array {
    $parsed = self::literal($input) ?? self::variable($input);
    if (!$parsed) throw new ParseException("Expected a number, a variable,
        or a parenthesized expression starting at '$input'");
    return $parsed;
  }

  private static function literal($input) {
    return self::regex_parse($input, '/^[-]?[0-9]+([.][0-9]+)?/',
      function($parsed) { return ['literal', (float) $parsed]; });
  }

  private static function variable($input) {
    return self::regex_parse($input, '/^[A-Za-z]+/',
      function($parsed) { return ['variable', $parsed]; });
  }

  private static function binary_operation($input, $lhs_fn, $rhs_fn, $ops) {
    list($lhs, $rem) = $lhs_fn($input);
    if (empty($rem)) return [$lhs, $rem];

    if (array_key_exists($rem[0], $ops)) $op = $rem[0];
    else {
      $describe_op = function($op, $name) { return "$name ($op)"; };
      $op_descriptions = array_map($describe_op, array_keys($ops), $ops);
      $expected_ops = join("or", $op_descriptions);

      throw new ParseException("Expected $expected_ops starting at '$input'");
    }

    $rem = ltrim(substr($rem, 1));
    list($rhs, $rem) = $rhs_fn($rem);
    return [[$op, $lhs, $rhs], $rem];
  }

  private static function regex_parse($input, $regex, callable $value_transform) {
    if (preg_match($regex, $input, $matches)) {
      $parsed = $value_transform($matches[0]);
      $remainder = substr($input, strlen($matches[0]));

      return [$parsed, ltrim($remainder)];
    }
  }
}
