<?php
//services_1.php
namespace Wcc;

use Wcc\RequestGlobals;

require "bootstrap.php";

function hide1() {
	if (class_exists(EmptyTest::class)) {
		$c1 = new EmptyTest();
		echo "has emty" . PHP_EOL;
	}
	else {
		$c1 = null;
	}



	$svc = Services::instance();

	//debug_zval_dump($svc);
	
	$cfg = $svc->newInstance(Config::class);
	//$svc->setObject($cfg);

	echo "has setObject" . PHP_EOL;

	$_SERVER["HTTP_HOST"] = 'pcan.test';
	//$_SERVER["REQUEST_URI"] = '/index.php?stuff=garbage';

	$_FILES["files"] = [
		"name" => ["human_material_consumption_growth.webp"],
		"full_path" => ["human_material_consumption_growth.webp"],
		"type" => ["image/webp"],
		"tmp_name" => ["/tmp/phpMvFFdk"],
		"error" => [0],
		"size" => [67446]
	];

	$request = $svc->newInstance(RequestGlobals::class);

	echo "get HttpHost" . PHP_EOL;
	$host = $request->getHttpHost();

	$test = $request->getURI(true);
	echo "URI is " . $test . PHP_EOL;

	$count = $request->numFiles(false);

	echo "File Count-false = " . $count . PHP_EOL;

	$count = $request->numFiles(true);

	echo "File Count-true = " . $count . PHP_EOL;

	$files = $request->getUploadedFiles(false, false);

	echo "Files - " . print_r($files, true) . PHP_EOL;

	echo "count files = " . count($files) . PHP_EOL;
	echo "test uri = $test" . PHP_EOL;

	$c2 = Services::getOne(EmptyTest::class);


	//$hnew = Services::getOne(HtmlGem::class);

	echo "got object " . print_r($c2, true) . PHP_EOL;

	$svc->set(
		"emty", function(Services $svc){
			//echo "called back with service " . debug_zval_dump($svc) . PHP_EOL;
			return new EmptyTest();
		});

	echo "set called" . PHP_EOL;

	$svc->set("config", new Config());

	echo "set config" . PHP_EOL;

	try {
		$d1 = $svc->get("emty");

		echo "got emty" . PHP_EOL;
		//debug_zval_dump($d1);
		$d2 = $svc->get("config");
		$d2->test = "property";

		//debug_zval_dump($d2);
	}
	catch (\Error $ex)
	{
		echo "Service get failed " . ex.getMessage() . PHP_EOL;
	}

	$ic = new ICache([], $svc);

	echo "icache" . PHP_EOL;

	$cfg = $ic->getService('config');
	echo "service $cfg->test" . PHP_EOL;

	//debug_zval_dump($ic);

}


class testsa extends ServiceAccess {
	public function __construct(?Services $svc = null)
	{
		parent::__construct($svc);
	}
};

function hide2()
{
	$svc = Services::instance();
	$test1 = new testsa();

	$test2 = new testsa($svc);

	$safetch = $test1->config;

	echo "service access $safetch->test" . PHP_EOL;
}

class depends extends ServiceAccess {
	public int $xvalue = 1;

	public function __destruct()
	{
		echo "destructed\n";
	}
};

function itdepends() 
{
	$d = new depends();

	echo "depends xvalue = " . $d->xvalue . PHP_EOL;

	$d = null;
}

//hide1();
//hide2();
//itdepends();

$svc = Services::instance();
//debug_zval_dump($svc);

$ic = new ICache([], $svc);

echo "icache" . PHP_EOL;


$svc->set('cache', $ic);

echo 'has = ' . $svc->has('cache') . PHP_EOL;

debug_zpp_dump($ic);
//debug_zval_dump($ic);

//$cfg = $ic->getService('config');


//echo "service $cfg->test" . PHP_EOL;