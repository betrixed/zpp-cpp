<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

class Assets {
	public function __construct();

	public function __destruct();
	
	public function add(array|string $list) : void {}

 	public function addAssets(array $add) : array {}

 	public function addBlob(string $blob, bool $header = false) : void {}

 	public  function addSourcePath(string $path) : array {}

 	public function addStyle(string $style) : void {}
 	
 	public function cssHeader() : string {}
 	
 	public function footer() : string {}

 	public function getSearchList() : SearchList {}

 	public function getWebList(
 		string $selector, 
 		null|string|array $names = null,
 		bool $list = true) : array {}

 	public function has(string $key) : bool {}

 	public function header() : string {}

 	public function inline_css(string $name): string {}

 	public function link() : string {}

 	public function loadAssetFile(string $file) : array {}

 	public function jsPull(string $name) : string {}
 	
 	public function setRun(object $env) : void {}
 	
 	public function reset() : void {}

 	public function styleHeader() : string {}

 	public function unmark(string $item) : void {}
};


