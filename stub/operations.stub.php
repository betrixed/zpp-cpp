<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd\Sql;

class Simple {
	public function __construct(IDriver $db);

	public function __destruct();

	public function arrayMap(string $keycol, string $valcol, string $table) : ?array {}

	public function arraySet(string $sql, ?array $params = null ) 
		: ?array {}

	public function bind(mixed $value) : string {}

	public function exec(string $sql, 
		?array $params = null) : int {}

	public function firstRow(string $sql, ?array $params = null )
    : ?array {}

    public function getRows() : ?array {}

    public function getSchemaName() : string {}

    public function insert(array $values) : mixed {}

    public function prepare(string $sql) : void {}

    public function quoteName($name) : string {}

    public function returnsValues(bool $retval) : void {}

    public function run() : mixed {}

    public function setValues(array $params) : void {}

    public function update(array $values) : mixed {}
};
/*
class Operation {
	public function __construct(IDriver $db);

	public function __destruct();

	public function addPrime(string $table, 
		?string $alias = null, ?array $cols = null) : TColumns {}

	public function firstRow(int $fetch) : mixed {}

	public function getJoiner() : JoinTables {}

	public function getParams() : ?array {}

	public function prepare(int $fetch) : Simple {}

	public function getRows(int $fetch) : ?array {}

};
*/