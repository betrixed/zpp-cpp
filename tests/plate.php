<?php

namespace Wcc;

use Wcc\{SearchList, Plate, PlateEngine};

require "bootstrap.php";

$pe = new PlateEngine();

$slist = new SearchList();

$slist->addPath($wcc_root . "/test");

$pe->setFinder($slist);

echo "call newPlate\n";

$obj = $pe->newPlate("test.phtml",false);

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

