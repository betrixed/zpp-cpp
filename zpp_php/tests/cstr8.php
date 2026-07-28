<?php

$s = new Str8("Hello world, Καλημέρα κόσμε, コンニチハ" );

echo "$s" . PHP_EOL;

foreach($s as $key => $value)
{
	echo "$key $value" . PHP_EOL;
}