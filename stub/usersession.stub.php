<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

class UserSession {

	public function __construct();

	public function activate() : UserSession {}

	public function addFlash(string $text, string $status): void {}

	public function addUserRoles(array $roles) : void {}

	public function adjustExpiry() : int {}

	public function auth(string|array $role) : bool {}

	public function clearFlash() : void {}

	public function delayWrite() : void {}

	public function flash(string $text, array $extra, string $status): void {}

	public function getEndTime() : string {}

	public function getFlash() : ?array {}

	public function getKey(string $key, mixed $adef) : mixed {}

	public function getUser() : ?Config {}

	public function getUserRoles() : array {}

	public function guestSession() : UserSession {}

	public function hasKey(string $key) : bool {}

	public function hasValues() : bool {}

	public function isEmpty() : bool {}
	public function isEnded() : bool {}
	public function isLoggedIn(string $role) : bool {}
	public function nullify() : void {}
	public function read() : UserSession {}
	public function roles() : string {}
	public function save() : void {}
	public function saveUser(mixed $user, array $roles) : void {}
	public function sessionName() : ?string {}
	public function setAdmin() : void {}

	public function setGuest() : void {}
	public function setKey(string $key, mixed $value): void {}
	
	public function setValidUser(mixed $user, array $roles) : void {}
	public function shutdown() : void {}
	public function unsetKey(string $key) : void {}
	public function updated() : void {}
	public function wipe() : void {}
	public function write(bool $force = false) : void {}


};