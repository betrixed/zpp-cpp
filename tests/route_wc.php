<?php
namespace Wcc;

use Wcc\{Route,RouteMatch,RouteSet,RouteAdd,Services, Target};
use DateTime;

require "bootstrap.php";

function init_test()
{
$now = new DateTime();
debug_zpp_dump($now);

$str1 = (string) $now->format("Y-M-jS");

$data = ["today" => $str1];

debug_zpp_dump($str1);

$str2 = str_camel("today_only_test");
$str3 = str_uncamel($str2);

debug_zpp_dump($str2);
debug_zpp_dump($str3);

$str4 = str_intern($str1);
debug_zpp_dump($str4);
}

//init_test();

//$targ1 = Target::go(EmptyTest::class, "target");
//$r1 = Route::get( "/target", Target::go(EmptyTest::class, "target"))->name("default");
$s = "STRING " . EmptyTest::class;
debug_zpp_dump($s);


$targ1 = Target::go(EmptyTest::class, "target");
echo "make route\n";
$r1 = Route::get( "/target", $targ1)->name("default");

$list[] = $r1;


$target = Target::go(EmptyTest::class);
/*
$route = new Route(Route::GET_I, "/", $target);
$route->name("index");
*/
$route = Route::get("/", $target)->name("index");
$list[] = $route;

$list[] = Route::get("/index.php", $target)->name("index.php");

echo "make routeset\n";
$radd = new RouteSet();
$radd->methodSfx("<verb>");


$radd->addRoutes(module:"default", list:$list);

echo "show routeset\n";
debug_zpp_dump($radd);

//echo " rset = " . print_r($rset,true) . PHP_EOL;

$rm = new RouteMatch("/index.php", Route::GET_I, Route::AJAX_ALSO);

//echo print_r($rm, true) . PHP_EOL;

//debug_zpp_dump($rset);
if ($rm->findRoute($radd)) {
	echo "OK" . PHP_EOL;
}
else {
	echo "Bad" . PHP_EOL;
}
echo "Break " . PHP_EOL;


//$services = Services::instance();

//$services->set('route_match', $rm);

//echo print_r($services,true) . PHP_EOL;

