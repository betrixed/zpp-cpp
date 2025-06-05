<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd;


class IConfig {
    public function assign(array $cfg) : void {}
    public function get(string $key) : mixed {}
    public function getArray() : array {}
    public function getCharset() : string {}
    public function getCollation() : string {}
    public function newConnect(string $name) : IfDriver {}
    public function getDriverClass() : string {}
    public function getPassword() : string {}
    public function getUsernmae() : string {}
    public function getPort() : int {}
    public function getHost() : string {}
    public function getDatabase() : string {}
    public function set(string $key, mixed $value): void {}

    public function getMyKey() : string {}
    public function setMyKey(string $value) : void {}
    
    public function newSql() : IfSql {}
};