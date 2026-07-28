<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

class ICache {
	
	public function __construct(array $options = []);

	public function addLocal(ICacheData $pkg): void {}

	public function clear(): bool {}

	public function clearPrefix(string $prefix): bool {}

	public function delete(string $key): bool {}

	public function deleteExpired(): int {}

	public function deleteMultiple(array $keys): bool {}

	public function get(string $key, mixed $default = null): mixed {}

	public function getData(string $key): mixed {}

	public function getMultiple(array $keys, mixed $default = null): array {}

	public function getOption(string $key) : mixed {}
	
	public function getPrefix(): string {}

	public function getService(string $name): mixed {}

	public function getTTL(): int {}

	public function getUnsaved(): array {}

	public function getCached(string $key): ICacheData|null {}

	public function set(string $key, mixed $value, int $ttl = 0): bool {}

	public function setCached(string $key, mixed $value, int $ttl = 0): ICacheData|null {}

	public function setMultiple(array $values, int $ttl = 0): bool {}

	public function setOption(string $key, mixed $value) : void {}

	public function setTTL(int $val): void {}

	public function flushCached() : void {}

};
