<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd\Ext\Pgs;



class PgQuery {
	public function __construct( Pgsqlfn $pgconnect, string $query);
	public function __destruct();

	public function close() : void {}
	public function setParams(array $params) : void {}
	public function execute(bool $fetch = true) : mixed {}
};


