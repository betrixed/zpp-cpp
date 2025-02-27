<?php
/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80400
 * @undocumentable
 */
namespace Wcc;
//use ReflectionClass;


class Finder {
	public function addFolder(string $fpath) : void {}
	public function addPath(string $nsroot, string $fspath) : void {}

	public function addPathArray(array $paths) : void {}
	public function addClass(string $cname, string $fspath) : void {}

	// properties
	public function getNSPaths() : array {}

	public function getClassPaths()  : array {}

	public function getFolders() : array {}

	// return list - success(bool), and path(string) if true
	public function find(string $cname) : false|string {}

};


class Target {
	public function __construct(
		string $class, string $func);

	public function getClass() : string {}

	public function getFunc() : string {}

	public function getModule() : ?string {}

	public function setFunc(string $name) : void {}

	public function module(?string $name) : Target {}

	public function __serialize() : array {}

	public function __unserialize(array $data) : void {}

	public static function go(string $class, string $func = "index") : Target {}
};

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
	
	public function getCompiled() : string {}

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

	public function routeUrl(array $params) : ?string {}
	

};

final class Pair {
	public function __construct(mixed $first, mixed $second);

	public function first() : mixed {}

	public function second() : mixed {}
};

final class RouteMatch {
	public function __construct(string $uri, int $verb_flag, mixed $ajax_flag);

	public function getVerbName() : string;

	public function getErrors() : array;

	public function getUri() : string;

	public function getVerb() : int;

	public function getAjax() : int;

	public function getRoles() : array;

	public function getModuleName() : string;

	public function getObjClass() : string;

	public function getObjMethod() : string;

	public function getObjArgs() : array;

	public function getMatch() : ?Route {}

	public function prepare_call() : bool;

	public function setCallInfo(string $obclass, string $obmethod, array $args) : void;

	public function setModuleName(string $name) : void {}

	public function findRoute(RouteSet $rset) : bool {}

	public function __invoke(?array $extra = null,  Pair|null $before = null, Pair|null $after = null) : mixed {}

	public function call_method(object $obj, string $method, ?array $args = null) : mixed {}

};

final class RouteSet {

	public function addRouteList(array $list) : void {}

	public function addRoute(Route $obj) : void {}

	public function getFixed(): array {}

    public function getVary(): array {}

    public function setFile(string $fname) : void {}
  
    public function getFile() : string|null {}

    public function __serialize() : array {}

	public function __unserialize(array $data) : void {}

	public function getRoute(string $name) : ?Route {}

	public function routeUrl(string $name, array $params) : ?string {}

};

final class ReflectCache {

	public static function instance() : ReflectCache {}

	public static function getrc(string $cname) : \ReflectionClass {}

	public static function staticInstanceArgs(string $cname, array $args) : object {}

	public static function staticInstance(string $cname) : object {}

	public function newInstance(string $cname) : object {}

	public function newInstanceArgs(string $cname, array $args) : object {}

	public function getReflect(string $cname) : \ReflectionClass {}

	public function clear() : void {}

};


class RouteAdd {
	public function __construct(?RouteSet $rset = null);

	public function addRoutes(array $list, 
		?string $prefix = null, 
		?string $module = null) : void {}

	public function fallback(mixed $target) : void {}

	public function getRouteSet() : RouteSet {}

	public function methodSfx(?string $sfx = null) : void {}

	public function module(?string $name = null) : void {}

	public function prefix(?string $name = null) : void {}

	public function ready(Route $route) : void {}

};



final class Services {
	public static function getOne(string $key) : ?object {}

	public static function instance() : Services {}

	public static function service(string $name) : mixed {}

	public static function setOne(string $key, object $obj) : object {}

	public function unset(string $name) : void {}

	public function get(string $name) : mixed {}

	public function getObject(string $key) : ?object {}

	public function has(string $name) : bool {}

	public function isActive(string $name) : bool {}

	public function newInstance(string $name) : ?object {}

	public function set(string $name, mixed $value) : void {}

	public function setObject(object $obj, string|null $key = null) : object {}

	public function setDefer(string $name, mixed $value) : void {}

	public function setThrowFail(bool $value) : void {}

	public function clearActive() : void {}

	public function clearDefer() : void {}

	public function clearObjects() : void {}


};

function init_globals() : void {}

//function route_extract_params(string $s): array {}

function str_camel(string $s, string $seperate = "_"): string {}

function str_uncamel(string $s, string $seperate = "_"): string {}

function test_wcc(mixed& $test) : mixed {} 
