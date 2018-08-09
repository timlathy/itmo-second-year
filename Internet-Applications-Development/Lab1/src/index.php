<?php
require __DIR__ . '/../vendor/autoload.php';

$app = new Bullet\App(array(
  'template.cfg' => array('path' => __DIR__ . '/templates/')
));

$app->path('/', function($request) use($app) {
  return $app->template('index');
});

$app->run(new Bullet\Request())->send();
