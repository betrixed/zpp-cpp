<?php

namespace Wcc;

use Wcc\Db\IServer;



require "bootstrap.php";


/** $schemafile1 = "tests/schema.xml";
$rdr = new XmlRead();

$sobj = $rdr->parseFile($schemafile1);

echo "getclass = " . get_class($sobj) . PHP_EOL;
*/

require "dbsetup.php";

echo "DB is setup " . PHP_EOL;

$cname = IServer::class;

debug_zval_dump($cname);


$schema_file = "tests/schema_dump.xml";

$con = IServer::connect("default");

$schema_class = $con->getSchemaClass();

echo "schema class is " . $schema_class . PHP_EOL;

$schema = ReflectCache::staticInstance($schema_class);

$schema->readSchema($con->iDriver());

$schema->toFile($schema_file);

if (file_exists($schema_file)) 
{
    $data = ReadCache::Xml($schema_file);
    echo "got data in object " . get_class($data) . PHP_EOL;
}

//debug_zval_dump($data);

