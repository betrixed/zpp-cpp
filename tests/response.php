<?php
namespace Wcc;


require "bootstrap.php";


$cname = Response::class;

try {
	$obj = Services::getOne($cname);

	$obj->redirect("https://pcan.test/admin/dash", true);
}

catch (\Throwaable $ex) {
	echo "Exception " . $ex.getMessage() . PHP_EOL;
}
echo "Response = " . print_r($obj,true) . PHP_EOL;