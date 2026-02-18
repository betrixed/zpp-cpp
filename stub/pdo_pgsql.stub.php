<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd\Ext;

class PdoPgsql extends \Wcd\IDriver {
	const string QUOTE_CHARACTER = "\"";
    const int DEFAULT_PORT = 5432;
	/** functions which are overrridden by C++ implementation. */

	
	/**
	public function getTableNames(): array {}
	public function getSqlType(): string {}
	public function lastSeqValue(string $name): ?int {}
	*/
};
