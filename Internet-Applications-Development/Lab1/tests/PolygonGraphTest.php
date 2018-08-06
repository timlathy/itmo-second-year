<?php

final class PolygonGraphTest extends PHPUnit\Framework\TestCase {
  const GRAPH_SVG = '<svg xmlns="http://www.w3.org/2000/svg" width="102" height="102">
    <g fill="none" stroke-width="1px">
      <path stroke="#dcdcdc" d="M 50,0 v 100"/>
      <path stroke="#dcdcdc" d="M 0,50 h 100"/>

      <path stroke="#000" d="M 50,50 V 25"/>
      <path stroke="#000" d="M 50,50 H 100"/>
      <path stroke="#000" d="M 100,50 V 75"/>
      <path stroke="#000" d="M 100,75 H 50"/>
      <path stroke="#000" d="M 50,75 L 0,50"/>
      <path stroke="#000" d="M 0,50 H 25"/>
      <path stroke="#000" d="M 25,50 C 25,35 35,25 50,25"/>
    </g>
  </svg>';

  function test_detects_points_inside_polygon() {
    $graph = new PolygonGraph(self::GRAPH_SVG, 100, 100);

    $this->assert_inside($graph, -4, -4);
  }

  function test_detects_points_outside_polygon() {
    $graph = new PolygonGraph(self::GRAPH_SVG, 100, 100);

    $this->assert_outside($graph, 30, 30);
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
