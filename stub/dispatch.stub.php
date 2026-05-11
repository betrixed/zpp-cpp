<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

class Dispatch {
	public function __construct();

	public function action(array $to) : void {}

	public function addModule(string $name, array|string $mod): Module {}

	public function clearRouteCache(string $file, string $cache_name) : bool {}

	public function dispatch(RouteMatch $rm): void {}

	public function dispatchError(
		    RouteMatch $rm, 
            string $errorClass, string $errorMethod, ?array $errArgs = null) : void {}

	public function forward( array|string $to) : void {}

	public function getActiveModule(): ?Module {}

	public function getArgs() : ?array {}

	public function getDefaultModule(): Module {}

	public function getMethodName(): ?string {}

	public function getModule(string $name): ?Module {}

	public function getRoles() : array {} 

	public function getRoute() : ?Route {}

	public function getRoutesCache(string $cache_name) : ?object {}

	public function getRouteMatch() : ?RouteMatch {}

	public function getUri() : ?string {}

	public function loadRoutes(string $file, string $cache_name) : ?array {}

	public function obcall(RouteMatch $rm) : mixed {}

	public function obcallEx(string $obclass, string $obmethod, 
						?array $obargs = null) : mixed {}

	public function parseRaw(array $input) : array {}

	public function respond(mixed $content): void {}

	public function setLog(bool $val): void {}

	public function setModule(string $name): ?Module {}

	public function setModuleCfg(array $modcfg) : void {}

};
