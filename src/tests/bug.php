<?php
namespace Wcc;

use Wcd\{IConfig, IDriver, IServer, Simple};
use Wcd\Sql\{IfSql, Select, JoinInfo, Literal, Param, TableAttr};
use Wcc\Services;

require "bootstrap.php";

$finder->addPath('Pcan\Models', "tests/pcan_models");

$setup_servers = function (Services $svc)
{
	$data = [
		'host' => 'localhost',
	    'port' =>  3306,
	    'driver' => 'pdo_mysql',
	    'charset' => 'utf8mb4',
	    'collation' => 'utf8mb4_unicode_ci',
	    'database' => 'pcanex',
	    'username' => 'pcanex',
	    'password' => 'WhatsUpDoc',
	    'model_ns' => 'Pcan\Models',
	    'processor' => 'Wcd\Sql\Mysql'
	];

	$dbconfig = new IConfig();
	$dbconfig->assign($data);

	$servers = $svc->getObject(IServer::class);
	$servers->addConfig($dbconfig, 'default');

	$svc->unset('servers');
};

$services = Services::instance();
$services->set('servers', $setup_servers);

$servers = new IServer('servers');
$services->setObject($servers);


$db = $servers->getConnect('default');

echo "New DB\n";

$db->connect();


$model = $db->getTableModel('articles');

echo "Is connected = " . $db->isConnected() . PHP_EOL;

function getRows(IDriver $db, Config $m) : ?array
{
    /* Too variable and complex to cache */

    $sel = new Select($db);
    $columns = $db->getColumnNames('articles');
    $tc = $sel->addPrime('articles', 'B', $columns);
    $tc->addExpr("full_count", "count(*) over()");

    if (!$m->all)
    {
        $owner = $sel->addTable('blog_owned', 'BO');
        $jowner = $sel->addJoin($owner,null);   
        $jowner->add('blog_id', new TableAttr('B','id'));
        $jowner->add('user_id', $m->memberid);
    }
    else {
        $owner = $sel->addTable('blog_owned', 'BO', ['user_id']);
        $jowner = $sel->addJoin($owner, null, JoinInfo::J_LEFT);   
        $jowner->add('blog_id', new TableAttr('B','id'));
    }

    $me = $sel->addTable('meta', 'M');
    $sel->addJoin($me, null);
    $bm = $sel->addTable('blog_meta', 'A', ['content' => 'author_name']);
    $j3 = $sel->addJoin($bm, $tc, JoinInfo::J_LEFT);
    $j3->add('blog_id', 'id');
    $j3->add(new TableAttr('A', 'meta_id'), new TableAttr('M', 'id'));
    $sel->where(new TableAttr('M', 'meta_name'), new Literal('author'));

    if ($m->catId > 0)
    {
        $bc = $sel->addTable('blog_to_category', 'BC');
        $j4 = $sel->addJoin($bc, $tc);
        $j4->add('blog_id', 'id');
        $sel->where(new TableAttr('BC', 'category_id'), $m->catId);
    }
    $order_sql = explode(' ', $m->order_field);
    if (!empty($order_sql))
    {
        $desc = (count($order_sql) === 1) || (strtolower($order_sql[1]) === 'desc');
        $sel->orderBy($order_sql[0], $desc);
    }
    $sel->limit($m->grabsize, new Param($m->start));

    $results = $sel->getRows();




    //echo debug_zpp_dump($sql) . PHP_EOL;
    //echo debug_zpp_dump($values) . PHP_EOL;
    /**
    $qry = new Simple($db, IDriver::FETCH_ASSOC);

        //This creates a crazy leak.


    $qry->returnsValues(true);

    $plist = $sel->getSqlParams();
    $sql = $plist->getSql();
    $values = $plist->getValues();

   if ($qry->prepare($sql))
   {
        $qry->setValues($values);
        $results = $qry->getRows();
    }
    **/
/** OR PDO_statement style 
    $plist = $sel->getSqlParams();
    $sql = $plist->getSql();
    $values = $plist->getValues();

    $stmt = $db->prepare($sql);
    $db->bind($stmt, $values);
    if ($stmt->execute())
    {
        $results = $db->fetchAllRows($stmt, IDriver::FETCH_ASSOC); 
    }
    $db->closeStmt($stmt);
*/
    return $results;
}

$m = new Config();

$m->numberPage = 1;
$m->catId = 0;
$m->orderBy = 'date';
$m->order_field = 'B.date_saved asc';
$m->all = 1;
$m->grabsize = 16;
$m->start = 0;
$m->memberid = 0;


$results = getRows($db, $m);

echo "Results = " . count($results) . " rows\n";

echo "Done\n";