<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc\Session;

class Flash {
	public array $lines;

	public function __construct(array $data = []);

	public function hasData() : bool {}

	public function add(string $text, string $status) : void {}

	public function getData() : array {}

	public function clear() : void {}
};

class UserData {

    public ?\Wcc\Config $user = null;
    public ?Flash = null;
    public array $keys = [];    
    public array $roles = [];  

	public function __construct();

	public function hasAnyRole(array $roles): bool {}

	public function hasRole(string $role) : bool {}

	public function hasUser() : bool {}

};