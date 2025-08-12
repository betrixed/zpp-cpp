php<?php
namespace Wcc;

use ReflectionClass;

function boolstr(bool $v) : string
{
	return $v ? "True" : "False";
}
$pattern = "/:module/:controller/:action/test/url";

$target = ["_obj" => "TestIndex", "_mth" => "IndexGet"];




$r = new Route(
	Route::GET_I | Route::POST_I, 
	$pattern, 
	["_obj" => "TestIndex", "_mth" => "IndexGet"]
);


$r->SetPattern("one");
$r->SetPattern("two");
$r->SetCompiled("Three");

$r->SetAjax(Route::AJAX_ALSO);

echo "Allow None " . boolstr($r->Allow(Route::GET_I, Route::AJAX_NONE)) . PHP_EOL;

echo "Allow Ajax " . boolstr($r->Allow(Route::GET_I, Route::AJAX_ONLY)) . PHP_EOL;



echo "Compiled " . $r->GetCompiled() . PHP_EOL;
echo "Patttern " . $r->GetPattern() . PHP_EOL;

echo "verbs " . $r->GetVerbs() . PHP_EOL;

echo "Compiled " . $r->GetCompiled() . PHP_EOL;

debug_zpp_dump($r);

echo "Params " . print_r($r->GetParams(),true) . PHP_EOL;
//echo "Params2 " . print_r($r->GetParams(),true) . PHP_EOL;
//echo "Params3 " . print_r($r->GetParams(),true) . PHP_EOL;

$method = "GET";
echo "Verb GET " . Route::GetVerbInt($method) . PHP_EOL;
echo "Verb get " . Route::GetVerbInt("get") . PHP_EOL;
echo "Verb post " . Route::GetVerbInt("post") . PHP_EOL;
echo "Verb put " . Route::GetVerbInt("put") . PHP_EOL;



$names = Route::GetVerbNames(Route::PUT_I | ROUTE::POST_I);

$myverbs = $r->GetVerbs();
$hasput = boolstr($r->HasVerb(Route::PUT_I));
$haspost = boolstr($r->HasVerb(Route::POST_I));
echo "PUT = $hasput, POST = $haspost" . PHP_EOL;

echo "Got names" . PHP_EOL;
echo "Verbs " . print_r($names,true) . PHP_EOL;



//echo "Params4 " . print_r($r->GetParams(),true) . PHP_EOL;
$data = serialize($r);

echo "Dump data - ";
debug_zpp_dump($data);
//echo "Serialize = " . print_r($data,true) . PHP_EOL;



$run = unserialize($data);

//echo "Unserialize run 1 = " . print_r($run, true) . PHP_EOL;

//echo "Unserialize run 2 = " . print_r($run, true) . PHP_EOL;

debug_zpp_dump($run);
echo "Params " . print_r($run->GetParams(),true) . PHP_EOL;



$set = [];

$set[] = $r;
$set[] = $run;

//$r->addOneMore = "value";
//$run->other = "other";

//$run = null;
echo "Set count = " . count($set) . PHP_EOL;

foreach($set as $ix => $rset) {
	echo "Route $ix " . print_r($rset,true) . PHP_EOL;
}






