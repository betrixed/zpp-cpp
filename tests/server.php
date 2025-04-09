<?php 
namespace Wcc;

require "bootstrap.php";

$test = new Hmap();

$test->value1 = "value1";

$sdata = serialize($test);

//echo "sdata = " . $sdata . PHP_EOL;

//debug_zpp_dump($test);

$cobj = unserialize($sdata);

echo "unserialized " . "value1 = " . $test->value1. PHP_EOL;

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
$_SERVER['HTTP_X_REQUESTED_WITH'] = "XMLHttpRequest";

$svc = Services::instance();

$request = make_object(RequestGlobals::class, 'request');

echo "IS_AJAX = " . (int) $request->isAjax() . PHP_EOL;

//echo debug_zpp_dump($request) . PHP_EOL;

echo "get HttpHost" . PHP_EOL;
$host = $request->getHttpHost();
echo "is $host" . PHP_EOL;

$test = $request->getURI(true);
echo "URI is " . $test . PHP_EOL;

$map = $request->getServer();


echo count($map) . "----\n";

/*
foreach($map as $key => $value)
{
    echo "$key => $value" . PHP_EOL;
}
*/

echo "array " . $map["HTTPS"] . PHP_EOL;

$map["test"] = "write value";
echo "set " . $map["test"] . PHP_EOL;


$serialized = serialize($map);

//echo "serialized " . $serialized . PHP_EOL;

$bmap = unserialize($serialized);



echo "-----dump\n";
//debug_zpp_dump($bmap);
echo "-----\n";

echo "Unregister loader\n";

$loader->unregister();

//echo $loader . PHP_EOL;

$loader = null;

echo "All done\n";