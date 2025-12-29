<?php
	namespace Wcc;
 	use App\MarkDown;

	include "bootstrap.php"; 

	$sv = Services::instance();

	$finder = $sv->get('finder');

	$finder->addPath("App", "/home/michael/www/book/private/sites/book/lib/App");
	
	$file = file_get_contents("/home/michael/www/book/web/zppbook/dev/src/SUMMARY.md");

	$md = new MarkDown();

	$md->rootdir = "./md?book=zppbook&page=";

	$html = $md->text($file);

	echo $html;
