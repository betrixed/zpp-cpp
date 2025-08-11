<?php
namespace Wcc;

require "bootstrap.php";

use ArrayAccess;
use stdClass;
use Wcd\IStore;

$values = [ "key1" => "test-1", 
	        "key2" => "test-2", 
			"key3" => "test-3",
		     150 => "xvalue",
		     "float" => 222.5343
		];

$cfg = new Hmap($values);

foreach($cfg as $key => $value)
{
    echo "iterator $key => $value\n";
}

//echo "STOP NOW !!\n"; return;
//$cfg = new stdClass();

$text1 = "Hidden value \"@key2\" inside";

$cfg->values = $values;

$cfg->values["extra"] = "more";

echo "set property 2\n";

$cfg->{"0123"} = $text1;

$cfg->{100} = "One Hundred";
$cfg->{100} = "Two Hundred";

$cfg->set("a100", "one dollar");

$getdef = $cfg->getornot("not there", 1000);


echo "Get or not " . $getdef . PHP_EOL;

$getdef = $cfg->getornot("a100", 1000);

echo "expect get = " . $getdef . PHP_EOL;
$cfg->property = "Rentier class";
$cfg->twice = "Rentier class";
$cfg->twice = "twice";

echo "cfg = "; 
debug_zpp_dump($cfg);


echo "set property : " . $cfg->property . PHP_EOL;
echo "Has Property " . boolstr($cfg->has("property")) . PHP_EOL;

$cfg->text1 = $cfg->unhive($text1) . PHP_EOL;

echo "unhived " . $cfg->text1;

if (class_implements($cfg, "ArrayAccess"))
{
	$cfg[100] = $text1;

	$cfg[0] = "Zero index key";

	$cfg["zero"] = 1000.001;

	echo "not zero = " . $cfg["zero"] . PHP_EOL;

	$cfg[100] = $text1;

	echo "cfg = " . print_r($cfg,true) . PHP_EOL;

	unset($cfg[0]);

	unset($cfg["property"]);
}

$data = ["k1" => "stuff1", "k2" => "stuff2"];

$cfg->odata = $data;

$cfg->odata["k3"] = "New Stuff";

$cfg->name = "Property Hmap";

$vars = $cfg->subset(["key1", "key2"]);

echo "subset = " . print_r($vars,true) . PHP_EOL;
//$cfg->setName("Primary Hmap");

$istore = new IStore();


echo "istore = " . print_r($istore,true) . PHP_EOL;


echo "istore = " . print_r($cfg->toArray(),true) . PHP_EOL;


$ch = new Hmap($cfg->toArray());
$ch->name = "Hmap Property name";

$ch->set('name', "Function set name");
$ch->key1 = "Child dynamic value for key1";

echo "Declared property name: " . $ch->name . PHP_EOL;
echo "Dynamic property name: " . $ch->get('name') . PHP_EOL;

debug_zpp_dump($ch);

$data = serialize($ch);

echo "serialized " . $data .PHP_EOL;

$obj = unserialize($data);


debug_zpp_dump($obj);

echo "unserialized name " . $obj->name . PHP_EOL;

$has = isset($ch["absent"]);

echo "is set " . intval($has) . PHP_EOL;

$missed = $ch->absent ?? null;

echo "Missing property absent " . intval(is_null($missed)) . PHP_EOL;