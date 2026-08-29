<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

class ICacheData {
	public function __construct(string $key, mixed $data, int $ttl = 0);

	public function __serialize() : array {}

	public function __unserialize(array $data) : void {}

	public function getData() : mixed {}

	public function getExpiry() : int {}

	public function getKey() : string {}

	public function getStored() : int {}

	public function getTTL() : int {}

	public function isSaved() : bool {}

	public function setStored() : void {}

	public function update(mixed $data, int $ttl) : void {}

};