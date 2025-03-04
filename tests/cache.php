<?php

namespace Wcc;

//use Wcc\Cache\XattrFile;

require "bootstrap.php";

$svc = Services::instance();

$svc->set("config", new Config());


$file = "tests/routeset.sample.cache";

//$fc = new XattrFile(["cache_dir" => "tests"], Services::instance());

$icd = unserialize(file_get_contents($file));

$file = "tests/routeset2.sample.cache";

$icd = unserialize(file_get_contents($file));
echo "returned unserialize" . PHP_EOL;



$ic = new ICache([], $svc);

$data = $ic->getUnsaved();

echo "got unsaved " . PHP_EOL;

