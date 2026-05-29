<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc\Session;


class UserData {

    public ?\Wcc\Config $user = null;
    public array $flash = [];
    public array $keys = [];    
    public array $roles = [];  

	public function __construct();

	public function hasAnyRole(array $roles): bool {}

	public function hasRole(string $role) : bool {}

	public function hasUser() : bool {}
};