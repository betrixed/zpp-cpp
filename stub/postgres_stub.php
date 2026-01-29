<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd\Sql;

class Postgres extends ISql {
	const string NAME_QUOTE = "\""; 

	public function quoteName(string $name) : string {}
	public function seqLastValue(string $seq_name) : string {}
	public function truncate(Bindings $bindings) : string {}

};