<?php
namespace Wcc;

use ReflectionClass;

echo "Start test" . PHP_EOL;

init_globals();
echo "init_globals() returned" . PHP_EOL;
$rc2 = ReflectCache::instance();


//debug_zval_dump($rc2);


function unsetglobals() : void
{
	$list = [];

	foreach ($GLOBALS as $key => $val)
	{
		if (!str_starts_with($key, "_")) {
			$list[] = $key;
		}
	}
	foreach( $list as $key)
	{
		echo "unset " . $key . PHP_EOL;
		unset($GLOBALS[$key]);
	}

}
function showglobals() : void
{
	debug_zval_dump($GLOBALS);
}

//ReflectCache::instance();



echo "Null rc2" . PHP_EOL;

$rc2 = null;

unsetglobals();




$rc2 = ReflectCache::instance();

$rc1 = &$rc2;

$a1 = ["apple", "avocado"];

$a2 = &$a1;

$a2[] = "orange";
$a2[] = "banana";
showglobals();
