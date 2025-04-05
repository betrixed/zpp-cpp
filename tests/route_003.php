<?php
namespace Wcc;
use ReflectionClass;


require "bootstrap.php";

$testfile = "$wcc_root" . DIRECTORY_SEPARATOR . "test" . DIRECTORY_SEPARATOR . "routes.php";

$rparser = new RouteParser($testfile, false);

$routes_input = require( $testfile);

$rdata = $rparser->parseRaw($routes_input);

$rset = $rdata["set"];
