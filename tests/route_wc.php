<?php
namespace Wcc;

use Wcc\{Route,RouteMatch,RouteSet,RouteAdd,Services, Target};

require "bootstrap.php";


$rset = new RouteSet();

$radd = new RouteAdd($rset);
$radd->methodSfx("<verb>");

 $target = Target::go(EmptyTest::class);

$list = [];

$list[] = Route::get("/", $target)->name("index");

$list[] = Route::get("/target", Target::go(EmptyTest::class, "target"))->name("default");

$list[] = Route::get("/index.php", $target)->name("index.php");

$radd->addRoutes(module:"default", list:$list);


echo " rset = " . print_r($rset,true) . PHP_EOL;

$rm = new RouteMatch("/index.php", Route::GET_I, Route::AJAX_ALSO);

echo print_r($rm, true) . PHP_EOL;

debug_zpp_dump($rset);
if ($rm->findRoute($rset)) {
	echo "OK" . PHP_EOL;
}
else {
	echo "Bad" . PHP_EOL;
}
echo "Break " . PHP_EOL;
die;
$services = Services::instance();

$services->set('route_match', $rm);

echo print_r($services,true) . PHP_EOL;