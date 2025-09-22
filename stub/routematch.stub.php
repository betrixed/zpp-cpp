<?php
/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80400
 * @undocumentable
 */
namespace Wcc;

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