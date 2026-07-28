<?php

namespace Wcc;
use Exception;

/**
 * Convert xml format to something not quite toml compatible, as
 * some "table" keys refer to object @<classname>
 * which has public properties.
 * Used for confirming XmlRead correctness. 
 * 
 * Pro: lose wrapping xml tags.
 * 
 * Con: Creates [get.long.table.path.names] 
 *      Lose the type specific tag names. 
 */
require ("bootstrap.php");

$datadir = __DIR__;

$datafiles = [
    'schema.xml'
];


function tomlStringVal(string $val) : string {
    if (str_contains($val,"\n") ||  (strlen($val) > 80)) {
        $out = '"""' . addcslashes($val,"\r\b\f\t");
        $out .= '"""';
        return $out;
    }
    if (str_contains($val, "'"))
    {
        if (str_contains($val, "\"")) {
            $val = str_replace("\"","\\\"\\\"", $val);
        }
        return '"' . $val . '"';
    }
    return  "'" . $val . "'";
}


function tablepath(array $keystack = []) : string
{
   $out = "";
   if (!empty($keystack)) {
        $out .= "[";
        $ix = 0;
        foreach($keystack as $value)
        {
            if ($ix > 0) {
                $out .= ".";
            }
            $ix++;
            $out .= $value;
        }
        $out .= "]";
    } 
    return $out;
}

function objectpath(array $keystack, object $val) : string
{
    $out = tablepath($keystack);
    $out .= ' @' . get_class($val);
    return $out;
}

function array2Toml(mixed $data, array $keystack = []) : string 
{
    $out = "";
   
    if (is_object($data))
    {
        $out .= objectpath($keystack, $data);
        $out .= PHP_EOL;
        
        if ($data instanceof \DateTimeInterface)
        {
            $out .= $data->format('Y-m-d H:m:s');
            return $out;
        }
    }
    else if (is_array($data))
    {
        $out .= tablepath($keystack);
        $out .= PHP_EOL;
    }
   
    foreach($data as $ix => $val) 
    {
        $dtype = gettype($val);
        $keytype = gettype($ix);
        
        switch($keytype) {
            case 'string':
                $key = $ix;
                break;
            case 'integer':
                $key = strval($ix);
                break;
            default:
                throw new Exception('Invalid key ' . $ix);
        }
        
        $match = null;
        if (!preg_match('/[A-Za-z0-9_-]*/', $key, $match)) {
            if (!str_contains($key, "\"")) {
                $key .= '"' . $key . '"';
            }
            else if (!str_contains($key,"'")) {
                $key .= "'" . $key . "'";
            }
            else {
                throw new Exception('2 Quote types in key ' . $ix);
            }
        }
        
        switch($dtype) {
            case 'string':
                $out .= $key . "=";
                $out .= tomlStringVal($val);
                break;
            case 'int':
                $out .= $key . "=";
                $out .= intval($val);
                break;
            case 'float':
                $out .= $key . "=";
                $out .= $val;
                break;
            case 'array':
            case 'object':
                $keystack[] = $key;
                $out .= array2Toml($val, $keystack);
                array_pop($keystack);
                break;
            case 'boolean':
                $out .= $key . "=";
                if ($val) {
                    $out .= "true";
                }
                else {
                    $out .= "false";
                }
                break;
            default : 
                $out .= $key . "=";
                $out .= (string) $val;
                break;
        }
        $out .= PHP_EOL;
    }
    return $out;
}

$reader = new XmlRead();
foreach($datafiles as $file) {
    $root = $reader->parseFile($datadir . "/" . $file);
    $data = $root->toArray();
    echo array2Toml($data) . PHP_EOL;
}

