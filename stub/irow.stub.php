<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd;

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

};




