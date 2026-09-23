<?php

namespace Wcc;
use Wcp\XmlRead;

function rutime($end, $start, $index) : float 
{
	// 
	$after_sec = $end["ru_$index.tv_sec"];
	$after_usec = $end["ru_$index.tv_usec"];

	$before_sec = $start["ru_$index.tv_sec"];
	$before_usec = $start["ru_$index.tv_usec"];


	echo "Start $before_sec $before_usec ";
	echo "End   $after_sec $after_usec " . PHP_EOL;
	
	$msec = 1.0e3;
 
 return (float) ($after_sec*$msec + $after_usec/$msec) - (float) ($before_sec*$msec + $before_usec/$msec);
}


use Exception;
//use Wcc\Db\IServer;

require __DIR__ . "/bootstrap.php";

$testfile1 = "tests/assets.xml";
$testfile2 = "tests/assets_full.xml";
$rd = new XmlRead();


/*
$config = ReflectCache::staticInstance("Wcc\\Config");

$config->test = [];

$config->test["one"] = "One Value";

debug_zpp_dump($config);

$config = null;

echo "DIES NOW\n"; return;
*/
function testone()
{
	global $rd, $testfile1;
	$data = $rd->parseFile($testfile1);
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

testone();
testone();

//echo "DIE NOW\n";
//return;
//testone();


//$d = new \DateTime("now");
//echo "Time now " . $d->format("D M Y") . PHP_EOL;


/*$cfg = new Config();


$cfg->property1 = "property value";
echo "cfg property = " . $cfg->property1 . PHP_EOL;
*/


//debug_zval_dump($rd);

function test() : mixed {

	global $rd;
	echo "CWD is " . \getcwd() . PHP_EOL;

	$result = $rd->parseFile("tests/assets_full.xml");

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

$data = test();
debug_zpp_dump($data);
//echo "DIES NOW\n"; return;



$data = null;



$start = microtime(true);


function testavg(int $ct, string $msg) {
	global $testfile2;

	$cpu_before = getrusage();

	//$start = microtime(true);
	for($i = 0; $i < $ct; $i++)
	{
		$rd = new XmlRead();
		//$s = file_get_contents("tests/assets_full.xml");
		//$result = $rd->parse($s);
		$rd->parseFile($testfile2);
	}//$emty = new EmptyTest();
	$cpu_after = getrusage();
	//$end = microtime(true);

	echo "$msg CPU usage (msec) Per iteration of $ct in msec" . PHP_EOL;

	$user = rutime($cpu_after, $cpu_before, "utime") / $ct;
	$system = rutime($cpu_after, $cpu_before, "stime")/ $ct;
	$total = $user + $system;

	echo "   User   " . $user . PHP_EOL;
	echo "   System " . $system . PHP_EOL;
	echo "   Total  " . $total . PHP_EOL;

	//$mtime =  ($end - $start)*1000.0/$ct;

	//echo "microtime = " . number_format($mtime,3) . " msec" . PHP_EOL;
}


echo "Test file is " . $testfile2 . PHP_EOL;
testavg(10, "Warm up");
testavg(200, "Final");
testavg(2000,"Lots");

show_versions();




