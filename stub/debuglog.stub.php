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

	public static function instance() : ?DebugLog {}

	public static function setInstance(?DebugLog $obj) : void {}

	public static function start(?string $msg = null) : ?DebugLog {}

	public function __construct(string $path, int $destflags = DebugLog::TO_FILE);

	public function line(string $s, int $addflags = DebugLog::FILE_APPEND) : void {}

	public function setOutputs(int $destflags = DebugLog::TO_FILE) : void {}

	public function dump(string $label, mixed $value) : void {}

};