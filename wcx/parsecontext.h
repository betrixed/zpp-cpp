#ifndef WCX_PARSECONTEXT_H
#define WCX_PARSECONTEXT_H

namespace wcx {

using namespace zpp;


class ParseContext : public PHPAlloc { 
public:
	ParseContext* 	prior_;
	CoreParser*   	parser_;
	str8_fwd      	fit_;
	std::string_view  data_;
	ErrorStack    	estack_;
	CharTestFn      isNameFn_;
	CharTestFn		isNameStartFn_;
	int             xmlVersion_;
	int             maxEdition_;
	bool            validate_;

	bool						scoped_;
    int							markupDepth;  //  count < and >
	int							elementDepth; // tag nesting
	int							squareDepth;  // count [ [ and ] ]
	int							parenDepth;		// ( and )

	ParseContext(CoreParser* cp, 
		ParseContext* prev, 
		const std::string_view& data );

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