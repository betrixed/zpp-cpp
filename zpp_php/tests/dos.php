<?php
namespace Wcc;
use Exception;
//use Wcc\Db\IServer;

require __DIR__ . "/bootstrap.php";


$dos = new Dos();

$src = $finder->dirList_fileExt("tests/pcan_models", ["php"]);

$dos->copy_all("tests/pcan_models", "tests/_models");

$dest = $finder->dirList_fileExt("tests/_models", ["php"]);

$sct = count($src);
$dct = count($dest);

echo "$dct copied from $sct\n";

$dos->rm_alldir("tests/_models");

$dest = $finder->dirList_fileExt("tests/_models", ["php"]);
$dct = count($dest);

echo "After rm_alldir $dct\n";

