<?php


use Wcc\{
    Config,
    ReadCache,
    ReflectCache,
    Services,
    XmlWrite,
};

use Wcd\{
    IDriver,
    IScript,
    IBuild,
    IServer
};
use Wcd\Models\Generate;
use Db\Models\Venue;

class SchemaTest extends Asserts
{

    protected $cfg;

    protected function setUp(): void
    {
        $this->cfg = Services::getOne(Config::class);
        $cache_all = Services::service('cache_all');
        $cache = $cache_all->getCache('sql_cache');
        $cache->clear();
    }

    protected function tearDown(): void
    {
        $this->cfg = null;
    }

    public function testCreateScript()
    {

        $si = Services::getOne(IServer::class);

        $cfg = $si->getConfig("default");

        fwrite(STDERR, print_r($cfg->getArray(), true) . "\n");

        $driverClass = $cfg->getDriverClass();
        $connectClass = $cfg->getConnectClass();
        $sqlClass = $cfg->getSqlClass();
        //$relClass = $cfg->getRelBuildClass();
        $dmlClass = $cfg->getDmlBuildClass();

        $this->assertNotEmpty($driverClass, "driver class");

        $this->assertNotEmpty($sqlClass, "sql class");
        //$this->assertNotEmpty($relClass, "relBuild class");
        $this->assertNotEmpty($dmlClass, "dml class");

        $con = $si->getConnect("default");
        
        $con = IServer::connect("default");
        $type = $driver->getSqlType();

        $inTrans = $driver->inTransaction();
        $this->assertFalse($inTrans);

        $tables = $driver->getTableNames();

        $inTrans = $driver->inTransaction();
        $this->assertFalse($inTrans);

        if (count($tables) > 0)
        {
            $isql = $con->iSql();
            foreach ($tables as $name)
            {
                $driver->querySingle('drop table ' . $isql->quoteName($name));
            }
        }
        $app = $this->cfg;

        $sql_dir = $app->data_dir . "/sql";
        $file = $sql_dir . "/" . $type . ".sql";
        if (file_exists($file))
        {
            $script = new IScript();
            $script->loadFile($file);
            $script->run($driver);

            $script->clear();
        }

        $tables = $driver->getTableNames();
        $this->assertTrue(is_array($tables) && (count($tables) > 0));

        $inTrans = $driver->inTransaction();
        $this->assertFalse($inTrans);
    }

    
    public function testSchemaDump()
    {
        $cfg = $this->cfg;

        $con = IServer::connect("default");

        $schema_class = $con->getSchemaClass();

        $schema = ReflectCache::staticInstance($schema_class);

        $schema->readSchema($con);

        $schema->toFile($cfg->schema_file);

        $this->assertFileExists($cfg->schema_file);

        $data = ReadCache::Xml($cfg->schema_file);

        $this->assertEquals($data, $schema);

        $inTrans = $con->inTransaction();
        $this->assertFalse($inTrans);
    }

    /*
      public function testSchemaFileRead() {
      $cfg = $this->cfg;

      $con = Servers::connect("default");

      $data = Read::fromXml($cfg->schema_file);


      }

     */

    public function testDeleteDir()
    {
        $cfg = $this->cfg;
        $svc = Services::instance();
        $dos = $svc->get('dos');

        $dos->rm_alldir($cfg->models_dir);

        $this->assertDirectoryExists($cfg->models_dir);
        $this->assertEmpty($dos->dirList_ByExt($cfg->models_dir, ["php"]));
    }

    public function testCreateModels()
    {

        $con = IServer::connect("default");

        $schema = $con->getSchema();

        $tables = $schema->getTables();
        //string $folder, string $namespace, ?array $mappings = null,
        //    \Closure $output = null)
        $cfg = $this->cfg;

        $nspace = $cfg->models_namespace;

        $path = $cfg->models_dir;

        $result = Generate::make(
                        $con,
                        $path,
                        $nspace,
                        null,
                        function (string $s) {
                            
                        });

        $ok = is_array($tables) && !empty($tables) && ($result === count($tables));
        $this->assertTrue($ok);
    }

    public function testBuildMultiInsert()
    {
        //insert 1 rows
        $con = IServer::connect("default");

        $inTrans = $con->inTransaction();
        $this->assertFalse($inTrans);

        $date = date("Y-m-d H:i:s");

        $values = [
            ['name' => "ezDB", 'created_at' => $date],
            ['name' => "ezDB1", 'created_at' => $date]
        ];
        // 1-D
        $model_class = $con->modelClassName("test");
        $model = Services::getOne($model_class);
        $builder = $model->getBuilderForMe();

        $row = $model->newRow($values[0]);

        $result = $builder->insert($row);

        //echo "insert result = " . print_r($result,true) . "\n";

        $this->assertEquals(1, $result);

        $inTrans = $con->inTransaction();
        $this->assertFalse($inTrans);

        $rows[] = $model->newRow($values[0]);
        $rows[] = $model->newRow($values[1]);
        $result = $builder->insert($rows);
        
        $this->assertCount(2, $result);

        // now 2 records in test have name of "ezDB"
        $builder->table('test')->where('name', 'ezDB');
        $ct = $builder->count();

        $this->assertEquals(2, $ct);

        $builder->wipe();

        // 1 record in test has name of "ezDB1"
        $builder->table('test')->where('name', 'ezDB1');
        $ct = $builder->count();
        $this->assertEquals(1, $ct);

        $inTrans = $con->inTransaction();
        $this->assertFalse($inTrans);
    }

    public function testSelect()
    {
        $con = IServer::connect("default");

        $builder = new IBuild($con);
        $builder->table('test');
        $bind = $builder->getBindings();
        
        $result = $builder->table('test')
                ->setFetch(IDriver::FETCH_OBJECT)
                ->where('name', '=', 'ezDB')
                ->get();

        $this->assertIsArray($result);
        $this->assertCount(2, $result);
        $obj = $result[0];
        $this->assertIsObject($obj);
        $this->assertEquals('ezDB', $obj->name, "selected where value =");
    }

    public function testAutoincrementValue()
    {
        $venue = Venue::row(['name' => 'Bob']);
        $venue->create();
        $this->assertTrue($venue->id > 0);
    }
    
}
