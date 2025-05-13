<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

// stdClass # implements ArrayAccess 

class Hmap //implements IteratorAggregate
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

	public function assign(array $data) : void {}

	public function getIterator():  Iterator {}

	public function count() : int {}

	public function offsetExists(mixed $offset) : bool {}

	public function offsetGet(mixed $offset) : mixed {}

	public function offsetSet(mixed $offset, mixed $value) : void {}

	public function offsetUnset(mixed $offset) : void {}

	public function __serialize() : array {}

	public function __unserialize(array $data) : void {}

	//public function __wakeup(): void {}


};


