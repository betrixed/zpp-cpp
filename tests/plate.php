<?php

namespace Wcc;

require "bootstrap.php";

$pe = new PlateEngine();

$pe->addNamedPath("a", $wcc_root . "/test");

echo "added path " . PHP_EOL;

$obj = $pe->make("test.phtml",false,null);

$obj->layout("main.phtml",[]);

echo "made template " . PHP_EOL;

$svobj = Services::instance();

$svobj->set("view_model", function ($sv) {
	echo "view_model service called" . PHP_EOL;
	return new Config();
});

$hp = new HtmlPlates("view_model");

$model = $hp->getModel();

$cfg = new Config();

$hp->setModel($cfg);

debug_zval_dump($model);
$obj = null;


$pe = null;


echo "Check Memory now" . PHP_EOL;
