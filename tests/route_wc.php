<?php
namespace Wcc;

use Wcc\{Route,RouteMatch,RouteSet,RouteAdd,Services, Target};

require "bootstrap.php";



$route = new Route(Route::GET_I, "/index.php", ["default", "Home", "indexGET"]);
echo "Made Route" . PHP_EOL;

$r2 = Route::get("/target", Target::go(EmptyTest::class, "func"))->name("default");
echo "Made Route r2" . PHP_EOL;



$rset = new RouteSet();

$radd = new RouteAdd($rset);

$radd->addRoutes([$route, $r2]);

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