<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;


class Run {


    public readonly Config $config;

    public readonly string $site_dir;
    public readonly string $gallery;
    
    public readonly string $site_leaf;
    public readonly string $wc_leaf;
    public readonly string $vendor_leaf;
    public readonly string $phproot;

    public readonly string $target;
    public readonly float  $start_time;
    public readonly string $config_dir;
    public readonly string $temp_dir;
    public readonly string $init_cwd;

    public readonly bool   $is_web;
    public readonly string $sapi;

    public array  $modules = [];
    public string $timezone;
    public string $web_dir;
    public string $theme;
    
    public ?CacheMgr $cache_mgr;

    public string $module_name;
    public ?Module $module_cfg;
    public string $routes;

    public ?object $site;

    public bool   $page_hits;
    public ?PhpStats $stats;

    public function __construct() {}

    public function __destruct() {}

    public function execute(string $bootstrap): void {}

    public function shutdown() : void {}

};
