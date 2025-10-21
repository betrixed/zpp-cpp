#ifndef CoreParser_CPP
#define CoreParser_CPP

#ifndef WCC_XML_PARSE_H
#include "xmlparse.h"
#endif

namespace wcx {

static void stdEntityInit(htab_rc& map)
{
	htab_rw hw(map);

	hw.set(XMLPi.LT_ent, (int) '<');
	hw.set(XMLPi.GT_ent, (int) '>');
	hw.set(XMLPi.AMP_ent, (int) '&');
	hw.set(XMLPi.QUOT_ent, (int) '\"');
	hw.set(XMLPi.APOS_ent, (int) '\'');
}

CoreParser::CoreParser(str_ptr data)
{
	init();
	data_ = data;
	ctx_ = new ParseContext(this, (ParseContext*)nullptr, data.vstr());
}

void CoreParser::init()
{
	docStarted_ = false;
	hasDeclared_ = false;
	isEntity_ = false;
	isStandalone_ = true;
	func_ = &CoreParser::initParse;
	ctx_ = nullptr;
	stdEntityInit(charEntity_);
}

ParseError* 
CoreParser::getNotWellFormed(const char* s)
{
	str_rc msg(s);
	return new ParseError(msg, ParseError::fatal);
}

CoreParser::~CoreParser()
{
	if (ctx_)
	{
		delete ctx_;
	}
}


bool 
CoreParser::initParse(ItemReturn& item)
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
			ctx_->popFront();
			ctx_->throwIfEmpty();

			testchar = ctx_->front();
			switch(testchar)
			{
			case '?':
				ctx_->popFront();
				return parseProcessInst(item, spaceCt);	
			case '!':
				ctx_->fit_.popFront();
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

bool 
CoreParser::parseProcessInst(ItemReturn& ret, int spaceCt)
{
	str_rc xpiName;
	if (!ctx_->getXmlName(xpiName))
		throw getNotWellFormed("Bad process instruction name");
	if (xpiName == "xml")
	{
		if (docStarted || spaceCt > 0)
			throw getNotWellFormed("xml declaration should be first");
		if (!hasDeclaration)
		{
			hasDeclaration = true;
			return parseDeclaration(ret);
		}
		else
			throw getNotWellFormed("Duplicate xml declaration");
	}
	DOMString content;
	ctx_->parsePI(content);
	DOMString_HM  extra;
	extra.putR(DOMString(), content);
	ret.set(ItemReturn::STR_PI,xpiName,extra);
	return true;
}
}; // namespace wcx
#endif