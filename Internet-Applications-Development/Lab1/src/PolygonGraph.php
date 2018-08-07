<?php
require __DIR__ . '/../vendor/autoload.php';

use SVG\SVG;

final class PolygonGraph {
  private $graph;
  private $graph_dim;

  const ARGB_ALPHA_MASK = 0xFF000000;
  const ARGB_ALPHA_SHIFT = 24;

  function __construct(string $graph_svg, int $graph_size) {
    $this->graph_dim = $graph_size;

    $this->graph = SVG::fromString($graph_svg)
      ->toRasterImage($graph_size, $graph_size);
  }

  function is_point_inside_polygon(int $x, int $y): bool {
    $abs_x = ($this->graph_dim / 2) + $x - 1;
    $abs_y = ($this->graph_dim / 2) - $y - 1;

    if ($abs_x >= $this->graph_dim || $abs_y >= $this->graph_dim)
      return false;

    return self::ray_crossings_count($abs_x, $abs_y) == 1;
  }

  private function ray_crossings_count(int $x, int $y): int {
    $is_point_on_edge = self::is_point_on_line($x, $y);
    $isec_count = 0;
    $prev_point_x = -1;
    $adjacent_hits = 0;
    while ($x > 0)
      if (self::is_point_on_line($x--, $y)) {
        /* Skip rasterization artifacts: several adjacent points
         * being on a line should count as a single crossing */
        if ($prev_point_x - 1 != $x) $isec_count++;
        else $adjacent_hits++;

        $prev_point_x = $x;
      }

    if ($isec_count % 2 == 0 && $is_point_on_edge) return 1;
    else if ($adjacent_hits > 4) return $isec_count - 1;
    return $isec_count;
  }

  private function is_point_on_line(int $x, int $y): bool {
    $color = imagecolorat($this->graph, $x, $y);
    $transparency = ($color & self::ARGB_ALPHA_MASK) >> self::ARGB_ALPHA_SHIFT;
    return $transparency < 127;
  }
}
