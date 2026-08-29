#ifndef UCODE8_H
#define UCODE8_H

#ifndef ALLOC_PHPREQ_H
#include "alloc_phpreq.h"
#endif

//#include <uchar.h>

//! return number of character units consumed, and unicode code value
unsigned int ucode8Fwd(
	char const*  cpt, 
	unsigned int slen, 
	char32_t& uc
	);

extern const char32_t INVALID_CHAR;

unsigned int utf32_str8(char32_t d, char (*result) [8]);

unsigned int u8bytes(char32_t d);

int hex_str8(const char *data, int slen, char (*result) [8]);

#endif
