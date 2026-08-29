<?php
/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80400
 * @undocumentable
 */
namespace Wcc;

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
