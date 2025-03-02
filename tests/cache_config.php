<?php

return
[
    'delete_expired' => 1800,
    'fast_cache' => "data_cache",
    "names" => [
        'data' => "SFile",
        'file' => "SFile",
        'sql' => "SFile",
        'sess' => "Sess",
        ],
    "defaults" => [
        'prefix' => "test",
        'expire' => 86400,
        'defer-write' => false,
        'keep-local' => false,
        ],
    "settings" => [
        "Memcached" => [
            'class' => 'Wcc\Cache\Libmemcached',
            'port' => 11211,
            'host' => "localhost",
        ],
        "Apcu" => [
            'class' => 'Wcc\Cache\Apcu',
        ],
        "SFile" => [
            'class' => 'Wcc\Cache\SFile',
            'cache_dir' => "$cfg->cache_dir/scache",
            'dirtree' => false,
            'expire' => 43200,
        ],
        "Sess" => [
            'class' => 'Wcc\Cache\SFile',
            'cache_dir' => "$cfg->cache_dir/session",
            'dirtree' => false,
            'expire' => 1800,
        ],
        "Noop" => [
            'class' => "Wcc\Cache\Noop"
        ],
    ],
];