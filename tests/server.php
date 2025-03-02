<?php 
namespace Wcc;

require "bootstrap.php";

$data = file_get_contents(__DIR__ . "/sglobal.json");

$_SERVER = json_decode($data,true);

$svc = Services::instance();

$request = $svc->newInstance(RequestGlobals::class);

echo "get HttpHost" . PHP_EOL;
$host = $request->getHttpHost();
echo "is $host" . PHP_EOL;

$test = $request->getURI(true);
echo "URI is " . $test . PHP_EOL;