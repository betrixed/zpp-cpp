<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

class XmlRead {

	// because some PHP code wants to parent::construct?
	public function __construct(?object $root = null);

	static public function fromFile(string $path) : mixed {}
	public function parseFile(string $path) : mixed {}

	static public function fromString(string $src) : mixed {}
	public function parse(string $src) : mixed {}
};

