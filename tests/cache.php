<?php

namespace Wcc;

//use Wcc\Cache\XattrFile;

require "bootstrap.php";

$svc = Services::instance();

$svc->set("config", new Config());


$file = "tests/routeset.sample.cache";

//$fc = new XattrFile(["cache_dir" => "tests"], Services::instance());

$ic = unserialize(file_get_contents($file));

$file = "tests/routeset2.sample.cache";

$ic = unserialize(file_get_contents($file));
echo "returned unserialize" . PHP_EOL;

echo "print_r " . PHP_EOL;
debug_zval_dump($ic);

