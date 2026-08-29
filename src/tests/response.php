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

/*
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
*/

$content = <<<EOS
{
    "glossary": {
        "title": "example glossary",
		"GlossDiv": {
            "title": "S",
			"GlossList": {
                "GlossEntry": {
                    "ID": "SGML",
					"SortAs": "SGML",
					"GlossTerm": "Standard Generalized Markup Language",
					"Acronym": "SGML",
					"Abbrev": "ISO 8879:1986",
					"GlossDef": {
                        "para": "A meta-markup language, used to create markup languages such as DocBook.",
						"GlossSeeAlso": ["GML", "XML"]
                    },
					"GlossSee": "markup"
                }
            }
        }
    }
}
EOS;
$obj->setContentType('application/json');
$obj->setContent($content);
$obj->send();

$headers = $obj->getHeaders();

debug_zpp_dump($headers);


echo "Content Sent".PHP_EOL;