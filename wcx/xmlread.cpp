#ifndef WCX_XMLREAD_CPP
#define WCX_XMLREAD_CPP

#ifndef WCX_XMLREAD_H
#include "xmlread.h"
#endif

namespace wcx {
	XmlRead::XmlRead()
	{
		validate_ = false;
		docXmlVersion_ = 10;
		versionStr_ = "1.0";
		isNameStartFunc = &isNameStart10;
		isNameCharFunc = &isName10;
		rstate_ = XMLR_STATE::DECLARE;
		ctx_ = nullptr;
	}

	ExceptionMsg* 
	XmlRead::getNotWellFormed(const char* s)
	{
		return new ExceptionMsg(s);
	}
	ExceptionMsg* 
	XmlRead::getNotWellFormed(WString& s)
	{
		return new ExceptionMsg(convertString<WString,String>(s));
	}

	ExceptionMsg* 
	XmlRead::getNotWellFormed(String& s)
	{
		return new ExceptionMsg(s);
	}

	ExceptionMsg* 
	XmlRead::getParseError(String& s)
	{
		return new ExceptionMsg(s);
	}

	ExceptionMsg* 
	XmlRead::getUnexpectedEnd()
	{
		return new ExceptionMsg("Unexpected end");
	}

	ExceptionMsg* 
	XmlRead::getBadCharError(char32_t c)
	{
		String msg("Bad character 0x");
		cat(msg,c,16);
		return new ExceptionMsg(msg);
	}

	virtual void reportInvalid() {} // override

	void setXmlVersion(str_ptr vstr)
	{
		versionStr_ = vstr;
		docXmlVersion_ = vstr.zdouble();
	}

};

#endif
