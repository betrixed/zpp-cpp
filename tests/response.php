<?php
namespace Wcc;


require "bootstrap.php";


$cname = Response::class;

try {
	$obj = Services::getOne($cname);

	//$obj->redirect("https://pcan.test/admin/dash", true);

	//$obj->setContent()
}

catch (\Throwaable $ex) {
	echo "Exception " . $ex.getMessage() . PHP_EOL;
}
//echo "Response = " . print_r($obj,true) . PHP_EOL;

$content = <<<EOS
<!DOCTYPE html>
<html><head>
<title>Home</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0" >
</head>
<body>Hello World
</body>
</html>
EOS;

$obj->setContentType("text/html", "UTF-8");
$obj->setContent($content);
$obj->send();