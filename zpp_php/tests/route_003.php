<?php
namespace Wcc;
use ReflectionClass;
use Wc\Valid;


require "bootstrap.php";

$testfile = "tests/routes.php";

$rparser = new RouteParser($testfile, false);

$routes_input = require( $testfile);

$rdata = $rparser->parseRaw($routes_input);

$rset = $rdata["set"];

$clues = ["name" => "My Name"];

$name = Valid::toStr($clues, "name");

echo "Valid name $name" . PHP_EOL;