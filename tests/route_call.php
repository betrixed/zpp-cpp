<?php

namespace Wcc;
use Exception;
//use Wcc\Db\IServer;

class Dummy {
    
    public function beforeTest(string $arg)
    {
        echo "beforeTest $arg" . PHP_EOL;
    }
    public function indexGET()
    {
        echo "IndexGet" . PHP_EOL;
    }
    
    
};
require __DIR__ . "/bootstrap.php";



$radd = new RouteAdd();
$radd->prefix("blog");
$radd->addRoutes([
    Route::get("/", new Target(Dummy::class, "indexGET"))
]);

$rset = $radd->getRouteSet();

$rm = new RouteMatch("/blog",Route::GET_I, 1);

if ($rm->findRoute($rset))
{
    echo "Found " . PHP_EOL;
    $before = new Pair(first:'beforeTest', second:["Pass this value"]);
    
    $second = $before->second();
    
    echo "Fetched what? " . gettype($second) . PHP_EOL;
    
    debug_zval_dump($before);
    $result =  $rm->call(extra:null, before:$before, after:null);
    
    
}
else {
    
    echo "Not Found " . PHP_EOL;
}

