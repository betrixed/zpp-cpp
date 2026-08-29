<?php
/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80400
 * @undocumentable
 */
namespace Wcc;

final class ReflectCache {

	public static function instance() : ReflectCache {}

	public static function getrc(string $cname) : \ReflectionClass {}

	public static function 
		staticInstanceArgs(string $cname, array $args) : object {}

	public static function 
		staticInstance(string $cname) : object {}

	public function newInstance(string $cname) : object {}

	public function newInstanceArgs(string $cname, array $args) : object {}

	public function getReflect(string $cname) : \ReflectionClass {}

	public function clear() : void {}

};