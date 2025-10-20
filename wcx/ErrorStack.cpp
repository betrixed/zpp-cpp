#ifndef ErrorStack_CPP
#define ErrorStack_CPP

#ifndef WCC_XML_PARSE_H
#include "xmlparse.h"
#endif

ErrorStack::ErrorStack() : errorLevel_(0)
{
}

void 
ErrorStack::errorStatus(int level)
{
	if (level > errorLevel_)
		errorLevel_ = level;
}

void 
ErrorStack::clear()
{
	messages_.init();
	errorLevel_ = 0;
}

int 
ErrorStack::pushMsg(str_ptr msg, int level = 0)
{
	htab_rw hw(messages_);
	hw.push_back(msg);

	if (level > errorLevel_)
		errorLevel_ = level;
	return errorLevel_;
}

str_rc 
ErrorStack::toString()
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


str_rc //static
ParseError::getErrorCodeMsg(int code)
{
    switch(code)
    {
    case UNEXPECTED_END:
        return "Unexpected end to parse source";
    case TAG_FORMAT:
        return "Tag format error";
    case MISSING_QUOTE:
        return "Missing quote";
    case EXPECTED_ATTRIBUTE:
        return "Attribute value expected";
    case BAD_CHARACTER:
        return "Bad character value";
    case MISSING_SPACE:
        return "Missing space character";
    case DUPLICATE_ATTRIBUTE:
        return "Duplicate attribute";
    case ELEMENT_NESTING:
        return "Element nesting error";
    case CDATA_COMMENT:
        return "Expected CDATA or Comment";
    case BAD_ENTITY_REFERENCE:
        return "Expected entity reference";
    case MISSING_END_BRACKET:
        return "Missing end >";
    case EXPECTED_NAME:
        return "Expected name";
    case CONTEXT_STACK:
        return "Pop on empty context stack";
    default:
        break;
    }
    return "Unknown error code";
}
#endif