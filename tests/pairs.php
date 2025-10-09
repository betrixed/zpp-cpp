<?php
//pairs.php
namespace Wcc;

use ArrayObject;

require "bootstrap.php";

$gResults = [];

function chop(float $x)
{
	return number_format($x,4);
}

function row(string $s, float $x, float $y, int $run)
{
	global $gResults;

	if ($run === 0)
	{
		return;
	}

	$run = $run - 1;

	$gResults[$s][0][$run] = $x;
	$gResults[$s][1][$run] = $y;

	//printf("| %-40s | %8.2f | %8.2f |\n", $s, $x, $y);
}

$check;

function test_a(int $run)
{
	global $check;

	$count = 1000;
	$b = new Pair(123,2345);
	if ($run===0)
	{
		echo print_r($b, true) . PHP_EOL;
	}
	$result = ($b->one + $b->two) / $b->one;
	if ($b->one !== $b->key())
	{
		throw new Exception("property not same as function");
	}
	$check = $result;

	$start = microtime(true);
	for($ix = 0; $ix < $count; $ix++)
	{
		$result = ($b->one + $b->two) / $b->one;
	}
	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;
	if ($run === 0)
	{
		echo "iter = " . chop($itime) . PHP_EOL;
		echo "----------------------------" . PHP_EOL;
	}
	return $itime;
}


function test_c(int $run)
{
	global $check;
	$count = 1000;
	$b = new Pair(123,2345);

	if ($run === 0) {
		echo print_r($b, true) . PHP_EOL;
	}
	$result = $b->sum();
	if ($result !== $check) {
		throw new Exception("Result not the same! $result");
	}

	$start = microtime(true);
	for($ix = 0; $ix < $count; $ix++)
	{
		$result = $b->sum();
	}
	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;
	if ($run === 0) {
		echo "iter = " . chop($itime) . PHP_EOL;
		echo "----------------------------" . PHP_EOL;
	}
	return $itime;
}


function test_d(int $run)
{
	global $check;

	$count = 1000;

	$c = new Config();

	$c->key = 123;
	$c->value = 2345;

	if ($run === 0)
	{
		echo print_r($c, true) . PHP_EOL;
	}
	$result = ($c->key + $c->value) / $c->key;
	if ($result !== $check) {
		throw new Exception("Result not the same! $result");
	}
	
	$start = microtime(true);

	for($ix = 0; $ix < $count; $ix++)
	{
		$result = ($c->key + $c->value) / $c->key;
	}

	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;

	if ($run === 0)
	{
		echo "iter = " . chop($itime) . PHP_EOL;
		echo "----------------------------" . PHP_EOL;
	}
	return $itime;
}


function test_e(int $run)
{
	global $check;

	$count = 1000;
	
	$key = 123;
	$value = 2345;
	

	$result = ($key + $value) / $key;

	if ($run === 0)
	{
		echo print_r(get_defined_vars(), true) . PHP_EOL;
	}
	if ($result !== $check) {
		throw new Exception("Result not the same! $result");
	}
	
	$start = microtime(true);

	for($ix = 0; $ix < $count; $ix++)
	{
		$result = ($key + $value) /$key;
	}

	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;

	if ($run === 0)
	{
		echo "iter = " . chop($itime) . PHP_EOL;
		echo "----------------------------" . PHP_EOL;
	}
	return $itime;
}


function test_f(int $run)
{
	global $check;
	$count = 1000;

	$c = new Hmap();

	$c->key = 123;
	$c->value = 2345;
	$result = ($c->key + $c->value) / $c->key;
	
	if ($run === 0)
	{
		echo print_r($c, true) . PHP_EOL;
		echo "Zend object dynamic propertites = " . $result . PHP_EOL;
		if ($result !== $check) {
			throw new Exception("Result not the same! $result");
		}
	}
	
	
	
	$start = microtime(true);

	for($ix = 0; $ix < $count; $ix++)
	{
		$result = ($c->key + $c->value) / $c->key;
	}

	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;

	if ($run === 0)
	{
		echo "iter = " . chop($itime) . PHP_EOL;
		echo "----------------------------" . PHP_EOL;
	}
	return $itime;
}

