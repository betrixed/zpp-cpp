<?php
/**
 * @generate-class-entries
 * @generate-legacy-arginfo 80400
 * @undocumentable
 */
namespace Wcc;


class Dos {
	public function __construct(?string $cwd = null);

	public function clean_dir(string $path) : void {}

	public function copy_all(string $from, string $to): void {}

	public function copy_file(string $from, string $to)  : bool {}

	static public function make_dir(string $path, int $permissions = 0755): bool {}

	public function get_cwd() : string;

	public function real_dirpath(string $relp) : string {}

	public function real_filepath(string $relp) : string {}

	public function rm_all(array $files) : bool {}

	public function rm_alldir(string $path, bool $deldir = false) : int  {}

	public function rm_old(array $files, int $tsecs) : int {}

	public function sync_dir(string $src, string $dest): int {}

};