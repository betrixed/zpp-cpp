<?php

require "bootstrap.php";

$extlist = ['wccz', 'wccr', 'wcch', 'wccm', 'wccd', 'runsa'];
$all = [];

foreach($extlist as $ename)
{
	$e2 = new ReflectionExtension($ename);
	$all = array_merge($all, $e2->getClassNames());
}

asort($all);
$ord = 0;
foreach($all as $ix => $name)
{
	$ord++;
	echo $ord . ': ' . $name . PHP_EOL;
}


