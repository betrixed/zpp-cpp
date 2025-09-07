<?php

namespace Wcc;
use Wcf\{AbsInt, Alnum, Alpha, BoolVal, Lower, LowerFirst, 
          Regex, Remove, Replace, Special, SpecialFull, StringVal, 
          Striptags, Trim, Upper, UpperFirst, UpperWords, Url};

require "bootstrap.php";

//debug_zval_dump($_SERVER);

//$_SERVER['TEST'] = 1;
echo "_SERVER items = " . count($_SERVER) . PHP_EOL;

$data = [];

$data2 = [];

$dref =& $data;

$d2ref =& $data;

test_wcc();

$data["first key"] = 100;

$value = "-82341";

$filter = new AbsInt();

$nval = $filter($value);

echo "filtered $value = " . $nval . PHP_EOL;

echo print_r($data, true) . PHP_EOL;

$junk = "JUNK-<10ASDF>&\";,,$$$402aa'--'|||ASDF";
$filter = new Alnum();

$sval = $filter($junk);

echo "filter alnum = " . $sval . PHP_EOL;

$filter = new Alpha();

$sval = $filter($junk);

echo "filter alpha = " . $sval . PHP_EOL;

$filter = new BoolVal();

$data = ["false", "true","on", "off", "1", "0", "yes", "no", "n","y"];

foreach($data as $bval)
{
	$test = $filter($bval);
	if ($test===true)
	{
		$result = "true";
	}
	else if ($test===false)
	{
		$result = "false";
	}
	else {
		$result = "undefined";
	}
	echo "$bval is " . $result . PHP_EOL;
}
echo "Done\n";

$filter = new Replace();

$subject = $junk;

echo "replaced " . $filter($subject, "ASDF", "xxxxASDFxxxx") . PHP_EOL;

$filter = new Remove();

echo "removed " . $filter($subject, "ASDF") . PHP_EOL;

$filter = new Regex();

echo "regex " . $filter($subject, "/\\$\\$|aa/", "__" ) . PHP_EOL;

$filter = new Lower();

echo "lower " . $filter($subject) . PHP_EOL;

$filter = new LowerFirst();

echo "lower first " . $filter($subject) . PHP_EOL;

$filter = new Special();

echo "special " . $filter($subject) . PHP_EOL;

$filter = new SpecialFull();

echo "special full " . $filter($subject) . PHP_EOL;

$filter = new StringVal();

echo "StringVal " . $filter($subject) . PHP_EOL;
$filter = new Striptags();

echo "Striptags " . $filter($subject) . PHP_EOL;

$filter = new Trim();
echo "Trim " . $filter("   \n\r\t" . $subject . "\n\r\t ") . PHP_EOL;

$filter = new Upper();
echo "Upper " . $filter("lower " . $subject) . PHP_EOL;

$filter = new UpperFirst();
echo "UpperFirst " . $filter("lower " . $subject) . PHP_EOL;

$filter = new UpperWords();
echo "UpperWords " . $filter("lower word " . $subject) . PHP_EOL;

$filter = new Url();
echo "Url " . $filter($subject) . PHP_EOL;

