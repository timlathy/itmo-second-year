<?php
require __DIR__ . '/../vendor/autoload.php';

use \ArithmExpr\Parser;
use \ArithmExpr\Evaluator;

final class GraphBuilder {
  public $evaluated_lines;

  const REQUIRED_KEYS = [
    "h" => ["x1", "y1", "x2"],
    "v" => ["x1", "y1", "y2"],
    "l" => ["x1", "y1", "x2", "y2"],
    "q" => ["x1", "y1", "x2", "y2", "x3", "y3"]
  ];

  /* These colors must be mirrored in graph-form.css */
  const LINE_COLORS = [
    "#e5aa20", "#129de4", "#26ab2e", "#e35832",
    "#c21fdb", "#9f3dff", "#23cfc1", "#d7db32"
  ];

  function __construct(array $lines, array $variables) {
    $lines = array_filter($lines, function($line) {
      return array_key_exists("type", $line) &&
        in_array($line["type"], array_keys(self::REQUIRED_KEYS));
    });
    array_walk($lines, function(&$line, $line_i) use ($variables) {
      $this->evaluate_line($line_i, $line, $variables);
    });
    $this->evaluated_lines = $lines;
  }

  function build_graph_svg(): string {
    $svg = '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100">';
    $svg .= '<g fill="none" stroke-width="1px">';
    foreach ($this->evaluated_lines as $line_i => $line) {
      $this->require_keys_evaluated($line_i, $line, self::REQUIRED_KEYS[$line["type"]]);
      $color = self::LINE_COLORS[$line_i];
      $svg .= '<path stroke="' . $color . '" d="M ' . $line["x1"] . ',' . $line["y1"];
      switch ($line["type"]) {
        case "h":
          $svg .= ' H ' . $line["x2"];
          break;
        case "v":
          $svg .= ' V ' . $line["y2"];
          break;
        case "l":
          $svg .= ' L ' . $line["x2"] . ',' . $line["y2"];
          break;
        case "q":
          $svg .= ' Q ' . $line["x2"] . ',' . $line["y2"];
          $svg .= ' ' . $line["x3"] . ',' . $line["y3"];
          break;
      }
      $svg .= '"/>';
    }
    $svg .= '</g></svg>';
    return $svg;
  }

  private function require_keys_evaluated(int $line_i, array $line, array $keys): void {
    foreach ($keys as $k) {
      if (!is_float($line[$k])) throw new LineException("$k is not specified", $line_i);
    }
  }

  private function evaluate_line(int $line_i, array &$line, array $variables): void {
    array_walk($line, function(&$raw_value, $key) use ($variables) {
      switch ($key) {
        case "x1": case "x2": case "x3": case "y1": case "y2": case "y3":
          if (empty($raw_value)) return;
          try {
            $raw_value = Evaluator::eval_sexpr(Parser::sexpr($raw_value), $variables);
          }
          catch (\ArithmExpr\ParseException | \ArithmExpr\EvaluationException $e) {
            throw new \LineException($e->getMessage(), $line_i);
          }
        default:
          break;
      }
    });
  }
}
