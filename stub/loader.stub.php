<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

class Loader
{
    
    public static function instance() : Loader {}

    public static function readPHP(string $file) : mixed {}

    public function __destruct();

    public function setThrowNotFound(bool $val): void {}

    public function setExtLoader(object $extloader) : void {}

    public function setFinder(Finder $finder) : void {}

    public function getFinder() : ?Finder {}

    public function require(string $file): mixed {}

    public function load(string $class): bool {}

    public function mustload(string $class): bool {}
   
    public function setBaseDir(string $path): void {}

    public function getBaseDir(): string {}

    public function regLoader():void {}

    public function unregLoader():  void {}

};