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
    
    public ?CacheMgr $cache_mgr = null;

    public string $module_name;
    public ?Module $module_cfg = null;
    public string $routes;

    public ?object $site = null;


    public ?PhpStats $stats = null;

    public bool  $page_hits = false;

    public array $bootstrap = [];
    

    public function __construct() {}

    public function __destruct() {}

    public function execute(string $bootstrap): void {}

    public function shutdown() : void {}

};
