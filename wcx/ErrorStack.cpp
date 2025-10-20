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
