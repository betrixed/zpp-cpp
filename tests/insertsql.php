<?php
use Wcc\{
    CacheAll,
    Config,
    Dos,
    Services

};

use Wcc\Cache\SFile;

use Wcd\IServer;

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

$services->setObject($cfg, Config::class);
$services->set('config',$cfg);
$services->set('dos', new Dos());


$finder = $services->get('finder');

$vendor = "../hub/private/vendor";

$finder->addPathArray([
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



$cache = new CacheAll
([
    'delete_expired' => 60*10,
    'expired_key' => 'pcan_delete_expired',
    'fast_cache' => "file_cache", 
    'default_cache' => 'file_cache',
    "defaults" => [
        'prefix' => "unit-test",
        'expire' => 60*60*24,
        'defer_write' => true,
    ],
]);


$cache->createCache
(    
    service_key:"file_cache", 
    class_name:SFile::class, 
    options: [
        'cache_dir' => $cfg->cache_dir . "/sfile",
        'dirtree' => false,
        'expire' => 60*60*40            
    ]
);

$cache->createCache
(    
    service_key:"sess_cache", 
    class_name:SFile::class, 
    options: [
        'cache_dir' => $cfg->cache_dir . "/session",
        'dirtree' => false,
        'expire' => 60*30, 
        'defer_write' => false
    ]
);

$services->set('cache_all', $cache);

$cfg->cache_all = $cache;



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
    echo "Cleared Services\n";
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

echo "Wcc " .  phpversion("wcc") . PHP_EOL;
echo "XDebug " . phpversion("xdebug") . PHP_EOL;