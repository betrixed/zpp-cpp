<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

#[\AllowDynamicProperties]
class Module extends Config {

 	const string DEFAULT_MOD = "default";
    const string BASE = "base";
    const string ALIAS = "alias";
    const string ROUTES = "routes";
    const string VIEWPATHS = "view_paths";
    const string NAMESPACES = "namespaces";
    const string REQUIRES = "requires";
    const string CLASSFILES = "classfiles";
    const string DATABASE = "database";
    const string ASSETS = "assets";
    const string ASSET_FILE = "asset_file";

    public function __construct(string $name, array $mod);

    public function activate(Finder $finder) : void {}

    public function addDefaults(Module $def) : void {}

    public function getAssets() : array {}

    public function getName() : string {}

    public function getRequires() : array {}
	
	public function getViewPaths() : array {}

	public function setConfigPath(string $path) : void {}

    public function getActive() : bool {}

    public function setActive(bool $val) : void {}

    public function setRequires(array $rlist) : void {}
    
};