function test_m(int $run)
{
	global $check;
	$count = 1000;

	$c = new Hmap();

	$c['key'] = 123;
	$c['value'] = 2345;
	$result = ($c['key'] + $c['value']) / $c['key'];

	if ($run === 0)
	{
		echo print_r($c, true) . PHP_EOL;
		echo "Zend object dynamic propertites = " . $result . PHP_EOL;
		if ($result !== $check) {
			throw new Exception("Result not the same! $result");
		}
	}

	$start = microtime(true);

	for($ix = 0; $ix < $count; $ix++)
	{
		$result = ($c['key'] + $c['value']) / $c['key'];
	}

	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;

	if ($run === 0) 
	{
		echo "iter = " . chop($itime) . PHP_EOL;
		echo "----------------------------" . PHP_EOL;
	}
	return $itime;
}

function test_g(int $run)
{
	global $check;
	$count = 1000;

	$c = [];

	$c["key"] = 123;
	$c["value"] = 2345;
	$result = ($c["key"] + $c["value"]) / $c["key"];
	if ($run === 0)
	{
		echo print_r($c, true) . PHP_EOL;
		echo "Local array set and lookups = " . $result . PHP_EOL;
		if ($result !== $check) {
			throw new Exception("Result not the same! $result");
		}
	}
	
	$start = microtime(true);

	for($ix = 0; $ix < $count; $ix++)
	{
		//$temp = $c["key"];
		$result = ($c["key"] + $c["value"]) /$c["key"];
		 //$temp;//$c["key"];
	}

	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;
	if ($run === 0)
	{
		echo "iter = " . chop($itime) . PHP_EOL;
		echo "----------------------------" . PHP_EOL;
	}
	return $itime;
}


function test_h(int $run)
{
	global $check;
	$count = 1000;

	$c = new ConfigStd();

	$d = ["key" => 123, "value"=> 2345];

	$c->data = $d;
	$result = ($c->data["key"] + $c->data["value"]) / $c->data["key"];

	if ($run === 0)
	{
		echo print_r($c, true) . PHP_EOL;
		echo "Extend stdClass = " . $result . PHP_EOL;
		if ($result !== $check) {
			throw new Exception("Result not the same! $result");
		}
	}
	$start = microtime(true);

	for($ix = 0; $ix < $count; $ix++)
	{
		$result = ($c->data["key"] + $c->data["value"]) / $c->data["key"];
	}

	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;
	if ($run === 0)
	{
		echo "iter = " . chop($itime) . PHP_EOL;
		echo "----------------------------" . PHP_EOL;
	}
	return $itime;
}

function test_i(int $run)
{
	global $check;
	$count = 1000;

	$c = new EmptyTest();
	$c->key = 123;
	$c->value = 2345;
	$result = ($c->key + $c->value) / $c->key;
	if ($run === 0)
	{
		echo print_r($c, true) . PHP_EOL;
	
		echo "objects declared properties = " . $result . PHP_EOL;
		if ($result !== $check) {
			throw new Exception("Result not the same! $result");
		}
	}
	$start = microtime(true);

	for($ix = 0; $ix < $count; $ix++)
	{
		//$temp = $c->empty;
		$result = ($c->key + $c->value) / $c->key;
		//$result = ($temp["key"] + $temp["value"]) / $temp["key"];
	}

	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;
	if ($run === 0)
	{
		echo "iter = " . chop($itime) . PHP_EOL;
		echo "----------------------------" . PHP_EOL;
	}
	return $itime;
}


