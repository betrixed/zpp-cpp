<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd;

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


/*
interface IfSqlParam {
    public function __construct(\Wcd\IDriver $gen);

    public function getParams() : array {}

    public function getReturns() : ?array {}

    public function getSql() : ?string {}

    public function getValues() : ?array {}

    public function setParams(array $replace) : void {}

    public function setReturns(array $replace) : void {}

    public function setSql(string $sql) : void {}

    public function setValues(array $replace) : void {}

    public function addParam(mixed $value) : void {}

    public function addParamList(array $values): string {}

    public function makeList(int $start, int $count): string {}

    public function paramLiteral(mixed $value) : string {}

    public function useOwnValues() : void {}

    public function wipe() : void {}
};
*/



class IDriver {

    public function __construct(IConfig $cfg, string $name);

    public function __destruct();

    protected function afterConnect() : void {}

    protected function serverNameFormat() : string {}


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

class Model implements IfCrud {


    public static function KeyValue(array $key, array $value): ?IRow {}

    public static function WithValues(array $values) : ?IRow {}

    public static function __callStatic(string $method, array $parameters) : mixed {}

    public static function classToTableName(string $cname): string {}

    public static function createFromResult(string $mclass, mixed $results, 
        array $eagerLoad = []) : array {}

    public static function find(mixed $id): ?IRow {}

    public static function getTableName() : string {}

    public static function importFromCSV(string $fileName): int {}

    public static function modelBuild(): IBuild {}

    public static function row(array $data = []) : IRow {}

    public static function rowSaved(array $data = []) : IRow {}

    public function create(IRow $row, bool $reload = false) : bool {}

    public function createdAtName() : string {}

    public function delete(IRow $row) : bool {}

    public function exists(IRow $row): bool {}

    public function getColDefs() : ?array {}

    /** Important Interface properties */
    public function getConnect(): IfDriver {}

    public function getFieldDef(string $ckey) : ?array {}

    public function getForeignKey(): array {}

    public function getKeyOptions(): array {}

    public function getName(): string {}

    public function getPKey(): array {}

    public function getSeqDefs(): array {}

    public function getTSFlags() :  int {}

    public function getTableDef() : mixed {}

    public function hasTimeStamps() : bool {}

    public function newRow(array $rdata = [], bool $isSaved = false) : IRow {}

    public function readRow(IRow $row) : IRow {}

    public function save(IRow $row, bool $reload = false) : bool {}

    public function setColDefs(array $cdefs) : void {}


    public function setKeyOptions(array $options) : void {}

    

    public function setName(string $name) : void {}

    public function setPKey(array $pnames) : void {}

    public function  setSeqDefs(array $sdefs) : void {}

     public function setTSFlags(int $tsval) : void {}

    public function stampTime(string $stamp, 
            int $tsflags = IfCrud::ALL_TS) : array {}

    public function update(IRow $row, bool $reload = false) : bool {}

    public function updatedAtName() : string {}

};

class RunSql {
    public function __construct(IDriver $db, string $sql,
            ?array $bind=null, bool $retval = false);

    static public function Op(IDriver $db, string $sql,
            ?array $bind=null, bool $retval = false ) : mixed {}

    public function operation() : mixed {}
    
    public function run() : mixed {}

};



