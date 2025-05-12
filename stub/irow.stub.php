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

    public function getColDefs() : array {}
};


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


