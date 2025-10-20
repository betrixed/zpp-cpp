#ifndef ParseContext_CPP
#define ParseContext_CPP

#ifndef WCC_XML_PARSE_H
#include "xmlparse.h"
#endif

void 
ParseContext::init()
{
	markupDepth = 0;
	elementDepth = 0;
	squareDepth = 0;
	parenDepth = 0;
}

ParseContext::ParseContext(CoreParser* cp, ParseContent* prev, const std::string_view& data )
{
	init();
	parser_ = cp;
	prior_ = prev;
	data_ = data;
	fit_.set(data_.data(), data_.size());
}

bool 
ParseContext::empty() {
	 return (fit_.empty());
}

bool 
ParseContext::peek(char32_t match)
{
	return (match == fit_.front());
}
int 
ParseContext::munchSpace()
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

void 
ParseContext::adjustMarkupDepth(int adjust)
{
	markupDepth += adjust;
}