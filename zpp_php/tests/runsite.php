<?php

$testdir = dirname(__DIR__,2) . "/hub/private";

chdir($testdir);

return require("wc/src/Wcc/autoload.php");