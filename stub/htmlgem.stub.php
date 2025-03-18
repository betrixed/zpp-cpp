<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

interface IfLoadHtml {
     public function gethtml(string $path, array $data) : string {} 
};

interface IfFindLeaf {
    public function findLeaf(string $leaf, ?array $extensions = null) : null|string {}

    public function getPaths() : ?array {}

    public function addPaths(array $paths) : void {}

    public function addPath(string $path) : void {}

    public function setPaths(array $paths) : void {}

    public function clear() : void {}
};

class Money {
    public function __construct(string $lang = 'en-AU');

    public function formatNoSym(mixed $value) : string {}

    public function format(mixed $value) : string {}

    public function symbol() : string {}

    public function language() : string {}
};

class HtmlGem {
	public function button(array $pset) : string {}

    public function checkbox(array $pset) : string {}

    public function datetime(array $pset) : string {}

    public function datetime_text(array $pset) : string {}

    public function email(array $pset) : string {}

    public function figure(array $pset) : string {}

    public function hidden(array $pset) : string {}

    public function linkto(array $pset) : string {}

    public function money(array $pset) : string {}

    public function multiline(array $pset) : string {}

    public function plaintext(array $pset) : string {}

    public function number(array $pset) : string {}

    public function phone(array $pset) : string {}

    public function password(array $pset) : string {}

    public function radio(array $pset) : string {}

    public function select(array $pset) : string {}

    public function select_list(array $pset) : string {}

    public function submit(array $pset) : string {}

    public function text_value(array $pset) : string {}

    public function check_value(array $pset) : string {}

    public function datetime_value(array $pset) : string {}

    public function xcheck(array $pset) : string {}

    public function setLabelClass(string $labelc) : void {}

    public function setTextClass(string $textc) : void {}

    public function setDateIcon(string $html) : void {}

    public function setStyle(string $name, string|array $data) : void {}

    public function getStyle(string $name) : string | null {}

    public function output(mixed $value) : string {}

    static public function moneyFormat(?string $lang) : Money {} 
};

class SearchList implements IfFindLeaf {
    public function __construct(?array $paths = null);

    public function findLeaf(string $leaf, ?array $extensions = null) : null|string {}

    public function getPaths() : ?array {}

    public function addPaths(array $paths) : void {}

    public function clear() : void {}

    public function addPath(string $path) : void {}

    public function hasPath(string $path) : bool {}

    public function setPaths(array $paths) : void {}
};

class PlateEngine {
    public static function fileComment(string $file) : string;

    public function __construct();
    
    public function shareData(array $data, string|array|null $where) : void {}

    public function clearPaths() : void {}
    public function clearPlates() : void {}

    public function getData(?string $name = null) : array {}

    public function getExtensions() : array {}

    public function getFinder() : ?IfFindLeaf {}
    public function setFinder(IfFindLeaf $lfind) : void {}

    public function getLabel() : bool {}

    public function setLoadHtml(IfLoadHtml $loader) : void {}
    public function getLoadHtml() : IfLoadHtml {}

    /* public function getOutputClass() : ?string {}
    public function setOutputClass(string $s) : void {} */

    public function make(string $name, bool $store, string|null $raw) : Plate {}
    public function makeRaw(string $name, string $raw, bool $store=false) : Plate {}
    public function mergePlateData(array $data, string $name) : void {}


    public function render(string $name, array $data=[]) : string {}

    public function setExtensions(array $ext) : void {}
    public function setLabel(bool $doLabel) : void {}


    public function shareWithAll(array $data) : void {}
    public function store(string $name, Plate $tp) : void {}
};



class Plate extends ServiceAccess {
    public function __construct(PlateEngine $e, string $name);
    public function getData() : array {}
    public function getContent() : string {}
    public function getPath() : ?string {}

    public function setData(array $data) : void {}

    public function getPublish() : array {}
    
    public function escape(string $s) : string {}
    public function pathExists() : bool {}
    
    public function fetch(string $name, array $data  = []) : string {}
    public function insert(string $name, array $data = []) : string {}
    public function layout(string $name, array $data  = []) : void {}

    public function push(string $name) : void {}
    public function render(array $data = []) : string {}
    public function section(string $name, string $ifnot = '') : ?string {}

    public function setRaw(string $content) : void {}
    public function start(string $name) : void {}
    public function stop() : void {}
    public function styleBegin() : void {}
    public function styleEnd() : void {}

};

class HtmlPlates {
    public function __construct(?string $model_id = null);

    public function getModel() : object {}

    public function pushLevel(string $name) : void {}

    public function setModel(object $model) : void {}

    public function mergeData(array $items) : void {}

    public function renderView(array $options) : string {}

    public function render(array $options) : string {}

};
