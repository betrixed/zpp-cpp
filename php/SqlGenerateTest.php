<?php


use Wcc\{
    Config,
    ICache,
    Services
};

use Wcd\{IServer,IDriver};
use Wcd\Sql\{
    Select,
    TColumns,
    Literal,
    Param,
    JoinExpr,
    JoinInfo,
    TableAttr,
    Expr
};

class SqlGenerateTest extends Asserts
{

    protected $cfg;
    protected $iterations;

    protected function pdiff($sql, $ans)
    {

        if ($sql !== $ans)
        {
            $pos = strspn($sql ^ $ans, "\0");
            fwrite(STDERR, 'First difference at ' . $pos . "\n");
            fwrite(STDERR, '*1 got- ' . substr($sql, $pos) . "\n");
            fwrite(STDERR, '*2 exp- ' . substr($ans, $pos) . "\n");
        }
    }

    protected function setUp(): void
    {
        $this->cfg = Services::getOne(Config::class);
        $this->iterations = 3;
    }


    protected function tearDown(): void
    {
        $this->cfg = null;
    }

     public function testTableAttr() : void
     {
          $ta = TableAttr::splitDot("T.attr_name");
          $this->assertIsClass($ta, TableAttr::class);
          
          $this->assertEquals($ta->getTable(), "T");
           $this->assertEquals($ta->getAttr(), "attr_name");
          $s = TableAttr::splitDot("attr_name");
          $this->assertEquals($s, "attr_name");
     }
     public function getCache() : ?ICache
     {
         $cache_all = Services::service('cache_all');
         return $cache_all->getCache('sql_cache');
     }
     
     public function key1() : string 
     {
         return __CLASS__ . '_1.blog_sql_test';
     }
     
     public function key2() : string 
     {
         return __CLASS__ . '_2.blog_sql_test';
     }
     
     public function testColumnsData()
     {
          $servers = Services::getOne(IServer::class);
          $db = $servers->getConnect("pcanex");
          
          $loader = Services::service("loader");
          $loader->setThrowNotFound(false);
        
          $obj = $db->getTableModel("articles");
          
          $names = $db->getColumnNames('articles');
          
          $this->assertTrue(count($names) > 0);
          
           $loader->setThrowNotFound(true);
     }
     
     public function testCategoriesSet()
     {
         $id = 1490;
         $servers = Services::getOne(IServer::class);
         $db = $servers->getConnect("pcanex");

        $sel = new Select($db);
        $c = $sel->addPrime('blog_category', 'C', ['id', 'name', 'name_clean']);
        $b = $sel->addTable('blog_to_category', 'B', ['blog_id']);
        $j1 = $sel->addJoin($b, $c, JoinInfo::J_LEFT);
        $j1->add('category_id', 'id');
        //Param adds a value
        $j1->add('blog_id', new Param($id));
        $sel->orderBy('name');

        
        $sop = $sel->prepare();
        
        $result = $sop->getRows();
        
        $this->assertIsArray($result);
        $this->assertNotEmpty($result);
        
     }
     
     public function gen1(IDriver $db) : string
     {
        $limit = 20;
        $sel = new Select($db);
        $links_t = $sel->addPrime('links', 'A',
                ['id', 'url', 'title', 'sitename',
                    'summary', 'urltype', 'date_created']);
        $image_t = $sel->addTable('image', 'B',
                ['name' => 'im_file', 'description' => 'im_caption']);
        $gallery_t = new TColumns('gallery', 'C', ['path' => 'im_path']);
        $j1 = $sel->addJoin($image_t, $links_t, JoinInfo::J_LEFT);
        $j1->addExpr(new JoinExpr('id', 'imageid'));
        $j2 = $sel->addJoin($gallery_t, $image_t, JoinInfo::J_LEFT);
        $j2->addExpr(new JoinExpr('id', 'galleryid'));
        $attr = new TableAttr('A', 'urltype');

        $sel->where($attr, new Literal('Remote'), JoinExpr::OP_EQ);
        $sel->where($attr, new Literal('Front'), JoinExpr::OP_EQ, JoinExpr::B_OR);
        $sel->where($attr, new Literal('Event'), JoinExpr::OP_EQ, JoinExpr::B_OR);
        $sel->where($attr, new Literal('Blog'), JoinExpr::OP_EQ, JoinExpr::B_OR);
        $sel->orderBy('date_created', true);
        $sel->limit($limit);

        $plist = $sel->getSqlParams();
        return $plist->getSql();
        
     }
     public function testGenEx1()
    {
        
        $servers = Services::getOne(IServer::class);
        $db = $servers->getConnect("pcanex");

        $ct = $this->iterations;
        
        $sql = "";
        
        if ($ct > 1)
        {
            $tstart = microtime(true);
            $sql = $this->gen1($db); // discard 1st
             $tend = microtime(true);
             $msec = number_format(($tend - $tstart) * 1000, 3);
             fwrite(STDERR, "\nsql generation 0th discard iteration $msec ms\n");
        }
        $tstart = microtime(true);
        for ($i = 0; $i < $ct; $i++)
        {
            $sql = $this->gen1($db);
        }
        $tend = microtime(true);

        $msec = number_format(($tend - $tstart) * 1000 / $ct, 3);

        fwrite(STDERR, "\nsql generation avg $ct iteration $msec ms\n");

        $ans = <<<EOS
SELECT A.`id`,A.`url`,A.`title`,A.`sitename`,
A.`summary`,A.`urltype`,A.`date_created`,
B.`name` as `im_file`,B.`description` as `im_caption`,
C.`path` as `im_path` FROM `links` A
 LEFT JOIN  `image` B ON  B.`id` = A.`imageid`
 LEFT JOIN  `gallery` C ON  C.`id` = B.`galleryid`
 WHERE A.`urltype` = 'Remote'
 OR A.`urltype` = 'Front'
 OR A.`urltype` = 'Event'
 OR A.`urltype` = 'Blog'
 ORDER BY `date_created` DESC LIMIT ?
EOS;
        $ans = str_replace("\n", "", $ans);
        
        $this->assertIsString($sql);
                
        $sql = str_replace("\n", "", $sql);
        $this->pdiff($sql, $ans);

        $this->assertEquals($sql, $ans);
        
        $cache = $this->getCache();
        if (!is_null($cache))
        {
            $cache->set($this->key1(), $ans);
        }
    }

