<?php
declare(strict_types=1);

use Equations\Parser;
use PHPUnit\Framework\TestCase;

final class ParserTest extends TestCase {
  public function test_handles_simple_addition() {
    $this->assertEquals(
      ["+", ["literal", 2.0], ["literal", -2.3]],
      Parser::sexpr("2 + -2.3")
    );
  }
}
