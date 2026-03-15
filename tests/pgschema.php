<?php

namespace Wcc;

use Wcd\IServer;
use Wcc\Dos;
use Wcd\Schema\CSV;
use Wcc\{CacheMgr, ReadCache};

require "bootstrap.php";

$path = "tests/_data/schema.xml";

$data = ReadCache::Xml($path);