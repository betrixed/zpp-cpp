<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

class Assets {
	public function __construct();

	public function add(array|string $list) : void {}

 	public function addAssets(array $add) : void {}

 	public function addBlob(string $blob, bool $header = false) : void {}

 	public  function addSourcePath(string $path) : array {}
 	
 	public function getSearchList() : SearchList {}

 	public function addStyle(string $style) : void {}

 	public function clearCache() : void {}

 	public function cssHeader() : string {}

 	public function cssMinify() : void {}

 	public function has(string $key) : bool {}

 	public function inline_css(string $name): string {}

 	public function jsMinify() : void {}

 	public function loadAssetFile(string $file) : void {}

 	public function reset() {} void {}

 	public function setMinify(string $name) : void {}

 	public function styleHeader() : string {}

 	public function footer() : string {}

 	public function header() : string {}


};