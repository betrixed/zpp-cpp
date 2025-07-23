<?php
use Wcd\{IfCrud, Model};
use Wcd\Sql\{AnyModel};

use Wcc\{Config, Services};

use ErrorException;
use Error;

use Db\Models\Book;
use Db\Models\Venue;
use Db\Models\Author;

class AuthorWithoutKey extends Model
{

    public function __construct()
    {
        $this->name = "author";
        $this->setPKey([]);
    }
}

class AuthorAutoMeta extends Model
{
    public function __construct()
    {
        $this->name = "author";
    }

}

class BaseModelTest extends Asserts {
    private $obj;
    private $cfg;
    
    private function make_new_book_and($save = true)
    {
        $book =  Book::row();

        $book->name = 'rivers cuomo';
        $book->special = 1;

        if ($save)
            $book->create();
        return $book;
    }
    
    protected function setUp() : void {
        $this->obj = new AnyModel("test");
        $this->cfg = Services::getOne(Config::class);
    }
    
    protected function tearDown() : void{
        $this->obj = null;
    }
    
    public function testPKeyptions() {
        $options = $this->obj->getKeyOptions();
        $this->assertTrue(is_array($options) && count($options) > 0);
    }

    public function testTimeStamps() 
    {
        $obj = $this->obj;
        $obj->setTSFlags(IfCrud::ALL_TS);
        
        $row = $obj->newRow();

        $row->stampTime(date("Y-m-d H:m:s"));
        
        $val1 = $row->get(IfCrud::CREATED_AT);
        $val2 = $row->get(IfCrud::UPDATED_AT);
        
        $this->assertIsString($val1);  
        $this->assertIsString($val2);  
        
        $this->assertTrue($row->isDirty(IfCrud::CREATED_AT));
        $this->assertTrue($row->isDirty(IfCrud::UPDATED_AT));
        
        $this->assertTrue(count($row->getDirty()) === 2);
        
        $row->set("name", "Richard III");
        
        $row->create();
    }
    
    
    // tests
    public function testNewVenue()
    {
        $venue =  Venue::row(['name' => 'Tito']);
        $venue->create();
        $obj = Venue::findFirstByName('Tito');
        $this->assertNotEmpty($obj);
    }
    
    public function testNewAuthor()
    {
        $author = Author::rowSaved(['name' => 'Blah Blah']);

        $row = Author::find($author->author_id);
        
        $this->assertNotEmpty($row);
    }   
    
    public function testForgotPrimary()
    {

        $rec =  AuthorWithoutKey::row(['name' => 'Bob!']);
        $this->assertTrue($rec->create());
        $value = $rec->author_id ?? null;
        $this->assertTrue(empty($value));
    }
    
    

    public function testNoPrimary()
    {
        //$this->expectException('ActiveRecord\DatabaseException');
        //if (!$this->conn->supports_sequences())
        //throw new ActiveRecord\DatabaseException('');
        $this->expectException(Error::CLASS);

        $row =  AuthorWithoutKey::row(['name' => 'Joe!']);
        $this->assertTrue($row->create(true));
        
        
    }
    
    public function testUpdatedAt()
    {
        $author =  Author::row(array('name' => 'Blah Blah'));

        $author->save();
        
        $model = $author->getModel();
        $flags =  $model->getTSFlags();
        $this->assertEquals($flags, IfCrud::ALL_TS);
        
        $driver =  $model->getConnect();

        $field = $driver->quoteName('updated_at');
        $query = $driver->lastSQL();

        $this->assertTrue(strpos($query, $field) !== false);
    }
    
    /** Original SQL script has 'Id', not id. Mariadb will be case sensitive
     * on linux, if not windows.
     * On postgresql it will be lower case!!
     * 
     * TODO: postgres and mariadb (mysql) accepted without complaint
     * duplicate inserted name="Bob", address=NULL for UNIQUE(name,address)
     * but firebird threw exception.
     */
    public function testAutoincrementValue()
    {
        $venue =   Venue::row(['name' => 'Alice']);
        $venue->create();
        $this->assertTrue($venue->id > 0);
    }
    
