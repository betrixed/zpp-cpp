<?php
//serialfile.php
namespace Wcc;

use Wcc\{CacheMgr,Services};
use Wcc\Cache\DirCache;

require "bootstrap.php";



$services = Services::instance();

$options = [
        'cache_dir' => "tests/_data",
        'dirtree' => false,
        'expire' => 60*10,  
        'defer_write' => false,
    ];

$cache = new DirCache($options, $services);

$key = "somekey";

$cache->set($key, $options, 0);


$data = $cache->get($key);

echo "Got " . print_r($data,true) . PHP_EOL;

$cache->deleteExpired();
