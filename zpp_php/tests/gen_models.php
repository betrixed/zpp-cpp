<?php
namespace Wcc;
use Wcd\{IConfig,IServer};
use Wcd\Models\Generate;

include "bootstrap.php";

$iserver = new IServer('servers');
$svc->setObject($iserver);

$svc->setDefer('servers', function($svc)
{
	$data = XmlRead::fromFile("tests/database.xml");

	$dbconfig = $data["db-config"] ?? null;
	$iservers = $svc->getObject(IServer::class);

	if ($dbconfig)
	{
		foreach($dbconfig as $name=>$cfg)
		{
			$icfg = new IConfig();
			$icfg->assign($cfg);

			$iservers->addConfig($icfg, $name);
		}
	}

	$iservers->setAlias("db", "database");

	return $iservers;
});

$svc->set('db', function($svc){
	$iservers = $svc->get("servers");
	return $iservers->getConnect('database');
});

$dbref = $svc->get('db');
$con = $dbref->get();
$schema = $con->getSchema();
$tables = $schema->getTables();

$cfg = $con->iConfig();

$nspace = $cfg->get("model_ns");
         
$finder = $svc->get("finder");

$paths = $finder->getNSPaths();
        
$path = $paths[$nspace] ?? null;
        
$dos = $svc->get('dos');

$dos->make_dir($path);

echo "create model classes in " . $path . PHP_EOL;
$dos->rm_alldir($path);
        
$result = Generate::make(
                $con,
                $path,
                $nspace,
                null,
                function (string $s) {
                    echo $s . PHP_EOL;
                });

$ok = is_array($tables) && !empty($tables) && ($result === count($tables));

