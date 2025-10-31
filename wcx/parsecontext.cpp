#ifndef ParseContext_CPP
#define ParseContext_CPP

#ifndef WCX_XML_PARSE_H
#include "xmlparse.h"
#endif

#ifndef WCX_PARSECONTEXT_H
#include "parsecontext.h"
#endif

namespace wcx {

void 
ParseContext::init()
{
	markupDepth = 0;
	elementDepth = 0;
	squareDepth = 0;
	parenDepth = 0;
}


ParseContext::ParseContext(str8_fwd& fit, CoreParse* cp, 
		ParseContext* prev, bool scoped) 
	: fit_(fit), scoped_(scoped), parser_(cp), prior_(prev)
{
	init();
}

ParseContext::~ParseContext()
{

}

bool ParseContext::isNameStart5thEd(char32_t test)
{
	if (xmlVersion_ == 10 && maxEdition_ >= 5)
	{
		if (!isNameStart11(test))
			return false;

		if (validate_)
		{
			estack_->pushMsg(
				"Name start character only specified by XML 1.0 fifth edition", 
				ParseError::invalid);
			reportInvalid();
		}
		return true;
	}
	return false;
}

bool ParseContext::getXmlName(str_rc& tag)
{
	if (empty())
		return false;
	char32_t test = fit_.front();
	if ( !(isNameStartFn_(test) || isNameStartFifthEdition(test)) )
		return false;
	
	frontFilterOff();
	scratch_.length(0);
	scratch_.put(front());

	popFront();
	while (!empty())
	{
		if (isNameCharFn(front()) || isNameCharFifthEdition(front()))
		{
			scratch_.put(front());
			popFront();
		}
		else
			break;
	}
	tag = scratch_;
	frontFilterOn();
    return true;
}

int 
ParseContext::munchSpace()
{
	int   count = 0;
	char32_t test = 0;

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

void 
ParseContext::adjustMarkupDepth(int adjust)
{
	markupDepth += adjust;
}

void ParseContext::throwIfEmpty()
{
	if (fit_.empty())
		throw parser_->getNotWellFormed("Incomplete xml");
}

bool ParseContext::empty() {
		return (fit_.empty());
	}

bool ParseContext::peek(char32_t match){
	return (match == fit_.front());
}

char32_t ParseContext::front() {
	return fit_.front();
}

void ParseContext::popFront(){
	fit_.popFront();
}
	

bool ParseContext::getXmlName(DOMString& wr)
{
	char32_t test;
	if (!fit_.peek(test))
		return false;
	const CharTestFn isNameFn = rdr_->isNameCharFunc;
	if (!isNameFn(test))
		return false;

	DOMString name;
	name.put(test);
	rd_.popFront();
	while (rd_.peek(test))
	{
		if (isNameFn(test))
		{
			name.put(test);
			rd_.popFront();
		}
		else {
			wr = name;
			return true;
			break;
		}
	}
	throwIfEmpty();
	return false;
}
    
} //namespace

#endif