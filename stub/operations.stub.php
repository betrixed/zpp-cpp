<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd\Sql;

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

class Operation {
	
	public function __construct(\Wcd\IDriver $db);

	public function __destruct();

	public function addPrime(string $table, 
		?string $alias = null, ?array $cols = null) : TColumns {}

	public function firstRow(int $fetch) : mixed {}

	public function getBind() : Bindings {}

	public function getDb() : \Wcd\Driver {}

	public function getJoiner() : JoinTables {}

	public function getRows(?int $fmode = null) : mixed {}

	public function getSql() : string {}

	public function getSqlParams() : ParamList {}

	public function limit(mixed $ct, int $start = 0) : void {}

	public function orderBy(mixed $column, bool $descend = false) : void {}

	public function prepare(?int $fmode = null) : Simple {}

	public function returns(array $retn) : void {}

	public function run() : mixed {}

	public function where(mixed $lattr, mixed $rattr = null, ?int $op = null, ?int $logic = null) : void {}

	public function wipe() : void {}
};

class Select extends Operation {
	public function __construct(\Wcd\IDriver $db, bool $autoAlias = false);

	public function __destruct();

	public function add(array $cols) : void {}

	public function addJoin(
		  IColumns $ltable, 
		  ?IColumns $rtable = null, 
		  int $jtype = JoinInfo::J_INNER) : JoinInfo {}

	public function addTable(
		string $table, 
		?string $alias = null, 
		?array $cols=null) : TColumns {}

	public function aggregate(string $fn, string $as, ?array $args) : void {}

	public static function getAlive() : array {}

	public function getRenamed() : array {}

	public function getSqlParams() : ParamList {}

	public function icols() : IColumns {}

	public function setAlias(string $alias) : void {}

	public function wipe() : void {}

};

class Delete extends Operation {
	public function getSqlParams() : ParamList {}
};

class Insert extends Operation {
	public function getSqlParams() : ParamList {}
};

class Update extends Operation {
	
	public function set(string $column, mixed $value): void {}

	public function getSqlParams() : ParamList {}
};
