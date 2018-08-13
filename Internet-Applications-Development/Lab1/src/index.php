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
