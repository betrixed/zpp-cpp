<?php

namespace Wcc;

use Wcd\IServer;
use Wcc\Dos;
use Wcd\Schema\CSV;


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

$schema_dir = "tests/dbdump";

$services = Services::instance();

$dos = new Dos();

$services->set('dos', $dos);


if (!is_dir($schema_dir)) {
    mkdir($schema_dir);
}
else {
    $dos->rm_alldir($schema_dir);
}
$schema_file = "$schema_dir/schema_dump.xml";


$dbref = IServer::connect("default");

$con = $dbref->get();

$schema_class = $con->getSchemaClass();

echo "schema class is " . $schema_class . PHP_EOL;

$schema = ReflectCache::staticInstance($schema_class);

$schema->readSchema($con);

$schema->toFile($schema_file);

if (file_exists($schema_file)) 
{
    $data = ReadCache::Xml($schema_file);
    echo "Datasaved to " . $schema_file . PHP_EOL;
}

$folderName = $schema_dir . "/_dir";
if (!file_exists($folderName)) {
    $dos->make_dir($folderName);
}
$dct = 0;
foreach ($schema->tables as $tdef) 
{
    if ($tdef->type === 'table') 
    {
        CSV::exportTable($con, $tdef, $folderName . '/' . $tdef->name . '.csv'); 
        $dct++;
    }
}

echo "************* DUMPED $dct TABLES ******************" . PHP_EOL;

shutdown();
       
//debug_zval_dump($data);

