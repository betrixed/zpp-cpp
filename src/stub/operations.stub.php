<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd\Sql;


class Operation {
	
	public function __construct(\WeakReference $db);

	public function __destruct();

	public function addPrime(string $table, 
		?string $alias = null, ?array $cols = null) : TColumns {}

	public function firstRow(int $fetch) : mixed {}

	public function getBind() : Bindings {}

	public function getDb() : \Wcd\Driver {}

	public function getJoiner() : JoinTables {}

	public function getRows(?int $fmode = null) : mixed {}

	public function getSql() : string {}

	public function getSqlParams() : IParams {}

	public function limit(mixed $ct, mixed $start = null) : void {}

	public function orderStr(string $clauses) : void {}

	public function orderBy(mixed $column, 
		bool $descend = false, 
		bool $nullslast = false) : void {}

	public function prepare(?int $fmode = null) : \Wcd\Simple {}

	public function returns(array $retn) : void {}

	public function run() : mixed {}

	public function where(mixed $lattr, mixed $rattr = null, ?int $op = null, ?int $logic = null) : void {}

	public function wipe() : void {}
};

class Select extends Operation {
	public function __construct(\WeakReference $db, bool $autoAlias = false);

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

	//public static function getAlive() : array {}

	public function getRenamed() : array {}

	public function getSqlParams() : IParams {}

	public function icols() : IColumns {}

	public function setAlias(string $alias) : void {}

	public function wipe() : void {}

};

class Delete extends Operation {
	public function getSqlParams() : IParams {}
};

class Insert extends Operation {
	public function getSqlParams() : IParams {}
};

class Update extends Operation {
	
	public function set(string $column, mixed $value): void {}

	public function getSqlParams() : IParams {}
};
