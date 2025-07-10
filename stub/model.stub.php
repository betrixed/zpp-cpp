<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd\Sql;


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