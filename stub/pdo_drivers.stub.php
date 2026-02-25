<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd\Ext;

class PdoDriver extends \Wcc\IDriver 
{
    
    public function getAttribute(int $attkey) : mixed {}

    public function getCaseAttribute(): int {}

    public function getConnectOptions(): array {}

    static public function pdoType(mixed $val): int {}

    public function query_lcase(string $sql, int $fmode) : array {}

    public function setAttribute(int $attkey, mixed $value)  : bool {}

    /* 
    
    public function param(int $pno): string {}

    public function getDSN(): string {}

    public function bind(mixed $stmt, array $params) : void {}

    public function closeStmt(mixed $stmt) : void {}
    
    public function inTransaction(): bool {}

    public function lastInsertId(): ?int {}

    public function begin(): bool {}


    public function commit(): bool {}

    public function connect(): void {}

    public function escape(string $value) : string {}
    public function getTableColumns(string $tableName) : array {}

    public function getColumnNames(string $tableName) : array {}

    public function execute(mixed $stmt, 
        bool $close = true, bool $fetch = false)  : mixed {}

    public function fetchAllRows(mixed $sth, int $mode): array {}

    public function fetchRow(mixed $stmt, int $mode) : mixed {}


public function prepare(string $query) : mixed {}
        public function prepareExecute(string $sql, 
    	?array $values = null, ?array $bindTypes = null) : void {} */

    /* public function prepareQuery(string $sql, 
    	array $values = [], array $bindTypes = []): mixed {} */

	/* public function query(string $query, 
		array $params=[]) : mixed {} 


      public function querySingle(string $sql) : mixed {}

      public function quoteName(string $name): string {}

    public function rollback(): bool {}

    public function transaction() :bool {}

   */
    


	

};

class PdoMysql extends PdoDriver {

    //public function getSqlType() : string {}
};
    
class PdoPgsql extends PdoDriver {
	const string QUOTE_CHARACTER = "\"";
    const int DEFAULT_PORT = 5432;

    //public function getSqlType() : string {}

};
