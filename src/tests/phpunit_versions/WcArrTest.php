<?php

use Wcc\Arr;


class ChildArr extends Arr {
    public $property = null;
    protected $protect = null;
    
    function setProtect(mixed $val) {
        $this->protect = $val;
    }
    
    function getProtect() : mixed {
        return $this->protect;
    }
    
}
class WcArrTest extends PHPUnit\Framework\TestCase
{
    public function testPropertySet() {
        $arr = new Arr();
        $arr['property'] = 'value';
        $arr->property = "value2";
        
        
        $test = $arr['property'];
        $this->assertEquals("value2", $test);

        $test2 = $arr->property;
        $this->assertEquals("value2", $test2);

    }
    
    /**
     * Data value in array has same name as public property.
     * Public property exists, __get not called.
     * Properties cannot be created dynamically, only declared
     */
    public function testChildProperty() {
        $arr = new ChildArr();
        $arr['property'] = 'value';
        $arr->property = 1;
        $test = $arr['property'];
        unset($arr['property']);
        
        $this->assertEquals("value", $test);
        $test2 = $arr->property;
        $this->assertEquals(1, $test2);
        
        $arr->notProperty = "value3";
        $test3 = $arr['notProperty'];
        $this->assertEquals("value3", $test3);
        
        unset($arr->notProperty);

        $this->assertTrue(property_exists($arr, "property"));
        
        $arr->property = 1;
        $test4 = $arr->property;
        $this->assertEquals(1, $test4);
        $this->assertTrue(property_exists($arr, "property"));
        
        $arr["property"] = 1;
        $arr->property = 2;
        
        $test5 = $arr->property;
        $this->assertEquals(2, $test5);
        
        $arr->property = 3;
        $this->assertEquals(3, $arr->property);
                
        $test6 = $arr["property"];
        $this->assertEquals(1, $test6);
        
        $arr->setProtect(42);
        $this->assertEquals(42, $arr->getProtect());
        
        $arr->protect = 199;
        $this->assertEquals(199, $arr->protect);
        
        $this->assertEquals(42, $arr->getProtect());
    }
}
