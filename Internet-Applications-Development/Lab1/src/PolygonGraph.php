<?php
require __DIR__ . '/../vendor/autoload.php';

use SVG\SVG;

final class PolygonGraph {
  private $graph;
  private $max_x;
  private $max_y;

  const ARGB_ALPHA_MASK = 0xFF000000;
  const ARGB_ALPHA_SHIFT = 24;

  function __construct(string $graph_svg, int $max_x, int $max_y) {
    $this->graph = SVG::fromString($graph_svg)->toRasterImage(100, 100);
    $this->max_x = $max_x;
    $this->max_y = $max_y;
  }

  function is_point_inside_polygon(int $x, int $y): bool {
    $abs_x = $x + ($this->max_x / 2);
    $abs_y = $y + ($this->max_y / 2);

    if ($abs_x > $this->max_x || $abs_y > $this->max_y) return false;

    return self::is_point_inside_polygon_abs($abs_x, $abs_y);
  }

  private function is_point_inside_polygon_abs(int $x, int $y): bool {
    $west_bound = false;
    $x_w = $x;
    while (!$west_bound && $x_w >= 0)
      $west_bound = self::is_point_on_line($x_w--, $y);
    if (!$west_bound) return false;

    $east_bound = false;
    $x_e = $x;
    while (!$east_bound && $x_e < $this->max_x)
      $east_bound = self::is_point_on_line($x_e++, $y);
    if (!$east_bound) return false;

    $north_bound = false;
    $y_n = $y;
    while (!$north_bound && $y_n >= 0)
      $north_bound = self::is_point_on_line($x, $y_n--);
    if (!$north_bound) return false;

    $south_bound = false;
    $y_s = $y;
    while (!$south_bound && $y_s < $this->max_y)
      $south_bound = self::is_point_on_line($x, $y_s++);
    return $south_bound;
  }

  private function is_point_on_line(int $x, int $y): bool {
    $color = imagecolorat($this->graph, $x, $y);
    $transparency = ($color & self::ARGB_ALPHA_MASK) >> self::ARGB_ALPHA_SHIFT;
    return $transparency < 127;
  }
}
