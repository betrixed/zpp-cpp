#ifndef UCODE8_CPP
#define UCODE8_CPP
/** 
    For PHP
    Have to detect valid character, construct the unicode character number
    , and  buffer
    into character buffer during the process, with a terminal 0.
    Length of buffer also calculated from change in index
*/

#ifndef UCODE8_H
#include "ucode8.h"
#endif


const char32_t INVALID_CHAR = (char32_t) -1;
/**
 * char const*  cpt      pointer to first character to consume
 * unsigned int  slen    offset of EOS from cpt
 * char32_t*     uc      next unicode character number returned, or INVALID_CHAR
 * @return unsigned int  number of bytes consumed
 */


/** returns number of characters processed */
unsigned int
    ucode8Fwd(
	char const* cpt, 
	unsigned int slen, 
	char32_t& uc)
{            
    uc = INVALID_CHAR;
    if (!cpt || !slen) {
        return 0;
    }
    const unsigned char* pa = (unsigned char const*)(cpt);
    unsigned int k = 1;
    

    char32_t test = *pa++; // ==1
    
    if (test < 0x80)
    { //noop
    }
    else if (test < 0xC2)
    {
        test = INVALID_CHAR;
        k = 0; // got nowhere
    }
    else if ((test < 0xE0) && (k < slen))
    {   
        k++;
        test = ((test & 0x1F) << 6) + (((char32_t)*pa++) & 0x3F);//2
    }  
    else if ((test < 0xF0) && (k+1 < slen))
    {
        k += 2;
        test = ((test & 0x0F) << 6) + (((char32_t)*pa++) & 0x3F);//2
        test =  (test << 6) + (((char32_t)*pa++) & 0x3F);//3
    }
    else if ((test < 0xF5) && (k+2 < slen)) // reach to 140000
    {
        k += 3;
        test = ((test & 0x07) << 6) + (((char32_t)*pa++) & 0x3F);//2
        test = (test << 6) + (((char32_t)*pa++) & 0x3F);//3
        test = (test << 6) + (((char32_t)*pa++) & 0x3F);//4
        if (test > 0x10FFFF) {
            test = INVALID_CHAR;
            k = 0;
        }
    }
    uc = test;
    return k;
}

unsigned int u8bytes(char32_t d)
{
    if (d < 0x80)
    {
        return 1;
    }
    if (d < 0x800)
    {
        return 2;
    }
    else if (d < 0x10000)
    {
        return 3;
    }
    else if (d < 0x110000)
    {
        return 4;
    }
    return 0;
}

// return number of char put in result
unsigned int
utf32_str8(char32_t d, char (*result) [8])
{
    if (d < 0x80)
    {
        (*result)[0] = (char) d;
        return 1;
    }
    if (d < 0x800)
    {
        // encode in 11 bits, 2 bytes
        (*result)[1] = (d & 0x3F) | 0x80;
        d >>= 6;
        (*result)[0] = (char) (d | 0xC0);
        return 2;
    }
    else if (d < 0x10000)
    {
        // encode in 16 bits, 3 bytes
        (*result)[2] = (d & 0x3F) | 0x80;
        d >>= 6;
        (*result)[1] = (d & 0x3F) | 0x80;
        d >>= 6;
        (*result)[0] = (char) (d | 0xE0);
        return 3;
    }
    else if (d > 0x10FFFF)
    {
        // not in current unicode range?
        return 0;
    }
    else {
        // encode in 21 bits, 4 bytes
        (*result)[3] = (d & 0x3F) | 0x80;
        d >>= 6;
        (*result)[2] = (d & 0x3F) | 0x80;
        d >>= 6;
        (*result)[1] = (d & 0x3F) | 0x80;
        d >>= 6;
        (*result)[0] = (char) (d | 0xF0);
        return 4;
    }
}

typedef struct _ret_char_buf {
    char str[32];
    int  slen;
}
ret_char_buf;

/* convert string of hexadecimal characters to a
   single unicode character. Return length */
int  
hex_str8(const char *data, int slen, char (*result) [8])
{
    char buf[24];

    // copy to terminate
    for(int i = 0; i < slen; i++) {
        buf[i] = *data++;
    }
    buf[slen] = '\0';

    char32_t val = (char32_t) strtol(buf, 0, 16);
    
    //Php::out << val << " is " << ec8.result << std::endl;
    return utf32_str8(val, result);
}

#endif