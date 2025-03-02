<?php
//searchlist.php
namespace Wcc;

require "bootstrap.php";


function localfn() {
	$svc = Services::instance();

	$svc->set("view_model",function($svc)
	{
		echo "view_model called\n";
		return new Config();
	}
	);


	$view = new HtmlPlates();

	$svc->setDefer("engine",
		function(Services $svc) {
			$engine = new PlateEngine();
			$engine->setLabel(true);
			return $engine;
		}
	);

	$list = new SearchList("plates", ["tests/views", "tests/more_views"]);

	$path = $list->findLeaf("main", ["phtml"]);

	echo "file is $path\n";

	$engine = $svc->get("engine");

	$engine->setPaths($list);
	$engine->setExtensions(["phtml"]);
	$engine->shareWithAll(["title" => "SearchList"]);

	
	$view->render(["views"=>["more", "main"],"final"=>false]);


	$svc->unset("engine");
	$svc->clearActive();
}

localfn();
