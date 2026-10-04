<?php
/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80400
 * @undocumentable
 */
namespace Wcc;

#[\AllowDynamicProperties]
class Target {
	
    public string  $objclass;
    public string  $objmethod;
    public array   $params;
	
	
	public function __construct(
		string $class, string $method = "index", array $params=[]);

	public function &refParams() : array {}

	public function getModule() : string;

	public static function go(string $class, string $method = "index", 
					array $params=[]) : Target {}
};