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

interface IfCrud
{
    /** Both update and create time stamps */
    const int ALL_TS = 3;
    /** Update timestamp flag */
    const int UPDATE_TS = 2;
    /** Create time stamp flag */
    const int CREATE_TS = 1;
    /** Default no timestamps value */
    const int NO_TS = 0;

    /** Method to return identity values after insert */
    const int ID_SET = 0; 
    const int LAST_ID = 1;
    const int LAST_SEQ = 2;
    const int ID_GEN = 4;

    /** array key for binding return identify value type */
    const string RETURNS = "_ret";

    /** sequence key options */
    const string SEQ_KEY = "seq";
    const string BIND_KEY = "bind";
    const string DEFAULT_KEY = "default";
    const string ID_KEY = "identity";
    const string AUTO_INC = "auto_inc";


    /** Auto field name for create time stamp */
    const string CREATED_AT = "created_at";
    /** Auto field name for update time stamp */
    const string UPDATED_AT = "updated_at";
    
    public function create(IRow $row, bool $reload = false) : bool {}

    public function read(IRow $row) : IRow {}

    public function update(IRow $row, bool $reload = false) : bool {}

    public function delete(IRow $row) : bool {}

    /** save performs create or update as required */
    public function save(IRow $row, bool $reload = false) : bool {}

    public function stampTime(string $stamp, int $tsflags = IfCrud::ALL_TS) : array {}

    public function getColDefs() : ?array {}
};





class IDriver {

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

    public function iSql(): ISql {}

    public function iConfig() : IConfig {}

    public function inTransaction(): bool {}

    public function isAutoCommit(): bool {}

    public function isConnected(): bool {}

    public function lastInsertId(): ?int {}

    public function lastSeqValue(string $name): ?int {}

    

    public function log(array $info) : void {}

    public function modelClassName(string $tableName) : string {}

    public function newDmlBuild() : IBuild {}

    public function param(int $pno): string {}

    public function prepare(string $sql) : mixed {}

    public function prepareQuery(string $sql, 
        array $values = [], array $bindTypes = []): mixed {}

    public function query(string $query, array $params=[]) : mixed {}

    public function querySingle(string $sql) : mixed {}

    public function quoteName(string $name): string {}

    public function readSchema(): IStore {}

    public function rollback(): bool {}


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

    public function getBindings() : Wcd\Sql\Bindings {}

    public function getFrom() :  Wcd\Sql\JoinTables {}

    public function getInsertSql(array $columns) :  Wcd\Sql\ParamList {}

    public function getParamList() :  Wcd\Sql\ParamList {}

    public function getSql() :  Wcd\Sql\ISql {}

    public function hasModel() : bool {}

    public function insert(array|IRow $rows) : mixed {}

    public function limit(int $limit, int $offset = 0): void {}

    public function now(): string {}

    public function offset(int $offset): void {}

    public function oneRow() : mixed {}

    public function orderBy(string $column, bool $descend = false) : void {}

    public function seqLastValue(string $seqname): ?int {}

    public function set(string $column, mixed $value): void {}

    public function setFetch(int $mode): int {}

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




