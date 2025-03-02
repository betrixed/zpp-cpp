<?php
namespace Wcc;
require "bootstrap.php";


$test = "Wcc\\Config";

//$loader->addClass($test, "Wcc/Config.php");

$result = $loader->load($test);

echo "Load result = " . intval($result) . PHP_EOL;

$cfg = new Config();

$cfg->property = "test";

echo "cfg property is $cfg->property" . PHP_EOL;

$clist = $loader->getClassPaths();

echo "clist " . print_r($clist,true) . PHP_EOL;