function test_k(int $run)
{
	global $check;

	$count = 1000;

	$c = new ArrayObject([],  ArrayObject::ARRAY_AS_PROPS);
	//$c->key = 123;
	//$c->value = 2345;

	$c['key'] = 123;
	$c['value'] = 2345;
	$result = ($c['key'] + $c['value']) / $c['key'];
	if ($run === 0)
	{
		echo print_r($c, true) . PHP_EOL;
	//$result = ($c->key + $c->value) / $c->key;
	
		echo "objects declared properties = " . $result . PHP_EOL;
		if ($result !== $check) {
			throw new Exception("Result not the same! $result");
		}
	}
	$start = microtime(true);

	for($ix = 0; $ix < $count; $ix++)
	{
		//$temp = $c->empty;
		//$result = ($c->key + $c->value) / $c->key;
		$result = ($c['key'] + $c['value']) / $c['key'];
		//$result = ($temp["key"] + $temp["value"]) / $temp["key"];
	}

	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;
	if ($run === 0)
	{
		echo "iter = " . chop($itime) . PHP_EOL;
		echo "----------------------------" . PHP_EOL;
	}
	return $itime;
}

function test_j(int $run)
{
	global $check;

	$count = 1000;

	$c = new ArrayObject([],  ArrayObject::ARRAY_AS_PROPS);
	//$c->key = 123;
	//$c->value = 2345;

	$c->key = 123;
	$c->value = 2345;
	$result = ($c->key + $c->value) / $c->key;

	if ($run === 0)
	{
		echo print_r($c, true) . PHP_EOL;
	//$result = ($c->key + $c->value) / $c->key;
	
		echo "objects declared properties = " . $result . PHP_EOL;
		if ($result !== $check) {
			throw new Exception("Result not the same! $result");
		}
	}
	$start = microtime(true);

	for($ix = 0; $ix < $count; $ix++)
	{
		//$temp = $c->empty;
		//$result = ($c->key + $c->value) / $c->key;
		$result = ($c->key + $c->value) / $c->key;
		//$result = ($temp["key"] + $temp["value"]) / $temp["key"];
	}

	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;
	if ($run === 0)
	{
		echo "iter = " . chop($itime) . PHP_EOL;
		echo "----------------------------" . PHP_EOL;
	}
	return $itime;
}

$total_runs = 10000;

for($run = 0; $run < $total_runs; $run++)
{
$a = test_a($run);
$c = test_c($run);
$d = test_d($run);
$e = test_e($run);
$f = test_f($run);
$g = test_g($run);
$h = test_h($run);
$i = test_i($run);
$k = test_k($run);
$j = test_j($run);
$m = test_m($run);

row("Local variables (e)", $e/$e, $e/$a, $run);
row("Wcc\\Pair (C++) declared properties (a)", $a/$e, $a/$a, $run);
row("EmptyTest (PHP) declared properties (i)", $i/$e, $i/$a, $run);
row("Wcc\Pair call sum() (c)", $c/$e, $c/$a, $run);
row("Wcc\\Config dynamic properties", $d/$e, $d/$a, $run);

row("Use local array (g)", $g/$e, $g/$a, $run);

row("Extend stdClass (h)", $h/$e, $h/$a, $run);

row("Hmap property handler (f)", $f/$e, $f/$a, $run);
row("Hmap array handler (m)", $m/$e, $m/$a, $run);

row("ArrayObject [array]  (k)", $k/$e, $k/$a, $run);
row("ArrayObject ->Property  (j)", $j/$e, $j/$a, $run);

}

$decp = 2;

foreach($gResults as $s => $column)
{
	

	$x = $column[0];
	$y = $column[1];



	$mx = array_sum($x) / count($x);
	$sx = stats_standard_deviation($x);
	$my = array_sum($y) / count($y);
	$sy = stats_standard_deviation($x);

	echo "| $s |" . " " . round($mx,$decp) . " \u{00B1} " . round($sx,$decp) . " |" ;
	echo " " . round($my,$decp) . " \u{00B1} " . round($sy,$decp) . " |" . PHP_EOL;	

}


echo "Versions - PHP " . phpversion() . " Wcc " . phpversion("Wcc") . " XDebug " 
. phpversion("XDebug") . PHP_EOL;
