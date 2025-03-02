<?php

namespace Wcc;
use Wcc\CArray;

require "bootstrap.php";


$ca = new CArray(CArray::CA_UINT32,10);

for($i = 0; $i < 10; $i++) {
	$ca[$i] = 2*$i;
}

$c = $ca->sub(2,5);
foreach( $c as $key => $value) {
		echo $key . " " . $value . PHP_EOL;
	}