#ifndef WCC_XMLPARSE_CPP
#define WCC_XMLPARSE_CPP

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
#include "zpp/ucode.h"
#endif

#include <string_view>

namespace wcx {

	using namespace zpp;

constexpr std::string_view kCDATAi = "<![CDATA[";
constexpr std::string_view kCOMMENTi = "<!--";

constexpr std::string_view kCDATAf = "[CDATA[";
constexpr std::string_view kDASH2f = "--";

constexpr std::string_view kCDATA_END = "]>";

constexpr std::string_view kVERSION = "version";
constexpr std::string_view kDOCTYPE = "DOCTYPE";

constexpr std::string_view kENCODING = "encoding";
constexpr std::string_view kSTANDALONE = "standalone";

constexpr std::string_view LT_ent = "lt";
constexpr std::string_view GT_ent = "gt";
constexpr std::string_view AMP_ent = "amp";
constexpr std::string_view QUOT_ent = "quot";
constexpr std::string_view APOS_ent = "apos";


enum ParserState {
	XML_DECLARATION,
	PROCESS_INSTRUCTION,
	DOC_TYPE,
	DTD_INTERNAL_SUBSET,
	COMMENT,
	TAG_START,
	TAG_END,
	EPILOG
};

class XMLP_init : public state_init {
public:
	str_intern LT_ent;
	str_intern GT_ent;
	str_intern AMP_ent;
	str_intern QUOT_ent;
	str_intern APOS_ent;

	void init() override;
};

void 
XMLP_init::init()
{
	 LT_ent = "lt";
	 GT_ent = "gt";
	 AMP_ent = "amp";
	 QUOT_ent = "quot";
	 APOS_ent = "apos";
};

XMLP_init XMLPi;

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
		STR_COMMENT,
	};

	int 	   itype_;
	str_rc     item_
	htab_rc    attributes_; 

	ItemReturn() : itype_(RET_NULL) 
	{}

	ItemReturn(int itype, str_ptr text, htab_ptr attr) 
		: itype_(itype), item_(text), attributes_(attr) 
	{}

	ItemReturn(int itype, str_ptr text)  : itype_(itype), item_(text) 
	{}

	ItemReturn(const ItemReturm& c ) 
		: itype_(c.itype_), item_(c.item_), attributes_(c.attributes_)
	{}

	ItemReturn(ItemReturm &&m)
	{
		attributes_ = std::move(m.attributes_);
		item_ = std::move(m.item_);
		itype_ = m.itype_;
		m.itype_ = RET_NULL;
	}

	void 
	set(int itype, str_ptr text)
	{
		itype_ = itype;
		item_ = text;
		attributes_.init();
	}
	
	void 
	set(int itype, str_ptr text, htab_ptr attr)
	{
		itype_ = itype;
		item_ = text;
		attributes_ = attr;
	}

	const ItemReturn& operator=(const ItemReturn&c)
	{
		if (this != &c)
		{
			itype_=c.itype_;
			item_ = c.item_;
			attributes_ = c.attributes_;
		}
	}
	const ItemReturn& operator=(ItemReturn&& m)
	{
		if (this != &m)
		{
			attributes_ = std::move(m.attributes_);
			item_ = std::move(m.item_);
			itype_ = m.itype_;
			m.itype_ = RET_NULL;
		}
	}
};


static void stdEntityInit(htab_rc& map)
{
	htab_rw hw(map);

	hw.set(XMLPi.LT_ent, (zend_long) '<');
	hw.set(XMLPi.GT_ent, (zend_long) '>');
	hw.set(XMLPi.AMP_ent, (zend_long) '&');
	hw.set(XMLPi.QUOT_ent, (zend_long) '\"');
	hw.set(XMLPi.APOS_ent, (zend_long) '\'');
}
class ParseContext;


class ParseError : public PHPAlloc {
public:
	str_rc msg_;
	ParseError(str_rc errormsg) : msg_(errormsg) {}
};

class CoreParser : public PHPAlloc {
public:

