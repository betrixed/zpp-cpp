<?php
//bootstrap.php
namespace Wcc;

/** Folder containing Wcc PHP source */
$workdir = dirname(__DIR__);

chdir($workdir);

echo "Current directory is $workdir" .  PHP_EOL;

$wcc_root = "php/Wcc";

function get_version(string $extname)
{
    $result = phpversion($extname);
    if (empty($result))
    {
        $result = "None";
    }
    return $result;
}
function show_versions() 
{
    $version = get_version("wcc");
    $xdebug = get_version("xdebug");

    echo "wcc $version, xdebug $xdebug\n";    
}

function boolstr(bool $value) : string
{
    return $value ? "true" : "false";
}


if (!class_exists(Loader::class))
{
    require $wcc_root . DIRECTORY_SEPARATOR . "Loader.php";
}

$loader = Loader::instance();

$loader->setBaseDir($wcc_root);

$loader->regLoader();


class EmptyTest {
    public array $empty = [];
    public $key;
    public $value;


    public function indexGET() : mixed {
        return "get somthing";
    }

    public function targetGET() : mixed {
        return "get target";
    }


};




