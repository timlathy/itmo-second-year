<?php
declare(strict_types=1);

use ArithmExpr\Evaluator;
use PHPUnit\Framework\TestCase;

final class EvaluatorTest extends TestCase {
  public function test_nested_expressions() {
    $expr = 
      ["+",
        ["+",
          ["+", ["literal", 1.0], ["literal", 2.0]],
          ["literal", 3.0]],
        ["literal", 4.0]];
    $this->assertEquals(10.0, Evaluator::eval_sexpr($expr, []));
  }

  public function test_variables_in_expressions() {
    $expr = 
      ["*",
        ["+",
          ["/", ["variable", "R"], ["literal", 2.0]],
          ["literal", 3.0]],
        ["literal", 4.0]];
    $this->assertEquals(20.4, Evaluator::eval_sexpr($expr, ["R" => 4.2]));
  }

  public function test_variable_listing() {
    $expr =
      ["+",
        ["+",
          ["+", ["variable", "R"], ["literal", 3.14]],
          ["variable", "_Case_mishMash"]],
        ["variable", "h"]];
    $this->assertEquals(["R", "_Case_mishMash", "h"],
      Evaluator::sexpr_variables($expr));
  }

  /**
  * @group errors
  */
  public function test_error_division_by_zero() {
    $this->assert_eval_exception(
      ["/", ["literal", 3.1], ["variable", "R"]],
      ["R" => 0.0],
      "Division by zero"
    );
  }

  /**
  * @group errors
  */
  public function test_error_undefined_variable() {
    $this->assert_eval_exception(
      ["/", ["literal", 3.1], ["variable", "R"]],
      ["r" => 0.0],
      "Undefined variable 'R'"
    );
  }

  private function assert_eval_exception($expr, $vars, $expected_msg) {
    try {
      Evaluator::eval_sexpr($expr, $vars);
      $this->fail("Expected an evaluation exception");
    }
    catch (\ArithmExpr\EvaluationException $e) {
      $this->assertEquals($expected_msg, $e->getMessage());
    }
  }
}
