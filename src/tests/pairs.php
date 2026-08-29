<?php
//pairs.php
namespace Wcc;

use ArrayObject;

require "bootstrap.php";

$gResults = [];

$dos = new Dos();

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
		$result = ($b->one + $b->two) / $b->one;
		if ($b->one !== $b->key())
		{
			throw new Exception("property not same as function");
		}
		echo print_r($b, true) . PHP_EOL;
		// (a) goes first, for others to match
		$check = $result;
	}


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


function test_b(int $run)
{
	global $check;

	$count = 1000;
	$b = new Pair(123,2345);
	if ($run===0)
	{
		echo print_r($b, true) . PHP_EOL;
		$result = ($b->first() + $b->second()) / $b->first();
		if ($result !== $check) {
			throw new Exception("Result not the same! $result");
		}
		if ($b->one !== $b->first())
		{
			throw new Exception("property not same as function");
		}
	}



	$start = microtime(true);
	for($ix = 0; $ix < $count; $ix++)
	{
		$result = ($b->first() + $b->second()) / $b->first();
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
	$result = $b->test_calc();
	if ($result !== $check) {
		throw new Exception("Result not the same! $result");
	}

	$start = microtime(true);
	for($ix = 0; $ix < $count; $ix++)
	{
		$result = $b->test_calc();
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
		$result = ($c["key"] + $c["value"]) / $c["key"];
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

$times = [];

for($run = 0; $run < $total_runs; $run++)
{
$a = test_a($run);
$b = test_b($run);
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

if ($run > 0)
{
	$times["a"][] = $a;
	$times["b"][] = $b;
	$times["c"][] = $c;
	$times["d"][] = $d;
	$times["e"][] = $e;
	$times["f"][] = $f;
	$times["g"][] = $g;
	$times["h"][] = $h;
	$times["i"][] = $i;
	$times["j"][] = $j;
	$times["m"][] = $m;
}

row("Local variables (e)", $e/$e, $e/$a, $run);
row("Wcc\\Pair (C++) declared properties (a)", $a/$e, $a/$a, $run);

row("EmptyTest (PHP) declared properties (i)", $i/$e, $i/$a, $run);
row("Wcc\Pair call test_calc() (c)", $c/$e, $c/$a, $run);
row("Wcc\\Pair (C++) methods get (b)", $b/$e, $b/$a, $run);
row("Wcc\\Config dynamic properties (d)", $d/$e, $d/$a, $run);

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
	$sy = stats_standard_deviation($y);

	echo "| $s |" . " " . round($mx,$decp) . " \u{00B1} " . round($sx,$decp) . " |" ;
	echo " " . round($my,$decp) . " \u{00B1} " . round($sy,$decp) . " |" . PHP_EOL;	

}

$avg = [];
foreach($times as $key => $data)
{
	$avg[$key] = array_sum($data) / count($data);
}

$a = $avg["a"];
$b = $avg["b"];
$c = $avg["c"];
$d = $avg["d"];
$e = $avg["e"];
$f = $avg["f"];
$g = $avg["g"];
$h = $avg["h"];
$i = $avg["i"];
$j = $avg["j"];
$m = $avg["m"];

function r2(string $s, float $x, float $y)
{
	global $decp;
	echo "| $s | " . round($x,$decp) . " | " . round($y,$decp) . " |" . PHP_EOL;
}
echo "-----------------" . PHP_EOL;

r2("Local variables (e)", $e/$e, $e/$a);
r2("Wcc\\Pair (C++) declared properties (a)", $a/$e, $a/$a);

r2("EmptyTest (PHP) declared properties (i)", $i/$e, $i/$a);
r2("Wcc\Pair call test_calc() (c)", $c/$e, $c/$a);
r2("Wcc\\Pair (C++) methods get (b)", $b/$e, $b/$a);
r2("Wcc\\Config dynamic properties (d)", $d/$e, $d/$a);

r2("Use local array (g)", $g/$e, $g/$a);

r2("Extend stdClass (h)", $h/$e, $h/$a);

r2("Hmap property handler (f)", $f/$e, $f/$a);
r2("Hmap array handler (m)", $m/$e, $m/$a);

r2("ArrayObject [array]  (k)", $k/$e, $k/$a);
r2("ArrayObject ->Property  (j)", $j/$e, $j/$a);



echo "Versions - PHP " . phpversion() . " Wcc " . phpversion("Wcc") . " XDebug " 
. phpversion("XDebug") . PHP_EOL;
