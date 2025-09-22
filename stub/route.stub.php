<?php
/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80400
 * @undocumentable
 */
namespace Wcc;

final class  Route {
	public static function GetVerb(int $vbit) : string {}
	public static function GetVerbInt(string $verb) : int {}
	public static function GetVerbNames(int $vbits) : array {}

	public static function get(string $pattern, 
            mixed $target,
            int $ajaxFlags = 1): Route {}

	public static function post(string $pattern, 
            mixed $target,
            int $ajaxFlags = 1): Route {}

	public static function methods(int $methods, string $pattern, 
            mixed $target,
            int $ajaxFlags = 1): Route {}
	
	public function __construct(int $verbs, string $pattern, mixed $target);

	public function setCompiled(string $s) : void {}
	
	public function getCompiled() : ?string {}

	public function getVerbs() : int {}

	public function hasVerb(int $vbit) : bool {}

	public function getParams() : array {}

	public function setParams(array $params) : void {}

	public function getTarget() : mixed {}

	public function setTarget(mixed $target) : void {}

	public function hasParams() : bool {}

	public function setPattern(string $pattern) : void {}

	public function getPattern() : string {}

	public function setAjax(int $flag) : void {}

	public function allow(int $verb, int $ajax) : bool {}
	
	public function getAjax() : int {}

	public function name(string $id) : Route {}

	public function getName() : ?string {}

	public function __serialize() : array {}

	public function __unserialize(array $data) : void {}

	public function routeUrl(?array $params = null) : ?string {}
};