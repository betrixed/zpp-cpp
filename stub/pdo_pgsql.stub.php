<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd\Ext;

class PdoDriver extends \Wcc\IDriver 
{
    public function begin(): bool {}

    public function bind(mixed $stmt, array $params) : void {}

    public function closeStmt(mixed $stmt) : void {}

    public function commit(): bool {}

    public function connect(): void {}

    public function escape(string $value) : string {}

    public function execute(mixed $stmt, 
    	$close = true, $fetch = false)  : mixed {}

    public function fetchAllRows($sth, int $mode): array {}

    public function fetchRow($stmt, int $mode) : mixed {}

    public function getAttribute(int $attkey) : mixed {}

    public function getCaseAttribute(): int {}

    public function getColumnNames(string $tableName) : array {}

    public function getConnectOptions(): array {}

    public function getDSN(): string {}

    public function getTableColumns(string $tableName) : array {}

    public function getTableNames(): array {}

    public function inTransaction(): bool {}

    public function lastInsertId(): ?int {}

    public function lastSeqValue(string $name): ?int {}

	public function param(int $pno): string {
        return '?';
    }

    static function pdoType(mixed $val): int {}

    public function prepare(string $query) : mixed {}

    public function prepareExecute(string $sql, 
    	?array $values = null, ?array $bindTypes = null) : void {}

    public function prepareQuery(string $sql, 
    	array $values = [], array $bindTypes = []): mixed {}

	public function query(string $query, 
		array $params=[]) : mixed {}
    
    public function querySingle(string $sql) : mixed {}

    public function quoteName(string $name): string {}

    public function rollback(): bool {}

    public function setAttribute(int $attkey, mixed $value)  : bool {}

	public function transaction() :bool {}

};


    
class PdoPgsql extends PdoDriver {
	const string QUOTE_CHARACTER = "\"";
    const int DEFAULT_PORT = 5432;

};
