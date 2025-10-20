#ifndef UCODE8_H
#define UCODE8_H

#ifndef ALLOC_PHPREQ_H
#include "alloc_phpreq.h"
#endif

//#include <uchar.h>

// not intended for heap allocation
class str8_char {
public:
	const  char* data_;
	size_t 	     length_;
	char32_t     ucode_;

	str8_char() 
		: data_(nullptr), length_(0), ucode_(0) 
	{ 
	}
	str8_char(const str8_char& c) 
		: data_(c.data_), length_(c.length_), ucode_(c.ucode_)
	{

	}

	const str8_char& operator=(const str8_char& c)
	{
		data_ = c.data_;
		length_ = c.length_;
		ucode_ = c.ucode_;
		return *this;
	}

	std::string_view vstr() const { return std::string_view(data_, length_); }
};

// not intended for heap allocation
class str8_fit {
protected:
	size_t foreward(char32_t& uc);
	const char* 	data_;
	size_t      	length_;

public:
	
 	
 	str8_char		front_;

	str8_fit(const char* data, size_t len) : data_(data), length_(len){}
	str8_fit(const str8_fit& c) : data_(c.data_), length_(c.length_) {}
	str8_fit() : data_(nullptr), length_(0) {}

	size_t size() const { return length_; }
	const char* data() const { return data_; }

	void set(const char* data, size_t len)
	{
		data_ = data;
		length_ = len;
		popFront();
	}

	const str8_fit& operator=(const str8_fit& c)
	{
		data_ = c.data_;
		length_ = c.length_;
		return *this;
	}

	char32_t front()
	{
		return front_.ucode_;
	}

	std::string_view vstr() const { return std::string_view(data_, length_); }

	void popFront()
	{
		if (length_ > 0)
		{
			front_.data_ = data_;
			front_.length_ = foreward(front_.ucode_);
		}
		else {
			front_.data_ = nullptr;
			front_.length_ = 0;
		}
	}

	bool empty()
	{
		return ((length_ == 0)&&(front_.length_ = 0));
	}
};

//! return number of character units consumed, and unicode code value
unsigned int ucode8Fore(
	char const*  cpt, 
	unsigned int slen, 
	char32_t& uc
	);

extern const char32_t INVALID_CHAR;

unsigned int utf32_str8(char32_t d, char (*result) [8]);
int  		 hex_str8(const char *data, int slen, char (*result) [8]);

#endif
