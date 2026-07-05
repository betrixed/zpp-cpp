<?php
namespace Wcc;
use Wcd\{IConfig,IServer};
use Db\Models\Users; 	
use Wc\Link\LoginHelper; 

include "bootstrap.php";

$_SERVER['HTTP_HOST'] = "pcan.test:8080";
$_SERVER['SERVER_NAME']  = "pcan.test:8080";
$_SERVER['REQUEST_URI']  = "/login/login";
$_SERVER['HTTP_ORIGIN'] = "https://pcan.test:8080";
$_SERVER['HTTPS'] = "on";

$req = new RequestGlobals();

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

	$iservers->setAlias("dbref", "database");

	return $iservers;
});

$svc->set('dbref', function($svc){
	$iservers = $svc->get("servers");
	return $iservers->getConnect('database');
});



function urlPrefix($req): string
    {
        return $req->getScheme()
                . "://" . $req->getHttpHost();
    }


$helper = new LoginHelper();

$ok = $helper->loginWithAlias("Michael Rynn", "wat32*waLK");

$prefix = urlPrefix($req);

echo "UrlPrefix is $prefix" . PHP_EOL;