    public function testFindDelete()
    {
        
        $row = Author::find(1);

        $exists = $row->exists();
        $this->assertTrue($exists);
        
        $row->delete();
        $author = $row->getModel();
        
        $exists = $author->exists($row);
        $this->assertFalse($exists);

    }
    /*
    private  function testImportCSVSubs(string $classname, string $filename) {
        if (!file_exists($filename)) {
            return 0; // nothing to do
        }
        $dataRowCt = -1;
        $model = ReflectCache::staticInstance($classname);
        
        $tableName = $model->getTable();

        $db = $model->getConnect();
        $driver = $db->iDriver();
        $columns = array_keys($model->getColDefs());
        $init = false;
        $fieldNames = null;

        $builder = $model->getBuilder();
        $builder->setModel($model);

        $import = fopen($filename, "r");
        try {
            
            $line = $import ? fgetcsv($import) : false;
            while ($line !== false) {
                $values = array_map(
                        function ($value) {
                            return (null === $value) ? null : stripslashes($value);
                        }, $line
                );

                if (!$init) {
                    $driver->begin();
                    if (!empty($values)) {
                        $colCount = 0;
                        foreach ($values as $item) {
                            if (in_array($item, $columns)) {
                                $colCount += 1;
                            }
                        }
                        if ($colCount === count($values)) {
                            $fieldNames = $values;
                        }
                    }
                    if (empty($fieldNames)) {
                        $fieldNames = array_slice($columns, 0, count($values));
                        $dataRowCt = 0;
                    }
                    $init = true;
                }
                if ($dataRowCt == 0) {
                    $bind = [];
                    foreach ($values as $ix => $item) {
                        $bind[$fieldNames[$ix]] = $item;
                    }
                    $builder->setInsert($bind);
                    $args = $builder->prepareBind(ISql::SQL_INSERT);
                    $stmt = $driver->prepare($args[0]);
                    $driver->bind($stmt, $args[1]);
                    $check = $driver->execute($stmt, false, false);
                } elseif ($dataRowCt > 0) {
                    $driver->bind($stmt, $values);
                    $check = $driver->execute($stmt, false, false);
                }
                $dataRowCt += 1;
                $line = fgetcsv($import);
            } // while ($line)
            $driver->closeStmt($stmt);

            if ($import) {
                fclose($import);
                $import = false;
            }
            $seqdefs = $model->getSeqDefs();
            if (!empty($seqdefs)) {
                // Adjust next sequence number(s) after arbitrary inserts.
                $model->sequenceMax();
            }
            $driver->commit();         
            
        } catch (\Exception $e) {
            if ($import) {
                fclose($import);
            }
            $driver->rollback();
            throw $e;
        }

        return $dataRowCt;
    }
     * 
     *
    
     */
    public function testImportCSV()
    {
        $cfg = $this->cfg;

        $data_dir = $cfg->data_dir . "/fixtures";
        
        /* $ct = $this->testImportCSVSubs(Book::class, $data_dir . "/book.csv"); */

        $ct = Book::importFromCSV($data_dir . "/book.csv");

        $this->assertTrue($ct > 1);
    }

    public function testFindAll()
    {
        $all = Book::findAll();
        $this->assertTrue(count($all) > 1);
    }
    
    public function testFindUpdate()
    {
        $book = Book::find(1);
        $new_name = 'different name';
        $book->name = $new_name;
        $book->save();

        $this->assertEquals($new_name, $book->name);
        $b2 = Book::find(1);
        
        $this->assertEquals($new_name, $b2->name);
    }
    
    public function test_update_should_quote_keys()
    {
        $book = Book::find(1);
        $book->name = 'new name';
        $book->save();
        
        $model = $book->getModel();
        $driver = $model->getConnect();
        
        $query = $driver->lastSQL();

        $this->assertTrue(strpos($query, $driver->quoteName('name')) !== false);
    }
    public function testMergeData()
    {
        $book = Book::find(1);
        $new_name = 'How to lose friends and alienate people';
        $attrs = ['name' => $new_name];
        $book->mergeData($attrs)->update();

        $this->assertEquals($new_name, $book->name);
        $this->assertEquals($new_name, $book->name, Book::find(1)->name);
    }
    
