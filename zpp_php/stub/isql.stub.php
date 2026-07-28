<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd\Sql;

interface IfSql {
    const int SQL_OBJ = 0;
    const int SQL_INSERT = 1;
    const int SQL_UPDATE = 2;
    const int SQL_DELETE = 3;
    const int SQL_SELECT = 4;
    const int SQL_WHERE = 5;
    const int SQL_JOIN = 6;
    const int SQL_RETURN = 7;
    const int SQL_FROM = 8;
    const int SQL_ORDER = 9;
    const int SQL_LIMIT = 10;
    const int SQL_AGGREGATE = 11;
    const int SQL_DISTINCT = 12;
    const int SQL_RENAME = 13;
    const int FETCH_AS = 14;
    const int MODEL_OBJ = 15;
    const int MODEL_CLASS = 16;
    const int NAME_LIST = 17;

    public function quoteName(string $name): string {}

};


class ISql implements IfSql
{
    public static function tableClass(string $uname) : string {}
    
    public function deleteSql(Bindings $bind) : ParamList {}

    public function emit(
        IfSqlPart $sp, 
        Bindings $bind, 
        ?string $lalias = null, 
        ?string $ralias = null): string {}

    public function entityClass(string $table) : string {}

    public function getTruncateSql(): string {}

    public function insert(Bindings $bind) : ParamList {}

    public function quoteAlways(string $name): string {}

    public function quoteName(string $name): string {}

    public function select(Bindings $bind) : ParamList {}

    public function seqLastValue(string $seq_name): string {}

    public function setSeqValue(int $value, array $data): string {}

    public function truncate(Bindings $bindings) : string {}

    public function update(Bindings $bind) : ParamList {}

    public function valuesDefault(): string {}
};
