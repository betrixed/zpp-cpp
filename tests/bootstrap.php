<?php
//bootstrap.php
namespace Wcc;

/** Folder containing Wcc PHP source */
$workdir = dirname(__DIR__);

chdir($workdir);

//echo "Current directory is $workdir" .  PHP_EOL;

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

if (!class_exists(Finder::class)) {
	require $wcc_root . DIRECTORY_SEPARATOR . "Finder.php";
}
	
require $wcc_root . DIRECTORY_SEPARATOR . "Loader.php";

$finder = new Finder();
$finder->addPathArray([
    "Wcc" => "php/Wcc",
    "Wcd" => "php/Wcd",
    "Wcf" => "php/Wcf",
    "Wc" => "php/Wc",
    "voku\helper" => "php/voku/helper"
]);

$loader = new Loader("", $finder);
$loader->register();


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




