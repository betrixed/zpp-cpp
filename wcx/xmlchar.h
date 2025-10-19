#ifndef WCX_XML_CHAR_H
#define WCX_XML_CHAR_H

namespace wcx {

typedef bool (*CharTestFn)(char32_t c);

/// each character matches pattern "a-zA-Z0-9_.-"
/*
template<typename T>
bool isAsciiName(T const* s, uint slen)
{
    for(uint i = 0; i < slen; i++)
    {
	   const T d = s[i];
       if ((d >= 'a' && d <= 'z') || (d >= 'A' && d <= 'Z') || (d >= '0' && d <= '9')
		   || (d=='_') || (d=='.') || (d=='-'))
			continue;
	   return false;
    }
	 return true;
}
*/


extern bool isAsciiName(char const* s, uint slen);

extern bool in_pairs_table(char32_t const* table, uint zend, char32_t c);

extern bool isSpace(char32_t);
extern bool isLetter(char32_t c);
extern bool isDigit(char32_t c);
extern bool isControl11(char32_t c);

extern bool isBase(char32_t c);
extern bool isCombining(char32_t c);
extern bool isExtender(char32_t c);
extern bool isIdeographic(char32_t c);
extern bool isAlphabet(char32_t c);

extern bool isChar10(char32_t);
extern bool isChar11(char32_t);

extern bool isNameStart10(char32_t c);
extern bool isName10(char32_t c);

extern bool isNameStart11(char32_t c);
extern bool isName11(char32_t c);

}; // end namespace wcx

#endif

