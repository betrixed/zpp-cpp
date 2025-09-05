<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd\Sql;


class Bindings 
{
    public function __construct(ISql $gen, \Wcd\IDriver $connect);

    public function add(int $key, mixed $value) : void {}

    public function addJoinData(array $data) : bool {}

    public function aliasSelect(): bool {}

    public function get(int $key) : mixed {}

    public function getArray(int $key) : array|null {}

    public function getData(): array {}

    public function getJoins(): JoinTables {}

    public function getParamList() : ParamList {}

    public function iSql() : ISql {}

	public function orderBy(mixed $col, bool $desc) : void {}
    
    public function primeJoin(TColumns $tc): JoinTables {}

    public function set(int $key, mixed $value) : void {}

    public function setParamList(ParamList $params) : void {}

    public function unset(int $key) : void {}

    public function update(string $column, mixed $value) : void {}

    public function wipe(int $ix = -1) : void {}

    public function select(object $prop) : mixed {}


};