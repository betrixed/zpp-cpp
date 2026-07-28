#ifndef CH_MACRO_H
#define CH_MACRO_H


/* Make the UTF-8 support the default. */
#if !defined MD4C_USE_ASCII && !defined MD4C_USE_UTF8 && !defined MD4C_USE_UTF16
    #define MD4C_USE_UTF8
#endif

/* Magic for making wide literals with MD4C_USE_UTF16. */
#ifdef _T
    #undef _T
#endif
#if defined MD4C_USE_UTF16
    #define _T(x)           L##x
#else
    #define _T(x)           x
#endif


/*****************
 ***  Helpers  ***
 *****************/



/* Character classification.
 * Note we assume ASCII compatibility of code points < 128 here. */
#define ISIN_(ch, ch_min, ch_max)       ((ch_min) <= (unsigned)(ch) && (unsigned)(ch) <= (ch_max))
#define ISANYOF_(ch, palette)           ((ch) != _T('\0')  &&  md_strchr((palette), (ch)) != NULL)
#define ISANYOF2_(ch, ch1, ch2)         ((ch) == (ch1) || (ch) == (ch2))
#define ISANYOF3_(ch, ch1, ch2, ch3)    ((ch) == (ch1) || (ch) == (ch2) || (ch) == (ch3))
#define ISASCII_(ch)                    ((unsigned)(ch) <= 127)
#define ISBLANK_(ch)                    (ISANYOF2_((ch), _T(' '), _T('\t')))
#define ISNEWLINE_(ch)                  (ISANYOF2_((ch), _T('\r'), _T('\n')))
#define ISWHITESPACE_(ch)               (ISBLANK_(ch) || ISANYOF2_((ch), _T('\v'), _T('\f')))
#define ISCNTRL_(ch)                    ((unsigned)(ch) <= 31 || (unsigned)(ch) == 127)
#define ISPUNCT_(ch)                    (ISIN_(ch, 33, 47) || ISIN_(ch, 58, 64) || ISIN_(ch, 91, 96) || ISIN_(ch, 123, 126))

#define ISUPPER_(ch)                    (ISIN_(ch, _T('A'), _T('Z')))

#define ISLOWER_(ch)                    (ISIN_(ch, _T('a'), _T('z')))

#define ISALPHA_(ch)                    (ISUPPER_(ch) || ISLOWER_(ch))
#define ISDIGIT_(ch)                    (ISIN_(ch, _T('0'), _T('9')))
#define ISXDIGIT_(ch)                   (ISDIGIT_(ch) || ISIN_(ch, _T('A'), _T('F')) || ISIN_(ch, _T('a'), _T('f')))
#define ISALNUM_(ch)                    (ISALPHA_(ch) || ISDIGIT_(ch))



#endif