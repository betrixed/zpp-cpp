<?php

namespace Wcc;

use Wcc\Db\IServer;

$services = Services::instance();

$servers =  new IServer('db');
$services->setObject($servers);
$services->set('db',$servers);

$dbconfig = require ("db_config.php");
$servers->config($dbconfig);

$dbalias = "db1"; // db1, ms1, sqlite, fb1, fbfn, pg1, pgfo, from db_config.php

echo "DB alias is " . $dbalias . PHP_EOL;
$servers->setAlias("default", $dbalias);