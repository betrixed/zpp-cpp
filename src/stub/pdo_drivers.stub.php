<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd\Ext;

class PdoDriver extends \Wcc\IDriver 
{
    
    public function getAttribute(int $attkey) : mixed {}

    public function getCaseAttribute(): int {}

    public function getConnectOptions(): array {}

    static public function pdoType(mixed $val): int {}

    public function query_lcase(string $sql, int $fmode) : array {}

    public function setAttribute(int $attkey, mixed $value)  : bool {}

};

class PdoMysql extends PdoDriver {

    //public function getSqlType() : string {}
};
    
class PdoPgsql extends PdoDriver {
	const string QUOTE_CHARACTER = "\"";
    const int DEFAULT_PORT = 5432;

    //public function getSqlType() : string {}

};

class Pgsqlfn extends \Wcc\IDriver  {
    public function nextId() : int {}
    public static function attribute(string $name, string $value): string {}
};

class Mysqlfn extends \Wcc\IDriver  {
    
};