    public function testSqlCache1()
    {
        $ct =  $this->iterations;
        $tstart = microtime(true);
        $cache = $this->getCache();
        if (is_null($cache))
        {
            return;
        }
        $sql_key = $this->key1();

        for ($i = 0; $i < $ct; $i++)
        {
            $sql = $cache->get($sql_key);
            
        }
        $tend = microtime(true);

        $msec = number_format(($tend - $tstart) * 1000 / $ct, 3);
        
        $impl = get_class($cache);
        fwrite(STDERR, "\nCache $impl avg $ct iteration $msec ms\n");
        
        if ($sql) {
            $this->assertIsString($sql);
            $this->assertStringStartsWith('SELECT', $sql);
        }
        else {
             fwrite(STDERR, "Cache failed\n");
        }
    }

    public function gen2(IDriver $db)
    {
        $sel = new Select($db);
        $prime = $sel->addPrime('blog', 'A',
                ['id', 'title', 'style', 'title_clean']);
        $rev = $sel->addTable('blog_revision', 'R', ['content' => 'article']);

        $event = $sel->addTable('event', 'B',
                ['fromtime' => 'date1', 'totime' => 'date2']);

        $b_fromtime = new TableAttr('B', 'fromtime');
        $b_totime = new TableAttr('B', 'totime');

        $ex1 = new JoinExpr($b_fromtime, null, JoinExpr::OP_NOTNULL);
        $ex2 = new JoinExpr($b_totime, null, JoinExpr::OP_NOTNULL);

        $sql_type = $db->getSqlType();
        if ($sql_type ===
                'sqlite')
        {
            $now = new Expr("datetime('now')");
            $nowfn1 = new JoinExpr(new Expr('datetime(B.fromtime)'), $now,
                    JoinExpr::OP_GT);
            $nowfn2 = new JoinExpr(new Expr('datetime(B.totime)'), $now,
                    JoinExpr::OP_GT);
        } else
        {
            $now = new Expr('NOW()');
            $nowfn1 = new JoinExpr($b_fromtime, $now, JoinExpr::OP_GT);
            $nowfn2 = new JoinExpr($b_totime, $now, JoinExpr::OP_GT);
        }

        $jnow1 = new JoinExpr($ex1, $nowfn1, JoinExpr::OP_AND, JoinExpr::B_NULL);
        $jnow2 = new JoinExpr($ex2, $nowfn2, JoinExpr::OP_AND, JoinExpr::B_NULL);
        $jnow3 = new JoinExpr($jnow1, $jnow2, JoinExpr::OP_OR, JoinExpr::B_NULL);

        $j1 = $sel->addJoin($rev, $prime);
        $j1->add('blog_id', 'id', JoinExpr::OP_EQ);
        $j1->add('revision', 'revision', JoinExpr::OP_EQ, JoinExpr::B_AND);

        $j2 = $sel->addJoin($event, $prime);
        $j2->add('blogid', 'id', JoinExpr::OP_EQ);
        $j2->add($jnow3, null, JoinExpr::OP_NOP, JoinExpr::B_AND);

        $subQ1 = new Select($db);
        $subPrime = $subQ1->addPrime('blog_meta', 'MC', ['blog_id', 'content']);
        $jmeta = $subQ1->addTable('meta', 'M');

        $jsub = $subQ1->addJoin($jmeta, $subPrime);
        $jsub->add('id', 'meta_id');
        $jsub->add('meta_name', new Literal('og:description'));

        $subQ1->setAlias('C');
        $subQ1->add(['content']);
        $j3 = $sel->addJoin($subQ1->icols(), $prime);
        $j3->add('blog_id', 'id');

        $subQ2 = new Select($db);
        $subQ2p = $subQ2->addPrime('blog_meta', 'MC', ['blog_id', 'content']);
        $jm2 = $subQ2->addTable('meta', 'M');

        $js2 = $subQ2->addJoin($jm2, $subQ2p);
        $js2->add('id', 'meta_id');
        $js2->add('meta_name', new Literal('og:image'));

        $subQ2->setAlias('D');
        $subQ2->add(['content' => 'image']);
        $j4 = $sel->addJoin($subQ2->icols(), $prime);
        $j4->add('blog_id', 'id');

        $j5 = $sel->addJoin(new TColumns('links', 'L', ['url']), $prime,
                JoinInfo::J_LEFT);
        $j5->add('refid', 'id');
        $sel->orderBy($b_fromtime);

        $plist = $sel->getSqlParams();
        return $plist->getSql();
    }
    
