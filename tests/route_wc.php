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



$targ1 = Target::go(EmptyTest::class, "target");
debug_zpp_dump($targ1);


echo "make route\n";
$r1 = Route::get( "/target", $targ1, Route::AJAX_ONLY)->name("default");
debug_zpp_dump($r1);

//0
$list[] = $r1;


//echo "DIE_NOW\n"; return;


$target = Target::go(EmptyTest::class,"indexGET");
$target->setModule("default");
/*
$route = new Route(Route::GET_I, "/", $target);
$route->name("index");
*/
$route = Route::get("/", $target)->name("index");
//1
$list[] = $route;

//2
$list[] = Route::get("/index.php", $target)->name("index.php");

echo "make routeset\n";
$radd = new RouteSet();
$radd->methodSfx("<verb>");


$radd->addRoutes(module:"default", list:$list);

echo "show routeset\n";
debug_zpp_dump($radd);

//echo " rset = " . print_r($rset,true) . PHP_EOL;

$rm = new RouteMatch("/index.php", Route::GET_I, Route::AJAX_ALSO);

$rm2 = new RouteMatch("/target", Route::GET_I, Route::AJAX_ONLY);

$rm3 = new RouteMatch("/admin/dash/cmd/model_cache",Route::GET_I, Route::AJAX_ONLY);

//echo print_r($rm, true) . PHP_EOL;

//debug_zpp_dump($rset);
if ($rm->findRoute($radd)) {
	echo "OK" . PHP_EOL;
}
else {
	echo "Bad" . PHP_EOL;
}
echo "Break " . PHP_EOL;

echo "RM2 = ";
if ($rm2->findRoute($radd))
{
	echo "found" . PHP_EOL;
	$r = $rm2->getMatch();
	echo print_r($r,true) . PHP_EOL;
}
else {
	

	echo "not found" . PHP_EOL;
}

$radd->addRoutes(prefix: "admin", module: "admin", list: [
    Route::get("dash/timeout", Target::go(Dash::class, "timeout"), 
    	Route::AJAX_ONLY)->name("dash.timeout"),
    Route::get("dash", Target::go(Dash::class, "show"))->name("dash.show"),
    Route::get("info", Target::go(Dash::class, "info"))->name("dash.info"),
    Route::get("dash/cmd/:fn", Target::go(Dash::class, "cmd"), 
    	Route::AJAX_ONLY)->name("dash.cmd")
]);

$radd->addRoutes(prefix: "blog", module: "blog", list: [
	Route::get('revisions/:bid', new Target(BlogEdit::class, 'revisions'))->name("revisions.list")
]);


echo "RM3 ";
$rm3->findRoute($radd);
if ($rm3->findRoute($radd))
{
	echo "found" . PHP_EOL;
	$r = $rm3->getMatch();
	echo print_r($r,true) . PHP_EOL;
}
else {
	

	echo "not found" . PHP_EOL;
}

$url = $radd->routeUrl("revisions.list", ['bid' => 1488]);

echo "route url is " . $url . PHP_EOL;

//$services = Services::instance();

//$services->set('route_match', $rm);

//echo print_r($services,true) . PHP_EOL;

