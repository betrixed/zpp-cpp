#ifndef WCC_XML_PARSE_H
#define WCC_XML_PARSE_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef ALLOC_PHPREQ_H
#include "zpp/alloc_phpreq.h"
#endif

#ifndef WCX_XML_CHAR_H
#include "xmlchar.hpp"
#endif

#ifndef UCODE8_H
#include "zpp/ucode8.h"
#endif

class ParseContext;

class ItemReturn {
public:
	enum {
		RET_NULL,
		TAG_START, // tag start with inner content
		TAG_END,   // tag end of inner content
		TAG_EMPTY, // tag without inner content
		XML_DEC,   // attributes of xml declaration
		STR_TEXT,
		STR_CDATA,
		STR_PI,
		STR_XI,
		STR_COMMENT
	};

	int 	   itype_;
	str_rc     item_;
	htab_rc    attributes_; 

	ItemReturn();
	ItemReturn(int itype, str_ptr text, htab_ptr attr);
	ItemReturn(int itype, str_ptr text);

	ItemReturn(const ItemReturn& c );

	ItemReturn(ItemReturn &&m);

	void  set(int itype, str_ptr text);
	
	void  set(int itype, str_ptr text, htab_ptr attr);

	const ItemReturn& operator=(const ItemReturn&c);

	ItemReturn& operator=(ItemReturn&& m);
};

class ParseError : public PHPAlloc {
public:
	str_rc msg_;
	ParseError(str_rc errormsg) : msg_(errormsg) {}
};

class CoreParser : public PHPAlloc {
public:

	typedef bool (CoreParser::*ItemFn)(ItemReturn& iret);

	bool docStarted_;
	bool hasDeclared_;
	bool isEntity_;
	bool isStandalone_;

	htab_rc 		charEntity_;
	ItemFn  		func_;
	ParseContext*   ctx_;
	str_rc          data_;

	void init();

	ParseError* getNotWellFormed(const char* s);

	CoreParser(str_ptr data);
	~CoreParser();

	bool initParse(ItemReturn& item);
	
};

class ErrorStack : public PHPAlloc {
public:
	htab_rc	messages_;
	int     errorLevel_;

	ErrorStack();

	void clear();
	void errorStatus(int level);
	int  pushMsg(str_ptr msg, int level = 0);

	str_rc toString();
};

class ParseContext : public PHPAlloc { 
public:
	ParseContext* prior_;
	CoreParser*   parser_;
	str8_fit      fit_;
	std::string_view  data_;
	ErrorStack    estack_;

	bool						scoped_;
    int							markupDepth;  //  count < and >
	int							elementDepth; // tag nesting
	int							squareDepth;  // count [ [ and ] ]
	int							parenDepth;		// ( and )

	ParseContext(CoreParser* cp, 
		ParseContent* prev, 
		const std::string_view& data );

	void adjustMarkupDepth(int adjust);
	bool empty();
	void init();
	int  munchSpace();
	bool peek(char32_t match);
};


#endif

