<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd\Sql;

class IParams
{

    static public function param(int $ct) : string {}

    public function paramStr(int $ct) : string {}
    
    // Confusing, If requiires setup with reverse arrangement
    //  with a real value.
    public function addParamEquals(mixed $value): string {} 
    
    public function wipe() : void {}
    
    public function paramLiteral(mixed $obval) : string {} 
    
    public function addParamList(array $values): string {} 

    public function makeList(int $start, int $count): string {} 
    
    public function getValues() : ?array {} 
   
    public function useOwnValues() : void {}
    
    public function setValues(array $values) : void {}
    
    public function getReturns() : ?array {}
    
    public function setReturns(array $returns) : void {}
    
    public function getSql() : ?string {}
    
    public function setSql(string $sql) : void {}

    public function setParams(array $replace) : void {}    

    public function getParams() : array {}

    public function __destruct();
};


class NamedParams extends IParams
{
    static public function n_param(int $ct) : string;
    
    public function paramStr(int $ct) : string {}
    
    // Confusing, If requiires setup with reverse arrangement
    //  with a real value.
    public function addParamEquals(mixed $value): string {} 
    
};