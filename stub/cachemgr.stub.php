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

	public static function readFile(string $filename, ?string $ext) : mixed {}

	public static function readPhp(string $filename): mixed {}
	public static function readToml(string $filename): mixed {}
	public static function readXml(string $filename): mixed {}

	public function clearAll() : void {}

	public function createCache(string $svc_key, string $classname, array $options) : void {}

	public function deleteExpired() : void {}

	public function getCache(string $svc_key) : ?ICache {}

	public function getCacheClass(string $svc_key) : string {}

	public function getHits(string $svc_key) : int {}

	public function getMisses(string $svc_key) : int {}

	public function readCache(string $filename, string $cache_name) : mixed {}

	public function write_caches() : void {}

};