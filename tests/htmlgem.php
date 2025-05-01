<?php
namespace Wcc;


require "bootstrap.php";

$hg = new HtmlGem();
$btn_class = "button is-outline is-info";
$link = "/bank/index?orderby=1";
echo "Made new HtmlGem" . PHP_EOL;

//debug_zval_dump($hg);
echo "multiline\n";

echo $hg->multiline(["label" => "push", "name" => "test", "value" => "multiline text", "text"=>"anything"]) . PHP_EOL;

 echo $hg->plainText(['name' => 'title', 'value' => 'My Title', 'size' => 80,
                    'maxlength' => 80]);


echo "email\n";
echo $hg->email(["name"=>"email", "label" => "my label"]) . PHP_EOL;

echo "linkto\n";

echo $hg->linkto(["href"=>"https://server.domain/this/url", "text" => "my text"]) . PHP_EOL;

echo $hg->linkto(["href"=>"https://server.domain/this/url", "text" => "my text"]) . PHP_EOL;

echo $hg->linkTo(['href' => $link, 'btn-glyph' => "fast-backward", 'class' => $btn_class]) . PHP_EOL;

echo "plaintext\n";
echo $hg->plaintext(
	["in-label" => "push", 
	 "name" => "test", 
	 "value" => "plain text box", 
	 "text"=>"anything"]) . PHP_EOL;





$btn_class = "button";
$link = "/articles/this";

echo $hg->linkTo(['href' => $link, 'btn-glyph' => "fast-backward", 'class' => $btn_class]);

$catItems = [1 => "one", 2 => "two", 3 => "three"];
$catId = 1;

echo $hg->select(['label' => 'Category', 'class' => 'selectpicker', 'list' => $catItems, 'name' => 'catId', 'value' => $catId]) . PHP_EOL;

$catItems = ["donate" => "Donation", "member" => "Membership", "other" => "Other"];
$catId = "member";

echo $hg->select(['label' => 'Category', 'class' => 'selectpicker', 'list' => $catItems, 'name' => 'strId', 'value' => $catId]) . PHP_EOL;

echo $hg->hidden(['name' => 'args', 'value' => 'these args here']) . PHP_EOL;


$id = '-id1';

echo $hg->checkbox(['name' => 'op' . $id, 'checked' => 1]) . PHP_EOL;

$dtime = date('Y-m-d H:m:s');

echo "dtime = " . $dtime . PHP_EOL;

echo $hg->datetime(['name'=>'date_published', 'value' => $dtime,  'label' => 'Date', 'class'=>"form-control" ]) . PHP_EOL;

echo $hg->checkbox(['label' => 'Enabled', 'name'=>'enabled', 'checked' => true ]) . PHP_EOL;

$name = "meta";

echo $hg->multiline([ 'div' => 'gemtext', 'name' => $name, 'size'=>60, 'cols'=>60, 
                        'maxlength' => 120	, 
                        'value' => "Test Text lines\nAnd lines", 
                        'style' => "width:100%;"]) . PHP_EOL;



$metatags = [

	[
		"id" => 1,
		"meta_name" => "description",
		"template" => "<meta name='description' content='{}' />",
		"data_limit" => 155,
		"blog_id" => 1484,
		"content" => "stuff"
	],


	[
		"id" => 2,
		"meta_name" => "author",
		"template" => "<meta name='author' content='{}' />",
		"data_limit" => 50,
		"blog_id" => 1484,
		"content" => "Michael Rynn"		
	],
	[
		"id" => 3,
		"meta_name" => "keywords",
		"template" => "<meta name='keywords' content='{}' />",
		"data_limit" => 200,
		"blog_id" => 1484,
		"content" => "my stuff"
	],
	[
		"id" => 4,
		"meta_name" => "og:title",
		"template" => "<meta property='og:title'  content='{}' />",
		"data_limit" => 155,
		"blog_id" => 1484,
		"content" => "my stuff"
	],
	[
		"id" => 5,
		"meta_name" => "og:image",
		"template" => "<meta property='og:image'  content='{}' />",
		"data_limit" => 200,
		"blog_id" => null,
		"content" => null
	],
	[
		"id" => 6,
		"meta_name" => "og:description",
		"template" => "<meta property='og:description'  content='{}' />",
		"data_limit" => 200,
		"blog_id" => null,
		"content" => null
	],
	[
		"id" => 7,
		"meta_name" => "original-source",
		"template" => "<meta name='original-source' content='{}'>",
		"data_limit" => 200,
		"blog_id" => null,
		"content" => null

	],
	[
		"id" => 8,
		"meta_name" => "og:url",
		"template" => "<meta property=\"og:url\" content=\"{}\" />",
		"data_limit" => 100,
		"blog_id" => null,
		"content" => null
	],
	[
		"id" => 9,
		"meta_name" => "og:type",
		"template" => "<meta property=\"og:type\" content=\"{}\" />",
		"data_limit" => 30,
		"blog_id" => null,
		"content" => null

	]	

];


?>

<?php foreach($metatags as $meta):
                $label = $meta['meta_name'];
                $name = 'metatag-' .  $meta['id'];
                $value = $meta['content'];
                $limit = $meta['data_limit'];
                
            ?>

        <div class="columns">
            <div class="column is-one-fifth" align="right">
                <label class="ilab" for='<?= $name ?>'> <?= $label ?></label>
            </div>
            <div class="column">
    <?php if($limit <= 80): ?>
            <?= $hg->plainText(['div' => 'gemtext', 'name'=>$name, 
            		'size'=>65, 'cols'=>65, 'maxlength' => $limit,
                    'class'=> 'gemtext',
                    'value'=> $value,  
                    'style' => "width:100%;"]) ?>

    <?php else: ?>
            <?= $hg->multiline([ 
            	'div' => 'gemtext', 
                'name' => $name, 
                'size'=>60, 'cols'=>60, 'maxlength' => $limit, 
                'value' => $value, 'style' => "width:100%;"]) ?>

    <?php endif ?>
            </div>
        </div>
<?php endforeach ?>


<?= $hg->datetime(['label'=> "From Date", 'name' => "fromtime", 'class' => "form-control"]) ?>

<?php 

$m = HtmlGem::moneyFormat("en_AU");

echo $m->language() . PHP_EOL;

echo $m->format(12550) . PHP_EOL;