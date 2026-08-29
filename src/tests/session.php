<?php

namespace Wcc;

//use Wcc\Cache\XattrFile;

require "bootstrap.php";

$ud = new Session\UserData();

$value = "Something";

debug_zpp_dump($ud);

$ud->setKey("formr", $value);

$ret = $ud->getKey("formr");

echo "Key $ret\n";

debug_zpp_dump($ud);