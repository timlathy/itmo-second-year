<?php
require __DIR__ . '/../vendor/autoload.php';

$app = new Bullet\App(array(
  'template.cfg' => array('path' => __DIR__ . '/templates/')
));

/* TODO: merge with GraphBuilder */
function parse_js_graph(string $json_graph) {
  $g = json_decode($json_graph);
  if (is_array($g) && count($g) == 2 && is_array($g[1])) {
    $title = $g[0];
    $lines = array_map(function($line) {
      return array_combine(["type", "x1", "x2", "x3", "y1", "y2", "y3"], $line);
    }, $g[1]);
    return [$title, $lines];
  }
}

$app->path('graphs', function() use ($app) {
  $app->path('new', function($request) use ($app) {
    $app->get(function() use ($app) {
      return $app->template('graphs/new');
    });
  });

  $app->path('show', function($request) use ($app) {
    $app->get(function() use ($request, $app) {
      $graph = $request->param('g', '');
      list($title, $lines) = parse_js_graph($graph);
      $name_enc = urlencode($title);

      $vars = array_reduce($lines, function($acc, $exprs) {
        $vs = array_map(function ($expr) {
          if (empty($expr)) return [];
          return \ArithmExpr\Evaluator::sexpr_variables(
            \ArithmExpr\Parser::sexpr($expr));
        }, array_slice($exprs, 1));

        /* https://stackoverflow.com/a/1320259/1726690 */
        $it = new RecursiveIteratorIterator(new RecursiveArrayIterator($vs));
        foreach($it as $var) {
          if (!in_array($var, $acc)) $acc[]=$var;
        }

        return $acc;
      }, []);

      return $app->template('graphs/show', compact('title', 'name_enc', 'vars', 'graph'));
    });
  });

  $app->path('preview', function($request) use ($app) {
    $app->post(function() use($app) {
      $var_declarations = explode(",",
        str_replace(" ", "", $_POST['variables'])
      );
      $variables = array_reduce($var_declarations,
        function(array $acc, string $decl) {
          list($k, $v) = explode("=", $decl, 2);
          $acc[$k] = (float) $v;
          return $acc;
        }, []
      );
      try {
        return (new GraphBuilder($_POST['lines'], $variables))->build_graph_svg();
      }
      catch (\LineException $e) {
        return $app->response(422, $e->js_error_object());
      }
    });
  });

  $app->path('point_in_polygon', function($request) use ($app) {
    $app->get(function() use ($request, $app) {
      $script_start_time = microtime(true);

      list($title, $lines) = parse_js_graph($request->param('g', ''));
      $variables = array_map(function($v) { return (float) str_replace(",", ".", $v); },
        $request->param('v', []));
      $x = array_reduce($request->param('X', []), function($acc, $raw_v) {
        return $acc + (float) $raw_v;
      }, 0);
      $y = (float) str_replace(",", ".", $request->param('Y', ''));
      $params = ["X" => $x, "Y" => $y] + $variables;

      foreach ($variables as $k => $v) {
        if ($k == "R" && ($v < 1 || $v > 4))
          return $app->response(422, "$k is expected to be within a range of [1; 4]");
      }
      if ($y < -5 || $y > 3)
        return $app->response(422, "Y is expected to be within a range of [-5; 3]");

      $x = $x * 10;
      $y = $y * 10;
      $variables = array_map(function ($v) { return $v * 10; }, $variables);

      try {
        $svg = (new GraphBuilder($lines, $variables))->build_graph_svg();
        $is_inside = (new PolygonGraph($svg, 102))->is_point_inside_polygon($x, $y);

        $script_end_time = microtime(true);
        $running_time_micros = round(($script_end_time - $script_start_time) * 1000000);
        $server_time = date('d.m.Y h:i:s a') . " [" . date_default_timezone_get() . "]";

        return $app->template('graphs/point_in_polygon',
          compact('is_inside', 'params', 'graph', 'running_time_micros', 'server_time'));
      }
      catch (\LineException $e) {
        return $app->response(422, $e->js_error_object());
      }
    });
  });
});

$app->path('/', function($request) use($app) {
  return $app->template('index');
});

$app->run(new Bullet\Request())->send();
