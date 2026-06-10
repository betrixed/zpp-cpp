<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc\Session\Adapt;

class SysCache implements SessionHandlerInterface {

	public function __construct(array $options=[]);

	public function close(): bool {}

	public function destroy(string $id): bool {}

	public function gc(int $max_lifetime): int|false {}

	public function getExpires() : ?int{}

	public function (string $path, string $name) : bool {}

	public function read(string $id): string|false {}

	public function write(string $id, string $data): bool {}

};