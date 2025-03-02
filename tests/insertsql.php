<?php
use Wcc\{
	CacheAll,
    Config,
    Dos,
    ReflectCache,
    Services,
    XmlWrite,
};

use Wcd\{
    IDriver,
    IScript,
    IBuild,
    IServer
};



$basedir = dirname(__DIR__);
chdir($basedir);

echo "Working directory: " . $basedir . PHP_EOL;

$services = require("php/Wcc/autoload.php");

$cfg = new Config();
$cfg->addArray([
	"debug" => false,
	"data_dir" => "tests/_data",
	"cache_dir" => "tests/tmp",
	"models_dir" => "tests/_models",
	"schema_file" => "tests/_data/schema.xml",
	"models_namespace" => "Db\\Models"
]);

echo "Config " . print_r($cfg, true) . PHP_EOL;

$cache_cfg = require("tests/cache_config.php");
$services->setObject($cfg, Config::class);
$services->set('config',$cfg);
$services->set('dos', new Dos());

$cache_all = new CacheAll($cache_cfg);
$services->set('cache_all', $cache_all);

$loader = $services->get('loader');

$vendor = "../hub/private/vendor";

$loader->addPathArray([
    $cfg->models_namespace => $cfg->models_dir,
]);
if (!file_exists($cfg->models_dir)) {
    Dos::make_dir($cfg->models_dir);
}

$dbconfig = require("tests/db_config.php");

$servers =  new IServer('db');
$services->setObject($servers);
$services->set('db',$servers);

$servers->config($dbconfig);

$dbalias = "db1";
$servers->setAlias("default", $dbalias);

require "php/Asserts.php";
require "php/Runner.php";

require "php/SchemaTest.php";
require "php/BaseModelTest.php";
require "php/SqlGenerateTest.php";



$clean = function (Services $svc)
{
    $svc->clearDefer();
    $svc->clearActive();
    $svc->clearObjects();
    echo "Clean\n";
    die;
    return null;
};

$services->set('die', $clean);



$run = new Runner();

$test = new SchemaTest();

$run->runObject($test);

$test = new BaseModelTest();

$run->runObject($test);

$test = new SqlGenerateTest();

$run->runObject($test);

$services->get('die');