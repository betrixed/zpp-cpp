<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd\Sql;



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
    
};

class Model implements IfCrud {


    public static function KeyValue(array $key, array $value): ?\Wcd\IRow {}

    public static function WithValues(array $values) : ?\Wcd\IRow {}

    public static function __callStatic(string $method, array $parameters) : mixed {}

    public static function classToTableName(string $cname): string {}

    public static function createFromResult(string $mclass, mixed $results, 
        array $eagerLoad = []) : array {}

    public static function find(mixed $id): ?\Wcd\IRow {}

    public static function getTableName() : string {}

    public static function importFromCSV(string $fileName): int {}

    public static function modelBuild(): IBuild {}

    public static function row(array $data = []) : \Wcd\IRow{}

    public static function rowSaved(array $data = []) : \Wcd\IRow{}

    public function createRow(\Wcd\IRow$row, bool $reload = false) : bool {}

    public function createdAtName() : string {}

    public function deleteRow(\Wcd\IRow$row) : bool {}

    public function exists(\Wcd\IRow$row): bool {}

    public function getColDefs() : ?array {}

    public function getBuilder() : \Wcd\IBuild {}

    public function getBuilderForMe() : \Wcd\IBuild {}

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

    public function newRow(array $rdata = [], bool $isSaved = false) : \Wcd\IRow{}

    public function readRow(\Wcd\IRow$row) : \Wcd\IRow{}

    public function saveRow(\Wcd\IRow$row, bool $reload = false) : bool {}

    public function setColDefs(array $cdefs) : void {}

    public function setKeyOptions(array $options) : void {}

    public function setName(string $name) : void {}

    public function setPKey(array $pnames) : void {}

    public function  setSeqDefs(array $sdefs) : void {}

     public function setTSFlags(int $tsval) : void {}

    public function stampTime(string $stamp, 
            int $tsflags = IfCrud::ALL_TS) : array {}

    public function updateRow(\Wcd\IRow$row, bool $reload = false) : bool {}

    public function updatedAtName() : string {}

}
