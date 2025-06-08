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

class IServer {

    const string DEFAULT_NAME = "default";


    public static function Connect() : ?IDriver {}

    public function __construct(string $svkey) {}

    public function addConfig(IConfig $cfg, string $name = "default") : void {}

    public function getConnect(string $name = IfServer::DEFAULT_NAME): ?IDriver {}

    public function config(array $data) : void {}

    public function getAlias() : array {}

    public function getConfig(string $name) : ?IConfig {}

    public function getConfigNames() : array {}

    public function getConnect() : ?IDriver {}

    public function getDataCache() : ?ICache {}

    public function getDriverClass(string $driver): string {}

    public function getDriverClasses(): array {}

    public function getSqlClass(string $key): string {}

    public function getSqlClasses(): array {}

    public function initDone() : void {}

    public function setAlias(string $alias, string $name) : void {}
};