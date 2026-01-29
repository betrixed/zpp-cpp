<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd;

class PdoPgsql extends IDriver {
	public function getTableNames(): array {}
	public function getSqlType(): string {}
	public function lastSeqValue(string $name): ?int {}
};
