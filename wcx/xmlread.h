#ifndef WCX_XMLREAD_H
#define WCX_XMLREAD_H

#ifndef WCX_XML_CHAR_H
#include "xmlchar.h"
#endif

#ifndef WCX_ERRORSTACK_H
#include "ErrorStack.h"
#endif

namespace wcx {

using namespace zpp;

enum XMLR_STATE {
	DECLARE,
	PROCESS_INST,
	DOC_TYPE,
	DTD_INTERNAL,
	COMMENT,
	TAG_START,
	TAG_END,
	EPILOG,
};

class ParseContext;

class XmlRead : public PHPAlloc {
public:

	CharTestFn		isNameStartFn_;
    CharTestFn		isNameFn_;

	bool			validate_;
	double			docXmlVersion_;
	str_rc			versionStr_;
protected:
	XMLR_STATE		rstate_;
	ParseContext*   ctx_;
	
	virtual void reportInvalid();
	virtual void setXmlVersion(str_ptr vstr);

	void setState(XMLR_STATE st)
	{
		rstate_ = st;
	}

	XMLR_STATE getState() const
	{
		return rstate_;
	}

	XmlRead();

	ExceptionMsg* getNotWellFormed(const char* s);

	ExceptionMsg* getNotWellFormed(str_ptr s);

	ExceptionMsg* getParseError(str_ptr s);

	ExceptionMsg* getUnexpectedEnd();

	ExceptionMsg* getBadCharError(char32_t c);

};


};//namespace
#endif