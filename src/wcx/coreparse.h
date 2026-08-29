#ifndef WCX_COREPARSE_H
#define WCX_COREPARSE_H

#ifndef WCX_XMLREAD_H
#include "xmlread.h"
#endif

namespace wcx {

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

class CoreParse : public XmlRead
{
public:

	typedef bool (CoreParse::*ItemFn)(ItemReturn& iret);

	bool docStarted_;
	bool hasDeclared_;
	bool isEntity_;
	bool isStandalone_;

	htab_rc 		charEntity_;
	ItemFn  		func_;
	
	str_rc          data_;

	void init();

	ParseError* getNotWellFormed(const char* s);

	CoreParse(str_ptr data);
	~CoreParse();

	bool initParse(ItemReturn& item);
	bool parseProcessInst(ItemReturn& ret, int spaceCt = 0);
};

};


#endif
