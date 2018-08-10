<?php
declare(strict_types=1);
namespace ArithmExpr;

class EvaluationException extends \Exception { }

final class Evaluator {
  public static function eval_sexpr(array $expr, array $variables): float {
    if (count($expr) == 3) {
      $lhs = self::eval_sexpr($expr[1], $variables);
      $rhs = self::eval_sexpr($expr[2], $variables);
      switch ($expr[0]) {
        case "+":
          return $lhs + $rhs;
        case "-":
          return $lhs - $rhs;
        case "*":
          return $lhs * $rhs;
        case "/":
          if ($rhs == 0.0) throw new EvaluationException("Division by zero");
          return $lhs / $rhs;
      }
    }
    switch ($expr[0]) {
      case "literal":
        return $expr[1];
      case "variable":
        $var_name = $expr[1];
        if (array_key_exists($var_name, $variables)) return $variables[$var_name];
        else throw new EvaluationException("Undefined variable '$var_name'");
    }
  } 
}
