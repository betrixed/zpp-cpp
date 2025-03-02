<?php
namespace Wcd\Sql;

$services = require("bootstrap.php");




function test_callnew(TColumns $obj)
{
	echo "table " . $obj->getName() . PHP_EOL;
	echo "alias " . $obj->getAlias() . PHP_EOL;
	
	debug_zval_dump($obj->getColNames());
}


test_callnew(new TColumns("test", null, ['*']));


echo "done\n";