<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;


class DebugLog {

	const int TO_FILE = 1;

	const int TO_CONSOLE = 2;

	const int FILE_APPEND = 8;

	const int FILE_LOCK = 2;


	public readonly string $filename;

	protected int $outputs;

	public static function instance() : ?Debug {}

	public static function start(?string $msg = null) : ?Debug {}

	public function __construct(string $path, int $destflags = TO_FILE);

	public function line(string $s, int $addflags = FILE_APPEND) : void {}

	public function setOutputs(?int $destflags = TO_FILE);


};