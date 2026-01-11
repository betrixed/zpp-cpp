<?php
/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80400
 * @undocumentable
 */
namespace Wcc;

#[\AllowDynamicProperties]
class Target {
	public function __construct(
		string $class, string $func = "index");

	public function getClass() : string {}

	public function getFunc() : string {}

	public function getModule() : ?string {}

	public function setFunc(string $name) : void {}

	public function setModule(?string $name) : Target {}

	public function getParams() : ?array {}

	public function setParams(?array $params) : void {}

	public function getParam(string $key) : mixed {}

	public function setParam(string $key, mixed $value) : void {}

	public function __set(string $key, mixed $value): void {}

	public function __get(string $key) : mixed {}

	public function __serialize() : array {}

	public function __unserialize(array $data) : void {}

	public static function go(string $class, string $func = "index") : Target {}
};