<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

#[\AllowDynamicProperties]
class Config # implements ArrayAccess, Countable 
{
	public function __construct(?array $values=null);

	public function getOrNot(string $key, mixed $ifnot) : mixed {}

	public function has(string $key) : bool {}

	public function get(string $key) : mixed {}

	public function set(string $key, mixed $value) : void {}

	public function unset(string $key): void {}
	
	public function subset(string|array $data) : array {}

	public function addArray(array $data): void {}

	public function toArray() : array {}

	public function unhive(string $key) : string {}

	public function clear() : void {}

	public function count() : int {}

	public function offsetExists(mixed $offset) : bool {}

	public function offsetGet(mixed $offset) : mixed {}

	public function offsetSet(mixed $offset, mixed $value) : void {}

	public function offsetUnset(mixed $offset) : void {}


	
/*
	public function __get(string $name) : mixed {}

	public function __set(string $name, mixed $value) : void {}

	public function __isset(string $name) : bool {}

	public function __unset(string $name) : void {}

	public function offsetExists(mixed $index) : bool {}

    public function offsetGet(mixed $index) : mixed {}

    public function offsetSet(mixed $index, mixed $value) : void {}

    public function offsetUnset(mixed $index) : void {}
*/
//    public function count() : int {}


};