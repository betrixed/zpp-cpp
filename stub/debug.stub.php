<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;


class Debug {

	public bool $echo = false;

	public bool $log = true;

	public readonly ?string $logfile;

	public static function instance() : ?Debug {}

	public static function start(?string $msg = null) : ?Debug {}

	public function __construct(?string $logpath);

	public function line(string $s, int $flags = FILE_APPEND) : void {}

};