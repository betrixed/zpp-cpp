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
	public string $userName = "";
    public array $roles = []; //list of string "allowed roles";
    public string $email = "";
    public int $id = 0; // database record user id
    public int $memberid = 0; // alternate record id
    public string $status = ""; // status of account
    public array $keys = [];   // random user data

	public function __construct();

	public function addFlash(string $text, string $status = 'info') : Flash {}

	public function auth(array $roles) : bool {}

	public function hasAnyRole(array $roles): bool {}

	public function hasRole(string $role) : bool {}

	public function hasUser() : bool {}

	public function init() : void {}

	public function getKey(string $key, mixed $adef=null) : mixed;
	
	public function isGuest() : bool {}

	public function setGuest() : void {}

	public function setKey(string $key, mixed $val): void {}

	public function setValidUser(string $name, array $roles) : void {}

	public function unsetKey(string $key) : bool {}

	public function wipekeys() : void {}

};