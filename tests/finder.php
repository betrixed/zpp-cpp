<?php
namespace Wcc;
require "bootstrap.php";


$test = "Wcc\\ConfigStd";

//$loader->addClass($test, "Wcc/Config.php");

$result = $loader->load($test);

echo "Load result = " . intval($result) . PHP_EOL;

if (empty($result))
{
	echo "paths " . print_r($loader->getNSPaths(), true) . PHP_EOL;
}
$cfg = new Config();

$cfg->property = "test";

echo "cfg property is $cfg->property" . PHP_EOL;

$clist = $loader->getClassPaths();

echo "clist " . print_r($clist,true) . PHP_EOL;