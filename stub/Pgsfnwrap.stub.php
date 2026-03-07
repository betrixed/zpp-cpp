<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd\Ext\Pgs;


class PgResource {
	public function __construct(object $reso);
	public function __destruct();

	public function close() : void {}
};



class PgQuery extends PgResource {
	public function __construct( Pgsqlfn $pgconnect, string $query);
	public function __destruct();

	public function setParams(array $params) : void {}
	public function execute(bool $fetch = true) : mixed {}
};

class PgResult extends PgResource {
	public function rowFetch(int $fmode) : mixed {}
	public function allRows(?int $rtype = \Wcd\IDriver::FETCH_ASSOC): array {}
	public function allColumns(int $col = 0): array {}
	public function objFetch(): object {}
};

