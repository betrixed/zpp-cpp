<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd\Sql;

interface IfSqlPart {
    const int LIT_PID = 1;
    const int EXPR_PID = 2;
    const int JE_PID = 3;
    const int TA_PID = 4;
    const int PARAM_PID = 5;
    const int ICOL_PID = 6;
    const int TCOL_PID = 7;
    
    public function getPartId(): int {}
};

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

class Expr implements IfSqlPart {
    public function __construct(string $e);

    public function getPartId() : int {}

    public function __toString(): string {}
};

class Literal implements IfSqlPart {
    public mixed $value;

	public function __construct(mixed $val) {}

    public function getPartId() : int {}

    public function getValue() : mixed {}

    public function __toString() : string {}
};

class JoinExpr implements IfSqlPart {
    CONST int OP_EQ = 0;
    CONST int OP_NEQ = 1;
    CONST int OP_GT = 2;
    CONST int OP_LT = 3;
    CONST int OP_GE = 4;
    CONST int OP_LE = 5;
    CONST int OP_LIKE = 6;
    CONST int OP_AND = 7;
    CONST int OP_OR = 8;
    CONST int OP_ISNULL = 9;
    CONST int OP_NOTNULL = 10;
    CONST int OP_NOP = 11;
    
    CONST int B_NULL = 0;
    CONST int B_AND = 1;
    CONST int B_OR = 2;

    public function __construct(
        mixed $left_attr, 
        mixed $right_attr = null,
        int $op = JoinExpr::OP_EQ, 
        int $jop = JoinExpr::B_AND );

    public function __destruct() {}

    public function getPartId() : int {}

    public static function opstr(int $op) : string {}
    public static function boolstr(int $bop) : string {}

    public function emit(int $ix, Bindings $bind, string $lalias, string $ralias): string {}

};

class IColumns implements IfSqlPart {

    public function __construct(?Object $owner);

    public function __destruct() {}

    public function setOwner(?Object $owner) : ?Object {}

    public function getPartId() : int {}

    public function clear(): void{}

    public function getOwner() : ?object {}

    public function getColNames(): array {}

    public function setAlias(string $alias) : void {}

    public function getAlias(): ?string {}

    public function getName(): ?string {}

    public function setColAlias(string $name, ?string $alias = null) : void {}

    public function add(array $columns): void {}

    public function unsetCol(string $key) : void {}

    public function has(string $key) : bool {}

    public function hasColNames(): bool {}

    public function addExpr(string $alias, string $expr) : void {}

    public function getExpr() : array {}
};

class TableAttr implements IfSqlPart {
    public function __construct(string $tname, string $attr);

    public function getPartId() : int {}

    public function __toString() : string {}

    public function getTable() : string {}

    public function getAttr() : string {}

    static public function splitDot(string $colspec) : TableAttr | string;

};

class TColumns extends IColumns implements IfSqlPart {

    public function __construct(string $tname, 
        string|null $alias=null, 
        array|string|null $tcol=null);

    public function __destruct() {}

    public function attr(string $name) : TableAttr;

    public function getPartId() : int {}

    public function setName(string $name) : void {}

    public function getName() : ?string {}

    public static function tableCol(string $expr) : TColumns {}
};

class Param implements IfSqlPart 
{
    public mixed $value;
    
    public function __construct(mixed $value);

    public function getPartId() : int {}

    public function getValue() : mixed {}
};

class JoinInfo {
    const int J_INNER = 0;
    const int J_LEFT = 1;
    const int J_RIGHT = 2;
    const int J_FULL = 3;

    public function __construct(IColumns $LTable, ?IColumns $RTable, int $jtype = JoinInfo::J_INNER);

    public function __destruct();

    public function add(
            mixed $lattr,   
            mixed $rattr = null, 
            int $op = JoinExpr::OP_EQ, 
            int $logic =  JoinExpr::B_NULL) : void {}
    public function addExpr(JoinExpr $expr) : void {}
    public function getConditions() : array {}
    static public function toJoinType(string $join) : int {}
};

class JoinTables {

    public function __destruct() {}

    public function addJoin(JoinInfo $ji) : JoinInfo {}

    public function addResult(TableAttr $attr) : void {}

    public function addTable(IColumns $tc) : void {}

    public function addWhere(mixed $lattr, mixed $rattr=null, 
        int $op=JoinExpr::OP_EQ, 
        int $logic=JoinExpr::B_NULL) : void {}

    public function getData() : array {}

    public function getModel() : ?string {}

    public function getOrder() : array {}

    public function getPivot(): ?IColumns {}

    public function getPrime(): ?IColumns  {}

    public function getTable(string $name) : ?IColumns {}

    public function getTableAlias(string $name) : ?IColumns {}

    public function getTables(): array {}

    public function order(string $name, bool $ascend = true) : void {}

    public static function rowSplit(array $row, array $rename) : object {}
    
    public function setModel(string $name) : void {}

    public function setPrime(IColumns $tc) : void {}

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

    public function quoteName(string $name): string {}

    public function select(Bindings $bind) : ParamList {}

    public function seqLastValue(string $seq_name): string {}

    public function setSeqValue(int $value, array $data): string {}

    public function truncate(Bindings $bindings) : string {}

    public function update(Bindings $bind) : ParamList {}

    public function valuesDefault(): string {}
};

class ParamList {
    public function __construct(\WeakReference $gen);

    public function addParam(mixed $value) : void {}

    public function addParamList(array $values): string {}

    public function getParams() : array {}

    public function getReturns() : ?array {}

    public function getSql() : ?string {}

    public function getValues() : ?array {}

    public function makeList(int $start, int $count): string {}

    public function paramLiteral(mixed $value) : string {}

    public function setParams(array $replace) : void {}

    public function setReturns(array $replace) : void {}

    public function setSql(string $sql) : void {}

    public function setValues(array $replace) : void {}

    public function useOwnValues() : void {}

    public function wipe() : void {}
};



class Raw {
    public function __construct(string $sql, ?\Wcd\IDriver $connect);

    public function setConnection(\Wcd\IDriver $db) : void {}

    public function getConnection() : ?\Wcd\IDriver  {}

    public function getSql() : string {}

    public function execute() : void {}
};


class RunSql {
    public function __construct(IDriver $db, string $sql,
            ?array $bind=null, bool $retval = false);

    static public function Op(IDriver $db, string $sql,
            ?array $bind=null, bool $retval = false ) : mixed {}

    public function operation() : mixed {}
    
    public function run() : mixed {}

};