    public function test_update_attributes_undefined_property()
    {
        $this->expectException(Error::CLASS);
        $book = Book::find(1);
        $book->mergeData([
            'name' => 'new name',
            'invalid_attribute' => true,
            'another_invalid_attribute' => 'blah'
        ])->update();
    }
    
    public function test_save_null_value()
    {
        $book = Book::first();
        $book->name = null;
        $book->save();
        $ondisk = Book::find($book->book_id);
        $this->assertEquals(null, $ondisk->name);
    }
        
    public function test_save_empty_string()
    {

        $book = Book::find(1);
        $book->name = '';
        $book->save();
        $this->assertEquals('', Book::find(1)->name);
    }
    
    public function test_dirty_attributes()
    {
        $book = $this->make_new_book_and(false);
        $fields = array_keys($book->getData());
        $this->assertEquals(['name', 'special'], $fields);
    }

    public function test_dirty_empty_after_create()
    {
        $book = $this->make_new_book_and();
        $model = $book->getModel();
        
        $driver = $model->getConnect();
        
        $last_sql = $driver->lastSQL();
        $this->assertTrue(strpos($last_sql, 'name') !== false);
        $this->assertTrue(strpos($last_sql, 'special') !== false);
        $dirty = $book->getDirty();
        $this->assertEmpty($dirty);
    }
    public function test_no_dirty_attributes_but_still_insert_record()
    {
        $book =  Book::row();
        $this->assertEmpty($book->getDirty());
        $book->save();
        
        $this->assertEmpty($book->getDirty());
        $this->assertNotNull($book->book_id);
    }
    
    public function test_dirty_attributes_cleared_after_updating()
    {
        $book = Book::first();
        $book->name = 'rivers cuomo';
        $book->save();

        $this->assertEmpty($book->getDirty());
    }
    public function test_dirty_attributes_after_reloading()
    {
        $book = Book::first();
        $book->name = 'rivers cuomo';
        $row = $book->read();
        $this->assertEmpty($row->getDirty());
    }
    public function test_dirty_attributes_with_mass_assignment()
    {
        $book = Book::first();
        $update = ['name' => 'Dirty Update'];
        $book->mergeData($update);
        $dirty = $book->getDirty();
        $this->assertEquals(array_keys($update), $dirty);
    }
    
    public function test_timestamps_set_before_save()
    {
        $author =  Author::row();
        $author->save();
        $this->assertNotNull($author->created_at);
        $this->assertNotNull($author->updated_at);
        $rec = $author->read();
        $this->assertNotNull($rec->created_at);
        $this->assertNotNull($rec->updated_at);
    }
    
    public function test_updated_at_only_set_before_update()
    {
        $author = Author::row();
        $author->save();
        // sleep for at least 1  second
        // otherwise the update time likely will not change
        // as it resolves to seconds only
        sleep(2);
        $created_at1 = $author->created_at;
        $updated_at1 = $author->updated_at;
        $author->name = 'test updated';

        $author->save();

        $this->assertNotNull($author->updated_at);
        $this->assertEquals($created_at1, $author->created_at);
        $this->assertNotEquals($updated_at1, $author->updated_at);
    }
    public function testRowSaved()
    {
        $author = Author::rowSaved(['name' => 'Blah Blah']);
        $reload = Author::find($author->author_id);
        $this->assertNotEmpty($reload);
        $this->assertNotEmpty($author->created_at);
        $this->assertNotEmpty($reload->created_at);
        $this->assertEquals($reload->created_at, $author->created_at);
    }

    public function test_auto_meta()
    {
        $author = AuthorAutoMeta::first();
        $author->name = 'blahhhhhhhhhh';
        $author->save();
        $dirty = $author->getDirty();
        $this->assertEquals(count($dirty),0);
    }
}
