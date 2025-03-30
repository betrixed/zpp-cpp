<?php 
namespace Wcc;

require "bootstrap.php";

function make_object(string $classname, string $alias): object
{
    $services = Services::instance();
    $obj = $services->getObject($classname);
    if ($obj === null)
    {
        $obj = $services->newInstance($classname);
    }

    if ($obj)
    {
        $services->set($alias, $obj);
    }
    return $obj;
}

$data = file_get_contents(__DIR__ . "/sglobal.json");

$_SERVER = json_decode($data,true);

$svc = Services::instance();

$request = make_object(RequestGlobals::class, 'request');

echo debug_zpp_dump($request) . PHP_EOL;

echo "get HttpHost" . PHP_EOL;
$host = $request->getHttpHost();
echo "is $host" . PHP_EOL;

$test = $request->getURI(true);
echo "URI is " . $test . PHP_EOL;