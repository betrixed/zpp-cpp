<?php

use Wcc\{Config, Services, UserSession};


class UserSessionTest extends PHPUnit\Framework\TestCase {
    protected $cfg;
    
    protected function setUp() : void {
        $this->cfg = Services::getOne(Config::class);
    }
    
    protected function tearDown() : void{
        $this->cfg = null;
    }
    
    public function testCreateUserSession() {
        
       
        $obj = new UserSession();
        
        $obj->setGuest();
        
        $this->assertEquals($obj->getUserName() , "Guest");
    }
}
