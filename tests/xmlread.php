<?php

namespace Wcc;
use Exception;
//use Wcc\Db\IServer;

require __DIR__ . "/bootstrap.php";

function rutime($ru, $rus, $index){
 return ($ru["ru_$index.tv_sec"]*1000 + intval($ru["ru_$index.tv_usec"]/1000)) - ($rus["ru_$index.tv_sec"]*1000 + intval($rus["ru_$index.tv_usec"]/1000));
}


$rd = new XmlRead();

$testfile3 = "tests/assets.xml";
$testfile1 = "tests/assets_full.xml";
$testfile2 = "tests/.test_secrets.xml";

/*
$config = ReflectCache::staticInstance("Wcc\\Config");

$config->test = [];

$config->test["one"] = "One Value";

debug_zpp_dump($config);

$config = null;

echo "DIES NOW\n"; return;
*/
function testone($testfile)
{
	$data = XmlRead::fromFile($testfile);
	if (empty($data))
	{
	    throw new Exception("File read error");
	}
	//debug_zpp_dump($data);

	/*
	$hmap = $data["assets"];

	debug_zpp_dump($hmap->toArray());
	*/
	$data = null;

	//echo "DIE NOW\n";
	//die;
}

testone($testfile3);

testone($testfile2);

//echo "DIE NOW\n";
//return;
//testone();


//$d = new \DateTime("now");
//echo "Time now " . $d->format("D M Y") . PHP_EOL;


/*$cfg = new Config();


$cfg->property1 = "property value";
echo "cfg property = " . $cfg->property1 . PHP_EOL;
*/


$rd = new XmlRead();

//debug_zval_dump($rd);

function test($testfile) : mixed {

	global $rd, $testfile2;
	echo "CWD is " . \getcwd() . PHP_EOL;

	$s = file_get_contents($testfile);
	$result = $rd->parse($s);

	if (is_bool($result)) {
		echo "parseFile = " . intval($result) . PHP_EOL;
	}

	//unset($result["assets"]);

	//echo debug_zval_dump($result) . "\n";
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

$data = test($testfile1);

if (extension_loaded("wccz"))
{
	debug_zpp_dump($data);
}
else {
	debug_zval_dump($data);
}
//echo "DIES NOW\n"; return;



$data = null;



$start = microtime(true);


function testavg(int $ct, string $msg) {
	$cpu_before = getrusage();

	for($i = 0; $i < $ct; $i++)
	{
		$rd = new XmlRead();
		//$s = file_get_contents("tests/assets_full.xml");
		//$result = $rd->parse($s);
		$rd->parseFile("tests/assets_full.xml");
	}//$emty = new EmptyTest();
	$cpu_after = getrusage();
	echo "$msg CPU usage Per iteration of $ct in \u{00B5}s" . PHP_EOL;

	$user = rutime($cpu_after, $cpu_before, "utime") * 1000.0 / $ct;
	$system = rutime($cpu_after, $cpu_before, "stime")* 1000.0 / $ct;
	$total = $user + $system;

	echo "   User   " . $user . PHP_EOL;
	echo "   System " . $system . PHP_EOL;
	echo "   Total  " . $total . PHP_EOL;
}


testavg(10, "Warm up");
testavg(2000, "Final");

function testavgstr(int $ct, string $msg) {
	$cpu_before = getrusage();
	$rd = new XmlRead();
	$s = file_get_contents("tests/assets_full.xml");
	
	for($i = 0; $i < $ct; $i++)
	{
		//$s = file_get_contents("tests/assets_full.xml");
		//$result = $rd->parse($s);
		$result = $rd->parse($s);
	}//$emty = new EmptyTest();
	$cpu_after = getrusage();
	echo "$msg CPU usage Per iteration of $ct in \u{00B5}s" . PHP_EOL;

	$user = rutime($cpu_after, $cpu_before, "utime") * 1000.0 / $ct;
	$system = rutime($cpu_after, $cpu_before, "stime")* 1000.0 / $ct;
	$total = $user + $system;

	echo "   User   " . $user . PHP_EOL;
	echo "   System " . $system . PHP_EOL;
	echo "   Total  " . $total . PHP_EOL;
}

echo "Test parse of " . $testfile1 . PHP_EOL;
testavgstr(2000, "Parse String");

show_versions();


