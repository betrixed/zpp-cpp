#ifndef WCX_ERRORSTACK_H
#define WCX_ERRORSTACK_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef ALLOC_PHPREQ_H
#include "zpp/alloc_phpreq.h"
#endif

namespace wcx {

using namespace zpp;

enum ParseErrorCode {
    UNEXPECTED_END,
    TAG_FORMAT,
    MISSING_QUOTE,
    EXPECTED_ATTRIBUTE,
    BAD_CHARACTER,
    MISSING_SPACE,
    DUPLICATE_ATTRIBUTE,
    ELEMENT_NESTING,
    CDATA_COMMENT,
    BAD_ENTITY_REFERENCE,
    MISSING_END_BRACKET,
    EXPECTED_NAME,
    CONTEXT_STACK,
};


class ExceptionMsg : public PHPAlloc {
protected:
	str_rc msg_;
public:
	ExceptionMsg(str_rc s) : msg_(s)
	{
	}

	ExceptionMsg(const char* s) : msg_(s)
	{
	}

	ExceptionMsg(const ExceptionMsg& e)
	{
		msg_ = e.msg_;
	}

	const ExceptionMsg& operator=(const ExceptionMsg& e) noexcept
	{
		msg_ = e.msg_;
		return *this;
	}

	str_ptr msg() const { return msg_; }
	virtual const char* what() { return msg_.data(); }
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


class ParseError : public ExceptionMsg {
protected:
	uint severity_;
    int code_;
public:

	enum { noError, invalid, error, fatal };

	static str_rc getErrorCodeMsg(int code);

	inline uint severity() const { return severity_; }
	inline int code() const { return code_; }

	ParseError(str_ptr msg, uint level)
		:  ExceptionMsg(msg),
		severity_(level), code_(0)
	{
	}

	ParseError(int code, uint level) : 
		ExceptionMsg(getErrorCodeMsg(code)),
		severity_(level), code_(code)
	{
	}

	ParseError(const ParseError& e) : ExceptionMsg(e.msg_)
	{
		severity_ = e.severity_;
		code_ = e.code_;
	}

	const ParseError& operator=(const ParseError& e) noexcept
	{
		msg_ = e.msg_;
		severity_ = e.severity_;
		code_ = e.code_;
		return *this;
	}

	
};
} // namespace wcx

#endif