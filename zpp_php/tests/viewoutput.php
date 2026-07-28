<?php

namespace Wcc;

use Wcc\{Services, ViewOutput, Plate, PlateEngine};

require "bootstrap.php";


$svc = Services::instance(); // auto create in $GLOBALS

$viewpaths = ["tests/views", "tests/more_views"];

$engine = new PlateEngine($viewpaths[1], "phtml");

$engine->setPaths("path", $viewpaths);

$tp = new Plate($engine, "test");

$path = $tp->getPath();

echo "Path is " . $path . PHP_EOL;

$result = $tp->render(["title" => "Monday"]);

echo "render result is " . $result . PHP_EOL;

$content = $tp->section("content");

echo $content . PHP_EOL;






/*

$svc->set('renderfn', $render);





echo print_r($engine,true) . PHP_EOL;


$tp = new Plate($engine, "main");



echo print_r($tp,true) . PHP_EOL;

echo $tp->render(["title" => "TEST"]);

*/