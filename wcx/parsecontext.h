#ifndef WCX_PARSECONTEXT_H
#define WCX_PARSECONTEXT_H

namespace wcx {

using namespace zpp;


class ParseContext : public PHPAlloc { 
public:
	str8_fwd&      	fit_;

	ParseContext* 	prior_;
	CoreParse*   	parser_;
	ErrorStack*    	estack_;



	bool						scoped_;
    int							markupDepth;  //  count < and >
	int							elementDepth; // tag nesting
	int							squareDepth;  // count [ [ and ] ]
	int							parenDepth;		// ( and )

	ParseContext(str8_fwd& fit, CoreParse* cp, 
		ParseContext* prev, bool scoped = false);

	ParseContext(str8_fwd& fit);

	~ParseContext();

	void adjustMarkupDepth(int adjust);
	void init();
	int  munchSpace();
	
	void throwIfEmpty();

	bool empty();

	bool peek(char32_t match);

	char32_t front();

	bool isNameStart5thEd(char32_t test);

	void popFront();

	void pushFront(const char* sdata, size_t slen);

	bool getXmlName(str_rc& xpiName);
};

} //namespace wcx

#endif