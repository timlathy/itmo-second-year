<?php

final class PolygonGraphTest extends PHPUnit\Framework\TestCase {
  const GRAPH_SVG = '<svg xmlns="http://www.w3.org/2000/svg" width="100" height="100">
    <g fill="none" stroke-width="1px">
      <path stroke="#000" d="M 49,49 V 24"/>
      <path stroke="#000" d="M 49,49 H 99"/>
      <path stroke="#000" d="M 99,49 V 74"/>
      <path stroke="#000" d="M 99,74 H 49"/>
      <path stroke="#000" d="M 49,74 L 0,49"/>
      <path stroke="#000" d="M 0,49 H 24"/>
      <path stroke="#000" d="M 24,49 Q 24,24 49,24"/>
    </g>
  </svg>';

  function test_detects_points_inside_polygon() {
    $graph = new PolygonGraph(self::GRAPH_SVG, 100);

    $this->assert_inside($graph, -8, -10);
    $this->assert_inside($graph, -4, 9);
    $this->assert_inside($graph, 30, -12);
  }

  function test_detects_points_outside_polygon() {
    $graph = new PolygonGraph(self::GRAPH_SVG, 100);

    for ($x = 1; $x < 50; $x++)
      for ($y = 1; $y < 50; $y++)
        $this->assert_outside($graph, $x, $y);

    for ($x = -49; $x < 50; $x++)
      for ($y = -26; $y > -50; $y--)
        $this->assert_outside($graph, $x, $y);

    for ($y = -25; $y < -1; $y++)
      for ($x = -49; $x < -(25 + $y) * 2; $x++)
        $this->assert_outside($graph, $x, $y);
  }

  function test_detects_points_on_polygon_edges() {
    $graph = new PolygonGraph(self::GRAPH_SVG, 100);

    $this->assert_inside($graph, 0, 0);
    $this->assert_inside($graph, 50, -2);
    $this->assert_inside($graph, -25, 3);
  }

  private function assert_inside($graph, $x, $y) {
    $this->assertTrue($graph->is_point_inside_polygon($x, $y),
      "Expected ($x, $y) to be inside the polygon");
  }
  
  private function assert_outside($graph, $x, $y) {
    $this->assertNotTrue($graph->is_point_inside_polygon($x, $y),
      "Expected ($x, $y) to be outside the polygon");
  }
}