	typedef bool CoreParser::*ItemFn(ItemReturn& iret);

	bool docStarted_;
	bool hasDeclared_;
	bool isEntity_;
	bool isStandalone_;

	htab_rc 		charEntity_;
	ItemFn  		func_;
	ParseContext*   ctx_;
	str_rc          data_;

	void init()
	{
		docStarted_ = false;
		hasDeclared_ = false;
		isEntity_ = false;
		isStandalone_ = true;
		func_ = nullptr;
		ctx_ = nullptr;
		stdEntityInit(charEntity_);
	}

	ParseError* getNotWellFormed(const char* s)
	{
		return new ParseError(s);
	}

	CoreParser(str_ptr data)
	{
		init();
		data_ = data;
		ctx_ = new ParseContext(data, nullptr, data.vstr());
		func_ = &CoreParser::initParse;

	}

	~CoreParser()
	{
		if (ctx_)
		{
			delete ctx_;
		}
	}

	bool initParse(ItemReturn& item)
	{
		char32_t testchar;
		str_rc   xpiName;
		str_rc   content;
		int      spaceCt;

		while(!ctx_->empty())
		{
			spaceCt = ctx_->munchSpace();

			if (ctx_->empty())
				break;
			if (ctx_->peek('<'))
			{
				ctx_->adjustMarkupDepth(1);
				ctx_->fit_.popFront();
				ctx_->throwIfEmpty();

				testchar = ctx_->front();
				switch(testchar)
				{
				case '?':
					ctx_->popFront();
					return parseProcessInst(item, spaceCt);	
				case '!':
					ctx_->popFront();
					ctx_->throwIfEmpty();
					if (ctx_->match(DOCTYPE_d))
					{
						return parseDocType();
					}
					else if (ctx_->match(DASH2_d))
					{
						 return parseComment(item);
					}
					else
						throw getNotWellFormed("Illegal in prolog");
				default:
					if (isNameStartFunc(testchar))
					{
						func_ = &CoreParser::parseStartTag;
						return parseStartTag(item);
					}
					else
						throw getBadCharError(testchar);
					break;
				} // end switch


			} // end if
			// else?
		} // end while
		throw getNotWellFormed("bad xml");
		}
	}

};

class ErrorStack : public PHPAlloc {
public:
	htab_rc	messages_;
	int     errorLevel_;

	ErrorStack() : errorLevel_(0)
	{
	}

	void errorStatus(int level)
	{
		if (level > errorLevel_)
			errorLevel_ = level;
	}

	void clear()
	{
		messages_.init();
		errorLevel_ = 0;
	}

	int pushMsg(str_ptr msg, int level = 0)
	{
		htab_rw hw(messages_);
		hw.push_back(msg);

		if (level > errorLevel_)
			errorLevel_ = level;
		return errorLevel_;
	}

	str_rc toString()
	{
		str_buf buf;

		htab_walk msgs;

		auto value = msgs.value();

		for(msgs.start(messages_); msgs.ok(); msgs.next())
		{
			buf << value.zstr() << endl;
		}
		return buf.zstr();
	}

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

	void init()
	{
		markupDepth = 0;
		elementDepth = 0;
		squareDepth = 0;
		parenDepth = 0;
	}

	ParseContext(CoreParser* cp, ParseContent* prev, const std::string_view& data )
	{
			init();
			parser_ = cp;
			prior_ = prev;
			data_ = data;
			fit_.set(data_.data(), data_.size());
	}

	bool empty() {
		 return (fit_.empty());
	}

	bool peek(char32_t match)
	{
		return (match == fit_.front());
	}
	int munchSpace()
	{
		int   count = 0;
		const char32_t test;

		while(!fit_.empty())
		{
			test = fit_.front();
			if (isSpace(test))
			{
				count++;
				fit_.popFront();
			}
			else
				break;
		}
		return count;
	}

	void adjustMarkupDepth(int adjust)
	{
		markupDepth += adjust;
	}
};

#endif