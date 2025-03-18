<?php
//pairs.php
namespace Wcc;

use DS\Pair as dspair;

require "bootstrap.php";

function chop(float $x)
{
	return number_format($x,4);
}

function row(string $s, float $x, float $y)
{
	printf("%-40s %8.4f %8.4f\n", $s, $x, $y);
}

function test_a()
{
	$count = 1000;
	$a = new dspair(123,  2345);

	echo print_r($a, true) . PHP_EOL;
	$result = ($a->key + $a->value) / $a->key;
	echo "sum dspair = " . $result . PHP_EOL;
	
	$start = microtime(true);

	for($ix = 0; $ix < $count; $ix++)
	{
		$result = ($a->key + $a->value) / $a->key;
	}

	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;
	echo "iter = " . chop($itime) . PHP_EOL;
	echo "----------------------------" . PHP_EOL;
	return $itime;
}

function test_d()
{
	$count = 1000;

	$c = new ConfigStd();

	$c->key = 123;
	$c->value = 2345;

	echo print_r($c, true) . PHP_EOL;
	$result = ($c->key + $c->value) / $c->key;
	echo "sum dynamic properties = " . $result . PHP_EOL;
	
	$start = microtime(true);

	for($ix = 0; $ix < $count; $ix++)
	{
		$result = ($c->key + $c->value) / $c->key;
	}

	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;
	echo "iter = " . chop($itime) . PHP_EOL;
	echo "----------------------------" . PHP_EOL;
	return $itime;
}

function test_b()
{
	$count = 1000;
	$b = new Pair(123,2345);

	echo print_r($b, true) . PHP_EOL;
	$result = ($b->key + $b->value) / $b->first;
	echo "sum property handlers = " . $result . PHP_EOL;

	$start = microtime(true);
	for($ix = 0; $ix < $count; $ix++)
	{
		$result = ($b->key + $b->value) / $b->key;
	}
	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;
	echo "iter = " . chop($itime) . PHP_EOL;
	echo "----------------------------" . PHP_EOL;
	return $itime;
}


function test_c()
{
	$count = 1000;
	$b = new Pair(123,2345);

	echo print_r($b, true) . PHP_EOL;
	$result = $b->sum();
	echo "sum C++ function = " . $result . PHP_EOL;

	$start = microtime(true);
	for($ix = 0; $ix < $count; $ix++)
	{
		$result = $b->sum();
	}
	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;
	echo "iter = " . chop($itime) . PHP_EOL;
	echo "----------------------------" . PHP_EOL;
	return $itime;
}

function test_e()
{
	$count = 1000;
	
	$key = 123;
	$value = 2345;
	

	$result = ($key + $value) / $key;

	echo print_r(get_defined_vars(), true) . PHP_EOL;
	echo "locals set once = " . $result . PHP_EOL;
	
	$start = microtime(true);

	for($ix = 0; $ix < $count; $ix++)
	{
		$result = ($key + $value) /$key;
	}

	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;
	echo "iter = " . chop($itime) . PHP_EOL;
	echo "----------------------------" . PHP_EOL;
	return $itime;
}


function test_f()
{
	$count = 1000;

	$c = new Config();

	$c->key = 123;
	$c->value = 2345;

	echo print_r($c, true) . PHP_EOL;
	$result = ($c->key + $c->value) / $c->key;
	echo "Config array magic properties = " . $result . PHP_EOL;
	
	$start = microtime(true);

	for($ix = 0; $ix < $count; $ix++)
	{
		$result = ($c->key + $c->value) / $c->key;
	}

	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;
	echo "iter = " . chop($itime) . PHP_EOL;
	echo "----------------------------" . PHP_EOL;
	return $itime;
}


function test_g()
{
	$count = 1000;

	$c = [];

	$c["key"] = 123;
	$c["value"] = 2345;

	echo print_r($c, true) . PHP_EOL;
	$result = ($c["key"] + $c["value"]) / $c["key"];
	echo "Local array set and lookups = " . $result . PHP_EOL;
	
	$start = microtime(true);

	for($ix = 0; $ix < $count; $ix++)
	{
		//$temp = $c["key"];
		$result = ($c["key"] + $c["value"]) /$c["key"];
		 //$temp;//$c["key"];
	}

	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;
	echo "iter = " . chop($itime) . PHP_EOL;
	echo "----------------------------" . PHP_EOL;
	return $itime;
}


function test_h()
{
	$count = 1000;

	$c = new ConfigStd();

	$d = ["key" => 123, "value"=> 2345];

	$c->data = $d;

	echo print_r($c, true) . PHP_EOL;
	$result = ($c->data["key"] + $c->data["value"]) / $c->data["key"];
	echo "objects  array dynamic property = " . $result . PHP_EOL;
	
	$start = microtime(true);

	for($ix = 0; $ix < $count; $ix++)
	{
		$result = ($c->data["key"] + $c->data["value"]) / $c->data["key"];
	}

	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;
	echo "iter = " . chop($itime) . PHP_EOL;
	echo "----------------------------" . PHP_EOL;
	return $itime;
}

function test_i()
{
	$count = 1000;

	$c = new EmptyTest();

	$d = ["key" => 123, "value"=> 2345];

	$c->empty = $d;

	echo print_r($c, true) . PHP_EOL;
	$result = ($c->empty["key"] + $c->empty["value"]) / $c->empty["key"];
	echo "objects declared array = " . $result . PHP_EOL;
	
	$start = microtime(true);

	for($ix = 0; $ix < $count; $ix++)
	{
		//$temp = $c->empty;
		$result = ($c->empty["key"] + $c->empty["value"]) / $c->empty["key"];
		//$result = ($temp["key"] + $temp["value"]) / $temp["key"];
	}

	$end = microtime(true);

	$itime = (($end - $start) / $count) * 1000_000.0;
	echo "iter = " . chop($itime) . PHP_EOL;
	echo "----------------------------" . PHP_EOL;
	return $itime;
}

$a = test_a();
$b = test_b();
$c = test_c();
$d = test_d();
$e = test_e();
$f = test_f();
$g = test_g();
$h = test_h();
$i = test_i();

row("use locals set once", $e/$e, $e/$a);
row("declared properties fetch", $a/$e, $a/$a);

row("one function call", $c/$e, $c/$a);
row("dynamic properties (stdClass)", $d/$e, $d/$a);

row("Use local array", $g/$e, $g/$a);
row("Use objects declared array", $i/$e, $i/$a);
row("Use objects dynamic array", $h/$e, $h/$a);
row("read_property handler call", $b/$e, $b/$a);
row("use magic method (__get)", $f/$e, $f/$a);
