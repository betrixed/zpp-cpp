<?php
namespace Wcc;


$pattern = "/:module/:controller/:action/test/url";

$target = ["_obj" => "TestIndex", "_mth" => "IndexGet"];

$r = new Route(Route::GET_I | Route::POST_I, "/:module/:controller/:action/test/url", 
$target);

$p1 = $r->GetPattern();

//echo "verbs " . $r->GetVerbs() . PHP_EOL;

echo "Compiled " . $r->GetCompiled() . PHP_EOL;

echo "Params " . print_r($r->GetParams(),true) . PHP_EOL;

echo "Verb Int " . Route::GetVerbInt("GET") . PHP_EOL;

//echo "Verbs " . print_r($r->GetVerbNames(),true) . PHP_EOL;

//$hasput = boolstr($r->HasVerb(Route::PUT_I));
//$haspost = boolstr($r->HasVerb(Route::POST_I));
//echo "PUT = $hasput, POST = $haspost" . PHP_EOL;

$data = serialize($r);
echo "Serialized = " . print_r($data,true) . PHP_EOL;
$run = unserialize($data);



