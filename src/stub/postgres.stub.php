<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcd\Sql;

class Postgres extends ISql {
	/** At least add something extra */
	const string NAME_QUOTE = "\""; 

	/** These functions are overridden by C++ virtual functions 
	 *  and are so triggered by calls to ISql base class.
	 * */
	/**
	public function quoteName(string $name) : string {}
	public function seqLastValue(string $seq_name) : string {}
	public function truncate(Bindings $bindings) : string {}
	*/

};