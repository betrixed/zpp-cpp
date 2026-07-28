<?php

namespace Wcc;
use Exception;
//use Wcc\Db\IServer;

require __DIR__ . "/bootstrap.php";

$data = XmlRead::fromFile("tests/assets_full.xml");
if (empty($data))
{
    throw new Exception("File read error");
}
//$d = new \DateTime("now");
//echo "Time now " . $d->format("D M Y") . PHP_EOL;


/*$cfg = new Config();


$cfg->property1 = "property value";
echo "cfg property = " . $cfg->property1 . PHP_EOL;
*/


$rd = new XmlRead();


function test() : mixed {

	global $rd;
	echo "CWD is " . \getcwd() . PHP_EOL;

	$s = file_get_contents("tests/assets_full.xml");
	$result = $rd->parse($s);

	if (is_bool($result)) {
		echo "parseFile = " . intval($result) . PHP_EOL;
	}

	echo debug_zval_dump($result) . "\n";
	/**
	$s = file_get_contents("tests/test.xml");
	$f2 = $rd->parse($s);

	if (is_bool($f2)) {
		echo "parseFile = " . intval($f2) . PHP_EOL;
	}
	else {
		//echo print_r($f2,true) . PHP_EOL;
	} 

	return $f2;*/
	return $result;
}

$data = test();


$start = microtime(true);


function testavg(int $ct, string $msg) {
	$start = microtime(true);
	for($i = 0; $i < $ct; $i++)
	{
		$rd = new XmlRead();
		$rd->parseFile("tests/assets_full.xml");
	}//$emty = new EmptyTest();
	$end = microtime(true);

	$total = (($end - $start)*1000_1000) / $ct;

	//debug_zval_dump($data);

	echo $msg . ": " . number_format($total,0) . " microsecs per iteration ($ct)" . PHP_EOL;
}

echo "Wcc version is " . phpversion("Wcc") . PHP_EOL;

testavg(10, "Warm up");
testavg(2000, "Final");




