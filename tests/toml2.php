<?php

require "bootstrap.php";


$data_path = dirname(__DIR__);


function tree_count(array $data) : int {
    $ct = count($data);
    foreach($data as $item) {
        if (is_array($item)) {
            $ct += tree_count($item);
        }
    }
    return $ct;
}

function runit()
{
    global $data_path;

    $parseFiles = [
    'example.toml',
    'fruit.toml', 
    'config.toml',
    'hard_example.toml', 
    'hard_example_unicode.toml'
    ];

    foreach($parseFiles as $name) {
        $obj = new Toml();
        $td = file_get_contents($data_path . "/tests/" . $name);
        try {
            echo "try parse" . PHP_EOL;
            $data = $obj->parse($td);
            echo "Total keys = " . tree_count($data) . PHP_EOL;
            //echo print_r($data) . PHP_EOL;
        }
        catch(Exception $e){
            echo "Caught error: " . $e->getMessage() 
            . PHP_EOL . "File " . $name . PHP_EOL;
        }
        echo "parse done\n"; // . debug_zval_dump($data);
    }
}

runit();


