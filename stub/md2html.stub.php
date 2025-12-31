<?php
/**
 * @generate-class-entries
 * @undocumentable
 */
namespace Wcc;

class MarkToHtml 
{
	const int COLLAPSE_WHITESPACE  = 0x1; /* In MD_TEXT_NORMAL, collapse non-trivial whitespace into single space */

	const int PERMISSIVE_ATX_HEADERS = 0x2;   /* Do not require space in ATX headers ( ###header ) */

	const int PERMISSIVE_URL_AUTOLINKS = 0x4; /* Recognize URLs as autolinks even without '<', '>' */

	const int PERMISSIVE_EMAIL_AUTOLINKS = 0x8;   /* Recognize e-mails as autolinks even without '<', '>' and 'mailto:' */

	const int NO_INDENTED_CODEBLOCKS = 0x10; /* Disable indented code blocks. (Only fenced code works.) */

	const int NO_HTML_BLOCKS = 0x20;  /* Disable raw HTML blocks. */

	const int NO_HTML_SPANS = 0x40;  /* Disable raw HTML (inline). */
 
	const int TABLES = 0x100; /* Enable tables extension. */

	const int STRIKE_THROUGH = 0x200; /* Enable strikethrough extension. */

	const int WWW_AUTOLINKS = 0x400; /* Links that just begin with 'www.' */

	const int TASK_LISTS = 0x800; /* Enable task list extension. */

 	const int LATEX_MATH_SPANS = 0x1000; /* Enable task list extension. */

 	const int WIKI_LINKS = 0x2000;  /* Enable wiki links extension. */

 	const int UNDERLINE = 0x4000; /* Enable underline extension (and disables '_' for normal emphasis). */

 	const int HARD_SOFT_BREAKS = 0x8000;  /* Force all soft breaks to act as hard breaks. */

 	const int PERMISSIVE_AUTOLINKS = 0x8 | 0x4 | 0x400;

 	const int NO_HTML = 0x20 | 0x40;

 	const int DIALECT_GITHUB = 0x8 | 0x4 | 0x400 | 0x100 | 0x200 | 0x800;
 	

	public function __construct(int $parse = 0, int $render = 0);

	public function setFlags(int $parse, int $render = 0) : void {}

	public function text(string $input) : string {}

	public function cblink(array $attr) : array {}

	public function setTagClass(string $tag, string $cvalue) : void {}
};