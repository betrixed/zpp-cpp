<?php
namespace Wcc;

require "bootstrap.php";

$hg = new HtmlGem();

echo "Made new HtmlGem" . PHP_EOL;


$m = HtmlGem::moneyFormat("en_AU");

echo $m->language() . PHP_EOL;

echo $m->format(12550) . PHP_EOL;

//money.php