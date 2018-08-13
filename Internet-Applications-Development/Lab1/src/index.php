<?php
require __DIR__ . '/../vendor/autoload.php';

$app = new Bullet\App(array(
  'template.cfg' => array('path' => __DIR__ . '/templates/')
));

$app->path('graphs', function() use ($app) {
  $app->path('new', function($request) use ($app) {
    $app->get(function() use ($app) {
      return $app->template('graphs/new');
    });
  });

  $app->path('show', function($request) use ($app) {
    $app->get(function() use ($request, $app) {
      $graph_def = json_decode($request->param('g', ''));
      if (is_array($graph_def) && count($graph_def) == 2) {
        $title = $graph_def[0];
        $variables = array_reduce($graph_def[1], function($acc, $line) {
          $variables = array_map(function ($expr) {
            if (empty($expr)) return [];
            return \ArithmExpr\Evaluator::sexpr_variables(
              \ArithmExpr\Parser::sexpr($expr));
          }, array_slice($line, 1));

          /* https://stackoverflow.com/a/1320259/1726690 */
          $it = new RecursiveIteratorIterator(
            new RecursiveArrayIterator($variables));
          foreach($it as $var) {
            if (!in_array($var, $acc)) $acc[]=$var;
          }

          return $acc;
        }, []);

        return $app->template('graphs/show', compact('title', 'variables'));
      }
      return 400;
    });
  });

  $app->path('preview', function($request) use ($app) {
    $app->post(function() use($app) {
      $var_declarations = explode(",",
        str_replace(" ", "", $_POST['variables'])
      );
      $variables = array_reduce($var_declarations,
        function (array $acc, string $decl) {
          list($k, $v) = explode("=", $decl, 2);
          $acc[$k] = (float) $v;
          return $acc;
        }, []
      );
      try {
        $gb = new GraphBuilder($_POST['lines'], $variables);
        return $gb->build_graph_svg();
      }
      /* TODO: Client-side error display */
      catch (\ArithmExpr\ParseException $e) {
        return $e->getMessage();
      }
      catch (\ArithmExpr\EvaluationException $e) {
        return $e->getMessage();
      }
    });
  });
});

$app->path('/', function($request) use($app) {
  return $app->template('index');
});

$app->run(new Bullet\Request())->send();
