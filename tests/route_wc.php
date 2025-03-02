<?php
namespace Wc;

use Wcc\{Route,RouteMatch,RouteSet,Services};


$wc_source_dir = "/home/michael/www/hub/private/wc/src/Wc";

require $wc_source_dir . "/autoload.php";



$route = new Route(Route::GET_I, "/index.php", ["default", "Home", "indexGET"]);
echo "Made Route" . PHP_EOL;

echo print_r($route, true) . PHP_EOL;

$rset = new RouteSet();
$rset->addRoutes([$route]);

echo " rset = " . print_r($rset,true) . PHP_EOL;

$rm = new RouteMatch("/index.php", Route::GET_I, Route::AJAX_ALSO);

echo print_r($rm, true) . PHP_EOL;

if ($rset->match($rm)) {
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