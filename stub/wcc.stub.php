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

	public function addClasses(array $list) : void {}

	// properties
	public function getNSPaths() : array {}

	public function getClassPaths() : array {}

	public function getFolders() : array {}

	public function find(string $cname) : null|string {}

};


class Target {
	public function __construct(
		string $class, string $func = "index");

	public function getClass() : string {}

	public function getFunc() : string {}

	public function getModule() : ?string {}

	public function setFunc(string $name) : void {}

	public function setModule(?string $name) : Target {}

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

final class Pair {

	public mixed $one;
	public mixed $two;

	public function __construct(mixed $first, mixed $second);

	public function first() : mixed {}

	public function second() : mixed {}

	public function key() : mixed {}

	public function value() : mixed {}

	public function sum() : mixed {}
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

	public function call(?array $extra = null,  Pair|null $before = null, Pair|null $after = null) : mixed {}

	static public function callMethod(object $obj, string $method, ?array $args = null) : mixed {}

};

final class RouteSet {
	public function addRoute(Route $obj) : void {}

	public function addRoutes(array $list,?string $prefix = null, 
		?string $module = null) : void {}

	public function compile(Route $obj) : void {}

	public function getFile() : string|null {}

	public function getFixed(): array {}

	public function getRoute(string $name) : ?Route {}

    public function getVary(): array {}

    public function methodSfx(?string $sfx) : void {}

    public function module(?string $name = null, bool $prefix = true) : void {}

    public function notFound(mixed $target) : void {}

    public function prefix(?string $prefix) : void {}

    static public function rex_url() : string {}

    public function routeUrl(string $name, ?array $params = null) : ?string {}

    public function setFile(string $fname) : void {}
  
    public function __serialize() : array {}

	public function __unserialize(array $data) : void {}

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

class ServiceAccess {

	public readonly Services $gservices;
	public readonly Services $services;

    public function __construct(?Services $svc = null) {}

    public function __destruct();
    
    public function __get(string $name) : mixed {}

    public function __set(string $name, mixed $value) : void {}

    public function __isset(string $name) : bool {}

    public function __unset(string $name) : void {}

    public function unset(string $name) : void {}
    
    public function service(string $name) : mixed {}

    public function init_access() : void {}

    public function hasService(string $name) : bool {}

    public function setExtender(object $obj) : void {}

};

class Str {
	static public function camel(string $s, string $seperate = "_"): string {}
	static public function uncamel(string $s, string $seperate = "_"): string {}
	static public function test_wcc() : void {}
	static public function intern(string $s) : string {}
};

function debug_zpp_dump(mixed $val) : void {}

function init_globals() : void {}

//function route_extract_params(string $s): array {}
