<?php

namespace Wcc;

use Wcd\IServer;
use Wcc\{DebugLog,Dos};
use Wcd\Schema\CSV;
use Wcc\{CacheMgr, ReadCache};

require "bootstrap.php";

$path = "tests/database-pdo_pgsql.schema.xml";

$log = new DebugLog("tests/output.log", 
		DebugLog::TO_CONSOLE);
$log->setInstance($log);

if (!file_exists($path))
{
	echo "cwd " . getcwd() . PHP_EOL;
	echo "File not found: $path" . PHP_EOL;
}
$schema = XmlRead::fromFile($path);


