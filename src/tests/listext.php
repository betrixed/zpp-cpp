<?php

require "bootstrap.php";

$ext = new ReflectionExtension('Wcc');

$classes = $ext->getClassNames();
asort($classes);
$ord = 0;

foreach($classes as $ix => $name)
{
	$ord++;
	echo $ord . ': ' . $name . PHP_EOL;
}