    public function testGenEx2()
    {
        $db = IServer::connect("pcanex");
     
        $ct =  $this->iterations;
        
        $sql = "";
        
        if ($ct > 1)
        {
            $tstart = microtime(true);
            $sql = $this->gen2($db);
            $tend = microtime(true);
             $msec = number_format(($tend - $tstart) * 1000, 3);
             fwrite(STDERR, "\nsql generation 0th discard iteration $msec ms\n");
        }
        $tstart = microtime(true);
        for ($i = 0; $i < $ct; $i++)
        {
            $sql = $this->gen2($db);
        }
        $tend = microtime(true);
        
        $ans = <<<EOS
SELECT A.`id`,A.`title`,A.`style`,A.`title_clean`,R.`content` as `article`,
B.`fromtime` as `date1`,B.`totime` as `date2`,C.`content`,
D.`content` as `image`,L.`url`
 FROM `blog` A 
INNER JOIN  `blog_revision` R ON  R.`blog_id` = A.`id` AND R.`revision` = A.`revision`
 INNER JOIN  `event` B ON  B.`blogid` = A.`id`
 AND  (  (  ( B.`fromtime` IS NOT NULL ) AND  ( B.`fromtime` > NOW() ) )
 OR  (  ( B.`totime` IS NOT NULL ) AND  ( B.`totime` > NOW() ) ) )
 INNER JOIN  (SELECT MC.`blog_id`,MC.`content` FROM `blog_meta` MC
 INNER JOIN  `meta` M ON  M.`id` = MC.`meta_id` AND M.`meta_name` = 'og:description') C ON  C.`blog_id` = A.`id`
 INNER JOIN  (SELECT MC.`blog_id`,MC.`content` FROM `blog_meta` MC
 INNER JOIN  `meta` M ON  M.`id` = MC.`meta_id` AND M.`meta_name` = 'og:image') D ON  D.`blog_id` = A.`id`
 LEFT JOIN  `links` L ON  L.`refid` = A.`id` ORDER BY B.`fromtime` ASC
EOS;
        $ans = str_replace("\n", "", $ans);
        $sql = str_replace("\n", "", $sql);
        $this->pdiff($sql, $ans);
        $this->assertIsString($sql);
        $this->assertEquals($sql, $ans);
        
        $msec = number_format(($tend - $tstart) * 1000 / $ct, 3);

        fwrite(STDERR, "\nEx2 sql avg $ct iterations $msec ms\n");
        $this->assertStringStartsWith('SELECT', $sql);
        
       $cache = $this->getCache();
       if ($cache)
       {
           $cache->set($this->key2(), $ans);
       }
    }
    
    public function testSqlCache2()
    {
        $cache = $this->getCache();
        if (is_null($cache))
        {
            return;
        }
        $ct =  $this->iterations;
        $tstart = microtime(true);

        $sql_key = $this->key2();

        for ($i = 0; $i < $ct; $i++)
        {
            $sql = $cache->get($sql_key);
        }
        
        if ($sql)
        {
            $tend = microtime(true);

            $msec = number_format(($tend - $tstart) * 1000 / $ct, 3);
            $this->assertIsString($sql);
            fwrite(STDERR, "\nEx2 cache avg $ct iteration $msec ms\n");

            $this->assertStringStartsWith('SELECT', $sql);
        }
        else {
             fwrite(STDERR, "Cache failed\n");
        }
    }

    
}
