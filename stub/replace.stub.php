<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

class Replace {
	public function __construct(object $obj, ?string $rexpr = null);
	
	public function eval(string $subj): string {}

	static public function property(object $obj, string $data) : string {}
};