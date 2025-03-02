<?php
use Wcd\{IBuild,IServer,Simple};

use Wcd\Sql\{Insert, Update, Delete, Select};

use Wc\Valid;

use Db\Models\{
    Payment,
    PayItem
};

class DataCrudTest extends PHPUnit\Framework\TestCase {
    
    private $con;
    const DB_DATETIME = "Y-m-d H:i:s";
    
    protected function setUp() : void {
        $this->con = IServer::connect("default");
    }
    
    
    public function testDeleteAll() {
        
        $build = new IBuild($this->con);
        
        $db = $this->con->iDriver();
        
        $build->table('test');
        
        $count = $build->count('*');
        
        $this->assertTrue( $count > 0 );
        
        $delete = new Delete($db);
        $delete->addPrime('test');
        $delete->run();
        
        $count = $build->table('test')->count('*');
        
        $this->assertTrue($count == 0);
        
    }
    public function testInsert()
    {
        
        $date = date_create_immutable_from_format(self::DB_DATETIME, "2020-10-15 10:10:10");
        $date_fmt = $date->format(self::DB_DATETIME);
        
        $db = $this->con->iDriver();
        
        /*
        $insert = new Simple($db);
        
        $sql = "INSERT INTO test (name, created_at) VALUES (?,?)";
         * 
         */
        
        $inTrans = $db->inTransaction();
        
        $this->assertFalse($inTrans);
        
        $insert = new Insert($db);
        $insert->addPrime('test',null, ['name','created_at']);
        $insert->returns(['id']);
        
        $sql = $insert->getSql();

        $rec1 = [ "ezDB",  $date_fmt ];
        
        $rec2 = [ "ezDB1", $date_fmt ];
        
        
        $stmt = $db->prepare($sql);
        $db->bind($stmt, $rec1);
        $result = $db->execute($stmt,true,true);
        
        
        $this->assertIsArray($result);
        $this->assertGreaterThan(0, count($result));

        $inTrans = $db->inTransaction();
        $this->assertFalse($inTrans);
        
        // On firebird, select count(*) interposes a new transaction and commit
        $build = new IBuild($this->con);
        $count = $build->table('test')->count('*');
        $this->assertEquals(1,$count);
        
        $inTrans = $db->inTransaction();
        $this->assertFalse($inTrans);
        // and again
        $stmt = $db->prepare($sql);
        $db->bind($stmt, $rec1);
        $result = $db->execute($stmt,true,true);
        
        $this->assertIsArray($result);
        $this->assertGreaterThan(0, count($result));
        
        $builder = new IBuild($this->con);
        $builder->table('test')->where('name','ezDB1');
        $ct1 = $builder->count();
        
       // and again
        $stmt = $db->prepare($sql);
        $db->bind($stmt, $rec2);
        $result = $db->execute($stmt,true,true);
        
        $this->assertIsArray($result);
        $this->assertGreaterThan(0, count($result));
        
        $builder->table('test')->where('name','ezDB1');
        $ct2 = $builder->count();
        
        $this->assertEquals(1,$ct2 - $ct1);
    }
    
    public function testInsertMulti() {
        $db = $this->con->iDriver();
        
        $delete = new Delete($db);
        $delete->addPrime('test');
        $delete->run();
        
        $inTrans = $db->inTransaction();
        $this->assertFalse($inTrans);
        
        $date = date_create_immutable_from_format(self::DB_DATETIME, "2020-10-15 10:10:10");
        $date_fmt = $date->format(self::DB_DATETIME);
        
        $insert = new Insert($db);
        $insert->addPrime('test',null, ['name','created_at']);
        $insert->returns(['id']);
        
        $sql = $insert->getSql();

        $rec1 = [ "ezDB",  $date_fmt ];
        
        $rec2 = [ "ezDB1", $date_fmt ];
        
        $stmt = $db->prepare($sql);
        
        $db->bind($stmt, $rec1);
        $result1 = $db->execute($stmt,false,true);
        $result2 = $db->execute($stmt,false,true);
        
        $inTrans = $db->inTransaction();
        // for firebird , open statements are also transactions!!
        
        $db->bind($stmt, $rec2);
        $result3 = $db->execute($stmt,true,true);
        

        $build = new IBuild($this->con);
        $count = $build->table('test')->count('*');
        $this->assertEquals(3,$count);
        
        $inTrans = $db->inTransaction();
        $this->assertFalse($inTrans);
        
    }
    public function testUpdate() {
        $db = $this->con->iDriver();
        
        $builder = new IBuild($this->con);
        
        $builder->table('test');
        
        $builder->where('name','ezDB1');
        $avail = $builder->count();
        

        $update = new Update($db);
        $update->addPrime('test',null, ['name']);
        $update->where('name');
        
        //$sql = "UPDATE test SET name = ? WHERE name = ?";
        
        $data = [
            "ezDB", "ezDB1"
        ];
        
        $simple = $update->prepare();
        
        $result = $simple->update($data);
        
        $this->assertEquals($avail, $result);
        
        $result = $simple->update($data);
        
        $this->assertEquals(0, $result);
    }
    
    public function testPaymentTransact() {
        $db = $this->con->iDriver();
        $db->begin(); // in transaction
        
        $caught = false;
        try {
            
            $donation =  Payment::rowSaved(
                    [
                        "memberid" => 1,
                        "emailid" => 0,
                        "total_cost" => 0,
                        "created_at" => Valid::now(),
                        "status" => "new",
                        "trans_fee" => 0,
                        "trans_code" => "null"
                    ]
            );
            
            $id = $donation->id;
            
            
            $qry = new Delete($db);
            $qry->addPrime('pay_item');
            $qry->where('trans_id', $id);
            $qry->run();

            $ci = PayItem::rowSaved([
                "trans_id" => $id,
                "id" => 1, 
                "detail" => "detail",
                "amount" => 1000,
                "purpose" => "Membership",
            ]);
     
            $db->commit();
        }
        catch (\Exception $ex) {
            $caught = true;
            $this->assertEquals("", $ex->getMessage());
        }
        $this->assertFalse($caught);
    }
}
