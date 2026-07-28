<?php
use Wcc\{
    CacheMgr,
    Config,
     DebugLog,
    Dos,
    Services

};

use Wcd\Sql\Select;

use function Wcc\debug_zpp_dump;

use Wcc\Cache\DirCache;

use Wcd\IServer;

$basedir = dirname(__DIR__);
chdir($basedir);

echo "Working directory: " . $basedir . PHP_EOL;
echo "Php " . phpversion() . PHP_EOL;
echo "Wcc " .  phpversion("wcc") . PHP_EOL;
echo "XDebug " . phpversion("xdebug") . PHP_EOL;

if ($argc > 1)
{
    $dbalias = $argv[1];
}
else {
    $dbalias = "db1";
}

function bootstrap()
{
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


$debug = new DebugLog("debug_insertsql.log");

DebugLog::setInstance($debug);

$debug->start("<pre>Start");

    $finder = $services->get('finder');

    $vendor = "../hub/private/vendor";

    $finder->addPathArray([
        $cfg->models_namespace => $cfg->models_dir,
        "Pcan\Models" => "tests/pcan_models",
    ]);
    if (!file_exists($cfg->models_dir)) {
        Dos::make_dir($cfg->models_dir);
    }

    $dbconfig = require("tests/db_config.php");

    //$services->setObject($servers);
    $services->set('db',

        function(Services $svc) use($dbconfig) 
        {
            global $dbalias;
            echo "Setup DB for " . $dbalias . PHP_EOL;
            $servers = new IServer('db');
            $servers->config($dbconfig);
            $servers->setAlias("default", $dbalias);
            $svc->setObject($servers);
            $svc->set('db', $servers);
            return $servers;
        }
    );

    $cache = new CacheMgr
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
        class_name:DirCache::class, 
        options: [
            'cache_dir' => $cfg->cache_dir . "/sfile",
            'dirtree' => false,
            'expire' => 60*60*40            
        ]
    );

    $cache->createCache
    (    
        service_key:"sess_cache", 
        class_name:DirCache::class, 
        options: [
            'cache_dir' => $cfg->cache_dir . "/session",
            'dirtree' => false,
            'expire' => 60*30, 
            'defer_write' => false
        ]
    );

    $services->set('cache_mgr', $cache);

    $cfg->cache_mgr = $cache;
    
    $clean = function (Services $svc)
    {
        $svc->clearDefer();
        $svc->clearActive();
        $svc->clearObjects();
        echo "Cleared Services\n";
        return null;
    };

    $services->set('die', $clean);
}




bootstrap();

require "php/Asserts.php";
require "php/Runner.php";

require "php/SchemaTest.php";
require "php/BaseModelTest.php";
require "php/SqlGenerateTest.php";


$run = new Runner();

$run->runObject(new SchemaTest());

$run->runObject(new BaseModelTest());

$run->runObject(new SqlGenerateTest());


Services::service('die');


if(extension_loaded("wccd"))
{
$remains = Select::getAlive();
/*
if (!empty($remains)) {
    echo debug_zpp_dump($remains);
} */

echo "\nremains = " . count($remains) . PHP_EOL;
}