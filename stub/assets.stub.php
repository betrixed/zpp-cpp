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

 	public function addStyle(string $style) : void {}
 	
 	public function footer() : string {}

 	public function getSearchList() : SearchList {}

 	public function getWebList(string $selector, bool $list = true, ?array $names = null) : array {}

 	public function has(string $key) : bool {}

 	public function header() : string {}

 	public function inline_css(string $name): string {}

 	public function link() : string {}

 	public function loadAssetFile(string $file) : void {}

 	public function reset() {} void {}

 	public function styleHeader() : string {}

};