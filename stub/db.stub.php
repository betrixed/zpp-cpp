<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd;

/*
use Wcd\Sql\Bindings;
use Wcd\Sql\ISql;
use Wcd\Sql\JoinTables;
use Wcd\Sql\ParamList;
*/



class IDriver {

    public readonly string $name;

    public function __construct(IConfig $cfg, string $name);

    public function __destruct();

    public function begin(): bool {}

    public function bind(mixed $stmt, array $params) : mixed {}

    public function close() : void {}

    public function closeStmt(mixed $stmt) : void {}

    public function commit(): bool {}

    public function connect(): void {}

    public function escape(string $value) : string {}

    public function execute(mixed $stmt, 
            bool $close = true, bool $results = false) : mixed {}

    public function fetchAllRows(mixed $stmt,
                int $mode ): array {}

    public function fetchRow(mixed $stmt,
                int $mode ): array {}

    public function getAttribute(int $attkey) : mixed {}

    public function getCaseAttribute(): int {}

    public function setCaseAttribute(int $value) : void {}

    public function getColumnNames(string $tableName) : array {}

    public function getConnectOptions(): array {}

    public function getDSN() : string {}

    public function getDatabaseName(): string {}

    public function getFetch(): int {}
    
    public function getSqlType() : string {}
    
    public function getSchema(): IStore {}
    
    public function getSchemaClass(): string {}

    public function getTableColumns(string $tableName) : array {}

    public function getTableModel(string $tableName) : Model {}

    public function getTableNames(): array {}

    public function handle() : mixed {}

    public function iSql(): Sql\ISql {}

    public function iConfig() : IConfig {}

    public function inTransaction(): bool {}

    public function isAutoCommit(): bool {}

    public function isConnected(): bool {}

    public function lastInsertId(): ?int {}

    public function lastSeqValue(string $name): ?int {}

    public function lastSQL() : ?string {}

    public function log(array $info) : void {}

    public function modelClassName(string $tableName) : string {}

    public function newBindings() : \Wcd\Sql\Bindings;

    public function newDmlBuild() : IBuild {}

    public function newParamList() : \Wcd\Sql\ParamList;
    
    public function param(int $pno): string {}

    public function prepare(string $sql) : mixed {}

    public function prepareQuery(string $sql, 
        array $values = [], array $bindTypes = []): mixed {}

    public function query(string $query, array $params=[]) : mixed {}

    public function querySingle(string $sql) : mixed {}

    public function quoteName(string $name): string {}

    public function readSchema(): IStore {}

    public function rollback(): bool {}

    public function getWeakRef() : \WeakReference {}

    public function setAttribute(int $attkey, mixed $value) : void {}

    public function setFetch(int $value): int {}

    public function transaction() : bool {}

};

class IBuild {
    public function __construct(IDriver $db);

    public function __destruct();

    public function aggregate(string $func, ?array $columns = null) : mixed {}

    public function allRows() : mixed {}

    public function count(string|array $columns = "*") : int {}

    public function deleteRow(IRow $row) : bool {}

    public function distinct(bool $set = true): void {}

    public function first(?array $columns = null) : mixed {}

    public function get(?array $columns = null) : mixed {}

    public function getDriver() : IDriver {}

    public function getBindings() : Sql\Bindings {}

    public function getFrom() :  Sql\JoinTables {}

    public function getInsertSql(array $columns) :  Wcd\Sql\ParamList {}

    public function getSql() :  Sql\ISql {}

    public function hasModel() : bool {}

    public function insert(array| IRow $rows) : mixed {}

    public function limit(int $limit, int $offset = 0): void {}

    public function now(): string {}

    public function offset(int $offset): void {}

    public function oneRow() : mixed {}

    public function orderBy(mixed $column, bool $descend = false) : void {}

    public function seqLastValue(string $seqname): ?int {}

    public function set(string $column, mixed $value): void {}

    public function setInsert(array $data) : void  {}

    public function setModel(?Model $model = null, bool $bind = true): void {}

    public function setModelClass(string $cname) : void {}

    public function setReturns(array $names) : void {}

    public function setSeqValue(int $value, array $data): ?int {}

    public function table(string $table, bool $wipe = true) : void {}

    public function update(IRow $row, array $dirty = []) : mixed {}

    public function where(mixed $column, ?string $operator = null, 
                        mixed $value = null, string $bval = "AND") : void {}

    public function wipe(): void {}
};


class Simple {
    

    public function __construct(\Wcd\IDriver $db, ?int $fetch = null);

    public function __destruct();

    public function arrayMap(string $keycol, string $valcol, string $table) : ?array {}

    public function arraySet(string $sql, ?array $params = null ) 
        : ?array {}

    public function bind(mixed $value) : string {}

    public function exec(string $sql, 
        ?array $params = null) : int {}

    public function firstRow(string $sql, ?array $params = null )
    : ?array {}

    public function getRows() : mixed {}

    public function getSchemaName() : string {}

    public function insert(array $values) : mixed {}

    public function prepare(string $sql) : bool {}

    public function quoteName(string $name) : string {}

    public function returnsValues(bool $retval) : void {}

    public function run() : mixed {}

    public function setValues(array $params) : void {}

    public function update(array $values) : mixed {}
};


class IRow extends \Wcc\Hmap {
    public function __construct(IfCrud $m, ?array $data=null, bool $exists=false);

    public function setData(array $data, bool $exists = false) : void {}

    public function getData() : ?array {}

    public function getDataValues(array $names) : array {}

    public function getDirty() : array {}

    public function getModel() : IfCrud {}

    public function hasValue(string $key) : bool {}

    public function isDirty(?string $colname = null) : bool {}

    public function mergeData(array $data) : IRow {}

    public function create(bool $reload = false) : bool {}

    public function delete() : void {}

    public function exists() : bool {}

    public function read() : IRow {}

    public function save(bool $reload = false) : bool {}

    public function update(bool $reload = false) : bool {}

    public function setExists() : void {}

    public function stampTime(string $now, int $dtflags = IfCrud::ALL_TS) : array {}

}

