<?php
namespace Wcc;

use ReflectionClass;

require "bootstrap.php";

echo "route_002.php" . PHP_EOL;




$testfile =  "tests/routes.php";

echo "testfile $testfile\n";

$rparser = new RouteParser($testfile, false);

$routes_input = require( $testfile);

echo "parse " . $testfile . PHP_EOL;

$rdata = $rparser->parseRaw($routes_input);

echo "route data " . print_r($rdata,true) . PHP_EOL;
$serdata = serialize($rdata);

$rdata = unserialize($serdata);

$rset = $rdata["set"];



//echo debug_zval_dump($rset,true) . PHP_EOL;

echo "Loop" . PHP_EOL;

$uritable = [
	["/index.php", Route::GET_I, Route::AJAX_NONE],
	["/article/my_title", Route::GET_I, Route::AJAX_NONE],
	["/login/login", Route::POST_I, Route::AJAX_NONE],
    ["/images/album/2024",Route::GET_I, Route::AJAX_NONE]
];

echo "Ready" . PHP_EOL;


foreach($uritable as $uri)
{
	echo "Match " . $uri[0] . PHP_EOL;
	$request = new RouteMatch($uri[0], $uri[1], $uri[2]);
	if(!$request->findRoute($rset))
	{
		echo "route match fail " . $request->getUri() . " " . $request->getVerb() . PHP_EOL;
	}
	else {
		echo "Found route" . PHP_EOL;
		//$request->prepare_call();
		/*$params = $request->getObjArgs();
		if (count($params) > 0)
		{
			echo "args = " . print_r($params, true) . PHP_EOL;
		}*/
	}
}




