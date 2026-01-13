<?php
/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80400
 * @undocumentable
 */
namespace Wcc;

class Str {
	static public function camel(string $s, string $seperate = "_"): string {}
	static public function uncamel(string $s, string $seperate = "_"): string {}
	static public function grapheme(string $s, int $offset, int& $code) : string {}
	static public function test_wcc() : void {}
};

function debug_zpp_dump(mixed $val) : void {}

//function route_extract_params(string $s): array {}
