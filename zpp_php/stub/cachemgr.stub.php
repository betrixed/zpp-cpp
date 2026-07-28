<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

class CacheMgr {

	public function __construct(array $mgr_cfg);

	public function __destruct();

	public static function __callStatic(string $name, array $args) : mixed {}

	public static function readFile(string $filename, ?string $ext = null) : mixed {}

	public function clearAll() : void {}

	public function createCache(string $service_key, string $class_name, array $options) : void {}

	public function deleteExpired() : void {}

	public function getCache(string $service_key) : ?ICache {}

	public function getCacheClass(string $service_key) : string {}
    
    public function getCacheKeys() : array {}

	public function readCache(string $filename, string $cache_name) : mixed {}

	public function flush_caches() : void {}

};