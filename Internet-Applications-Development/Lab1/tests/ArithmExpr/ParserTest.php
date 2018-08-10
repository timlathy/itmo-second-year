<?php
declare(strict_types=1);

use ArithmExpr\Parser;
use PHPUnit\Framework\TestCase;

final class ParserTest extends TestCase {
  public function test_simple_addition_and_subtraction() {
    $this->assertEquals(
      ["+", ["literal", 2.0], ["literal", -2.3]],
      Parser::sexpr(" 2 + -2.3")
    );
    $this->assertEquals(
      ["-", ["literal", 3.84], ["literal", 1.0]],
      Parser::sexpr("     3.84-1.0 ")
    );
  }

  public function test_repeated_addition() {
    $this->assertEquals(
      ["+",
        ["+",
          ["+",
            ["literal", 1.0],
            ["literal", 2.0]],
          ["literal", 3.0]],
        ["literal", 4.0]],
      Parser::sexpr("1   +2+ 3 +    4")
    );
  }

  public function test_simple_multiplication_and_division() {
    $this->assertEquals(
      ["*", ["literal", -112.9], ["literal", 0.1]],
      Parser::sexpr("             -0112.90 *0.10")
    );
    $this->assertEquals(
      ["/", ["literal", 358.0], ["literal", 14.0]],
      Parser::sexpr("358/14")
    );
  }

  public function test_operator_precedence() {
    $this->assertEquals(
      ["+", ["-", ["literal", 10.0], ["literal", 3.0]], ["literal", 2.0]],
      Parser::sexpr("10 - 3 + 2")
    );
    $this->assertEquals(
      ["+", ["literal", 1.0], ["*", ["literal", 2.0], ["literal", 3.0]]],
      Parser::sexpr("1 + 2 * 3")
    );
    $this->assertEquals(
      ["-",
        ["+",
          ["-",
            ["literal", 4.0],
            ["/",
              ["literal", 12.0],
              ["literal", 2.0]]],
          ["literal", 8.0]],
        ["*",
          ["literal", 3.0],
          ["literal", 6.0]]],
      Parser::sexpr("4 - 12 / 2 + 8 - 3 * 6")
    );
  }

  public function test_parenthesized_expressions() {
    $this->assertEquals(
      ["/",
        ["*",
          ["+", ["literal", 1.0], ["literal", 2.3]],
          ["literal", 3.0]],
        ["-", ["literal", 4.0], ["literal", -5.92]]],
      Parser::sexpr("  ( 1 + 2.3) * 3 /(4--5.92     ) ")
    );
  }

  /**
  * @group errors
  */
  public function test_error_missing_operator() {
    $this->assert_parse_exception("(3 + 5) 5",
      "Unexpected '5' at the end of the equation");
  }

  /**
  * @group errors
  */
  public function test_error_missing_operand() {
    $this->assert_parse_exception("8 * 9 / 5 -",
      "Expected a number, a variable, or a parenthesized expression " .
      "at the end of the equation");
    $this->assert_parse_exception("8 * 9 / 5 - +",
      "Expected a number, a variable, or a parenthesized expression " .
      "starting at '+'");
  }

  /**
  * @group errors
  */
  public function test_error_missing_closing_paren() {
    $this->assert_parse_exception("(3 + 5 - 2", "Unbalanced parentheses");
  }

  /* === utils === */

  private function assert_parse_exception($input, $expected_msg) {
    try {
      Parser::sexpr($input);
      $this->fail("Expected '$input' to cause a parse exception");
    }
    catch (\ArithmExpr\ParseException $e) {
      $this->assertEquals($expected_msg, $e->getMessage());
    }
  }
}
