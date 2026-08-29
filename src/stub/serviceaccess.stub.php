<?php
/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80400
 * @undocumentable
 */
namespace Wcc;

class ServiceAccess {

	public readonly Services $gservices;
	
	public readonly Services $services;

    public function __construct(?Services $svc = null) {}

    public function __destruct();
    
    public function __get(string $name) : mixed {}

    public function __set(string $name, mixed $value) : void {}

    public function __isset(string $name) : bool {}

    public function __unset(string $name) : void {}

    public function unset(string $name) : void {}
    
    public function service(string $name) : mixed {}

    public function init_access() : void {}

    public function hasService(string $name) : bool {}

    public function setExtender(object $obj) : void {}

};