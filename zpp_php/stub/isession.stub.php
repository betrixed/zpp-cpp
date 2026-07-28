<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc\Session;

class ISession {
	public function __construct(array $options = []);

	public function __get(string $name) : mixed {}

	public function __isset(string $name) : bool {}

	public function __set(string $name, mixed $value) : void {}

	public function __unset(string $name) : void {}


	public function destroy(): void {}

	public function getAdapter(): \SessionHandlerInterface {}

	public function getEndTime(): string {}

	static public function id(?string $id = null) : string {}
	static public function name(?string $name = null): string {}
	static public function status() : int {}
	static public function exists(): bool {}

	public function getOptions() : array {}

	public function has(string $key) : bool {}

	public function regenerateId(bool $deleteOldSession = true): bool {}

	public function remove(string $key): void {}

	public function set(string $key, mixed $value): void {}

	public function setAdapter(\SessionHandlerInterface $adapter): void {}

	public function setOptions(array $options): void {}

	public function start() : bool {}


};

