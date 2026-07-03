<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd;


class IConfig {
    public function assign(array $cfg) : void {}
    
    public function getArray() : array {}
    public function getCharset() : string {}
    public function getCollation() : string {}
    public function newConnect(string $name) : IDriver {}
    public function getDriverClass() : string {}
    public function getPassword() : string {}
    public function getUsername() : string {}
    public function getPort() : int {}
    public function getHost() : string {}
    public function getDatabase() : string {}
    public function getSqlClass() : string {}
    public function getDmlBuildClass() : string {}

    public function get(string $key) : mixed {}
    public function set(string $key, mixed $value): void {}

    public function getMyKey() : string {}
    public function setMyKey(string $value) : void {}
    
    public function newSql() : \Wcd\Sql\ISql {}
};


class IServer {

    const string DEFAULT_NAME = "default";

    public static string $activecfg = "default";
    
    public static function Connect(?string $name = null) : ?\WeakReference {}

    public static function Instance() : IServer;

    public function __construct(string $svkey) {}

    public function __destruct() {}

    public function addConfig(IConfig $cfg, string $name = "default") : void {}

    public function getConnect(?string $name = null): ?\WeakReference {}

    public function config(array $data) : void {}

    public function getAliases() : array {}

    public function getConfig(string $name) : ?IConfig {}

    public function getConfigNames() : array {}

    public function getDataCache() : ?ICache {}

    public function getDriverClass(string $driver): string {}

    public function getDriverClasses(): array {}

    public function getSqlClass(string $key): string {}

    public function getSqlClasses(): array {}

    public function initDone() : void {}

    public function setAlias(string $alias, string $name) : void {}
};