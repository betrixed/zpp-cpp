<?php
//bootstrap.php
namespace Wcc;




/** Folder containing Wcc PHP source */
$workdir = dirname(__DIR__);

chdir($workdir);

echo "Current directory is $workdir" .  PHP_EOL;

$wcc_root = "php";

function get_version(string $extname)
{
    $result = phpversion($extname);
    if (empty($result))
    {
        return null;
    }
    return $result;
}
function show_versions() 
{
    echo "Versions -- " . PHP_EOL;
    $elist = ["wcc","wccz", "wccr", "wccd", "wccm", "runsa","xdebug"];
    foreach($elist as $extn)
    {
        $version = get_version($extn);
        if ($version) {
            echo "$extn $version" . PHP_EOL;
        }
    }  
}

function boolstr(bool $value) : string
{
    return $value ? "true" : "false";
}

$extloader = function(string $file) {
    echo "load $file\n";
    return require_once($file);
};

if (!class_exists(Loader::class))
{
    $extloader($wcc_root . DIRECTORY_SEPARATOR . 'Wcc/Loader.php');
}

$loader = Loader::instance();
$loader->setExtLoader($extloader);
$loader->setBaseDir($wcc_root);


$finder = $loader->getFinder();
$finder->addPath('voku\helper', "$wcc_root/voku/helper");
$finder->addPath("Db\Models", "tests/Db/Models");

$svc = Services::instance();

class_alias('WeakReference', 'Wcd\Adbref');

$debug_flags =  DebugLog::TO_CONSOLE;

$path = "test.log";

$log = new DebugLog($path, $debug_flags);

DebugLog::setInstance($log);
$log->start("<pre>test.log", $debug_flags);



$cfg = $svc->getOne(Config::class);

$svc->set('config', $cfg);

$cfg->config_dir = 'tests/modules';

$svc->set('dos', new Dos());
$svc->set('response', new Response());
$svc->set('security', new Security());

echo "BaseDir is " . $loader->getBaseDir() . PHP_EOL;

function shutdown()
{
    $svc = Services::instance();
    $cfg = $svc->get('config');
    if (is_object($cfg))
    {
        $cfg->clear();
    }

    $svc->clearDefer();
    $svc->clearActive();
    $svc->clearObjects();
    ReflectCache::instance()->clear();

}

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




