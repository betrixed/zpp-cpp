<?php
/**
 * @generate-class-entries
 * @undocumentable
 */

/* class TomlReader {
    static public function decodeFile(string $path) : array {}
    static public function decode(string $s) : array {}
    public function parse(string $s) : array {}
    public function parseFile(string $path) : array {}
}
*/
class Toml {
    static public function decodeFile(string $path) : array {}
    static public function decode(string $s) : array {}
    
    public function parse(string $s) : array {}
    public function parseFile(string $path) : array {}
}

class Day24 {
    public function __construct(?string $t24 = null);

    public function str(string $t24) : void {}
    public function day(float $t24) : void {}
    public function value() : float {}

    public function time(int $hours, int $mins, float $seconds) : void {}
    public function split(int& $h24, int& $min, float& $sec) : void {}
    public function format(int $flags = 0) : string {}
    
    public function __toString() : string {}

    public static function day24_time(int $hours, int $mins, float $seconds) : float {}
    public static function day24_str(string $t24) : float {}
    public static function day24_split(float $value, int &$h24, int &$min, float &$sec) : void {}
    public static function day24_format(float $value, int $flags = 0) : string {}
}

