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

	public static function dirList_dir(string $s) : array {}

	public static function dirList_fileExt(string $dir, ?array $extlist = null) {}
	

};


