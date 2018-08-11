<?php
require __DIR__ . '/../vendor/autoload.php';

use \ArithmExpr\Parser;
use \ArithmExpr\Evaluator;

class GraphBuilderException extends \Exception { }

final class GraphBuilder {
  public $evaluated_lines;

  const REQUIRED_KEYS = [
    "h" => ["x1", "y1", "x2"],
    "v" => ["x1", "y1", "y2"],
    "l" => ["x1", "y1", "x2", "y2"],
    "q" => ["x1", "y1", "x2", "y2", "x3", "y3"]
  ];

  function __construct(array $lines, array $variables) {
    $lines = array_filter($lines, function($line) {
      return array_key_exists("type", $line) &&
        in_array($line["type"], array_keys(self::REQUIRED_KEYS));
    });
    array_walk($lines, function(&$line) use ($variables) {
      $this->evaluate_line($line, $variables);
    });
    $this->evaluated_lines = $lines;
  }

  function build_graph_svg(): string {
    $svg = '<svg xmlns="http://www.w3.org/2000/svg" width="100" height="100">';
    $svg .= '<g fill="none" stroke-width="1px">';
    foreach ($this->evaluated_lines as $line) {
      $this->require_keys_evaluated($line, self::REQUIRED_KEYS[$line["type"]]);
      $svg .= '<path stroke="#000" d="M ' . $line["x1"] . ',' . $line["y1"];
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
      $svg .= '">';
    }
    $svg .= '</g></svg>';
    return $svg;
  }

  private function require_keys_evaluated(array $array, array $keys): void {
    foreach ($keys as $key) {
      if (!is_float($array[$key]))
        throw new GraphBuilderException("$key is not specified");
    }
  }

  private function evaluate_line(array &$line, array $variables): void {
    array_walk($line, function(&$raw_value, $key) use ($variables) {
      switch ($key) {
        case "x1": case "x2": case "x3": case "y1": case "y2": case "y3":
          if (empty($raw_value)) return;
          $raw_value = Evaluator::eval_sexpr(
            Parser::sexpr($raw_value), $variables);
        default:
          break;
      }
    });
  }
}
