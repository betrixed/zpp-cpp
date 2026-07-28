<?php
/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80400
 * @undocumentable
 */
namespace Wcc;

final class Services {
	public static function getOne(string $key, ?array $arglist=null) : ?object {}

	public static function makeOne(string $key, ?array $arglist=null) : object {}
	
	public static function setOne(string $key, ?object $obj = null) : object {}

	public static function instance() : Services {}

	public static function service(string $name) : mixed {}

	public function unset(string $name) : void {}

	public function get(string $name) : mixed {}

	public function getObject(string $key) : ?object {}

	public function has(string $name) : bool {}

	public function isActive(string $name) : bool {}

	public function newInstance(string $name, ?array $arglist=null) : ?object {}

	public function set(string $name, mixed $value) : void {}

	public function setObject(object $obj, string|null $key = null) : object {}

	public function setDefer(string $name, mixed $value) : void {}

	public function setThrowFail(bool $value) : void {}

	public function clearActive() : void {}

	public function clearDefer() : void {}

	public function clearObjects() : void {}

};