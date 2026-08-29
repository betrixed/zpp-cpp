#ifndef STR8_FWD_H
#define STR8_FWD_H


namespace wcx {

using namespace std;

typedef  std::vector<char32_t, alloc_phpreq<char32_t> >  CodeStack;

typedef  void (*ExceptionDg) (ParseError*);


class str8_fwd {
public:
	enum {LFilterOff, LFilterOn, NoLFilter};

protected:
	const char*     base_;
	size_t          blen_;

	const char* 	next_;
	size_t      	length_;

	CodeStack		back_;
	bool            empty_;

	char32_t		front_;
	uint			flen_;

	size_t				lineNumber_; // lines counted
	size_t				lineChar_;   // characters in line
	char32_t			lastChar_;   // for handling CR;LF
	int              	xmlVersion_; // 10, 11 ??
	
	int                 lfilter_; // Line end filter state
	bool                isEndOfLine_; // 

	ExceptionDg			exceptionDg_;

	static str_rc badCharMsg(char32_t c)
	{
		str_buf buf;

		buf.printf("bad character 0x%x [%c]\n", c, c);

		return buf.zstr();
	}	

	void reset()
	{
		back_.reserve(8);
		next_ = base_;
		length_ = blen_;
		empty_ = (blen_ == 0);
		front_ = 0;
		flen_ = 0;
		lfilter_ = NoLFilter;
		lastChar_ = 0;
		lineChar_ = 0;
		lineNumber_ = 0;
		isEndOfLine_= false;

	}

	void copy(const str8_fwd& c)
	{
		base_ = c.base_;
		blen_ = c.blen_;
		next_ = c.next_;
		length_ = c.length_;
		back_ = c.back_;
		empty_ = c.empty_;
		front_ = c.front_;
		flen_ = c.flen_;
		// for some reason, 
		// line end character filtering is important

		isEndOfLine_ = c.isEndOfLine_;
		lfilter_ = c.lfilter_;
		xmlVersion_ = c.xmlVersion_;
		lastChar_ = c.lastChar_;
		lineChar_ = c.lineChar_;
		lineNumber_ = c.lineNumber_;
		exceptionDg_ = c.exceptionDg_;
	}

	// Horrible line count and filter for "conformance"
	void filterEOL()
	{
		if (isEndOfLine_)
		{
			isEndOfLine_ = false;
			lineNumber_++;
			lineChar_ = 0;

			if (lastChar_ == 0x0D)
			{
				lastChar_ = 0;
				switch(front_)
				{
				case 0x0A:// (#xD #xA)  single #A for XML 1.0, skip this
					popFront();
					return;
				case 0x85: // (#xD #x85) single #A for XML 1.1, skip this
					if (xmlVersion_ > 10)
						popFront();
					return;
				case 0x2028: // put on the stack, as single #A for XML 1.1
					if (xmlVersion_ > 10)
						front_ = 0x0A;
					return;
				default:  // leave it as is.
					break;
				}
			}
		}

		switch(front_)
		{
		case 0x0D:
			lastChar_ = 0x0D;
			front_ = 0x0A;
		case 0x0A:
			isEndOfLine_ = true;
			break;
		case 0x0085:
		case 0x2028:
			if (xmlVersion_ > 10)
			{
				front_ = 0x0A;
				isEndOfLine_ = true;
			}
			else {
				lineChar_++;
			}
			break;
		default:
			auto c = front_;
			bool isSourceCharacter 
				= (c >= 0x20) && (c < 0x7F) ? true
				: (c < 0x20) ? ((c==0xA)||(c==0x9)||(c==0xD))
				: (c <= 0xD7FF) ? (xmlVersion_ < 11) || (c > 0x9F) || (c == 0x85)
				: ((c >= 0xE000) && (c <= 0xFFFD)) || ((c >= 0x10000) && (c <= 0x10FFFF));
					
			if (!isSourceCharacter)
			{
				uint severity = ParseError::fatal;
				// Check position for crappy check for conformance tests on invalid BOM characters.
				if (lineChar_ == 0 && lineNumber_ == 0)
					switch(front_)
					{
					case 0xFFFE:
					case 0xFEFF:
						severity = ParseError::error;
					default:
						break;
					}
				auto e = new ParseError(badCharMsg(front()),severity);
				if (exceptionDg_ != nullptr)
					exceptionDg_(e);
				else
					throw e;

			}
			lineChar_++;
			break;
		}
	}

public:
	str8_fwd(const char* data, size_t len) : base_(data), blen_(len)
	{
		exceptionDg_ = nullptr;
		reset();

	}

	str8_fwd(const str8_fwd& c)
	{
		copy(c);
	}
	str8_fwd() : base_(nullptr), blen_(0) 
	{
		exceptionDg_ = nullptr;
		reset();
	}

	void set(const char* data, size_t len)
	{
		base_ = data;
		blen_ = len;
		reset();
	}
	
	void setExceptionDg(ExceptionDg dg) {
		exceptionDg_ = dg;
	}

	size_t size() const { return length_; }
	const char* next() const { return next_; }


	void set_buffer(const char* data, size_t len)
	{
		reset();
		popFront();
	}

	const str8_fwd& operator=(const str8_fwd& c)
	{
		copy(c);
		return *this;
	}

	void pushFront(char32_t const* c, uint slen)
	{
		if (slen == 0)
			return;

        if (!empty_)
			back_.push_back(front_);
		while(slen-- > 1)
			back_.push_back(c[slen]);
		front_ = c[0];
		empty_ = false;
	}

	void pushFront(const char* c, uint slen)
	{
		if (slen == 0)
			return;

        if (!empty_)
        {
			back_.push_back(front_);
        }

        char32_t c32 = 0;
        uint     k = 0;
        CodeStack temp;
        do {
        	k = ucode8Fwd(c, slen, c32);
        	if (k) 
        	{
        		temp.push_back(c32);
        		c += k;
        		slen -= k;
        	}
        }
        while((k) && (slen > 0));

		if (temp.size())
		{
			for(auto it = temp.rbegin(); it != temp.rend(); it++)
			{
				back_.push_back(*it);
			}
			front_ = back_.back();
			back_.pop_back();
			empty_ = false;
		}
	}

	bool empty() const { return empty_; }

	void pushFront(char32_t c)
    {
        if (!empty_)
            back_.push_back(front_);
		else
			empty_ = false;
        front_ = c;
    }

	char32_t front()
	{
		return front_;
	}

	std::string_view vstr() const { return std::string_view(next_, length_); }

	void popFront()
	{
		if (back_.size())
		{
			front_ = back_.back();
			back_.pop_back();
			return;
		}
		if (length_ > 0)
		{
			uint k = ucode8Fwd(next_, length_, front_);
			if (!k)
			{
				empty_ = true;
				next_ = nullptr;
			}
		}
		if (lfilter_ != LFilterOn)
		{
			lineChar_++;
		}
		else {
			filterEOL();
		}
	}

	void lineFilterOn()
	{
		if ((lfilter_ != NoLFilter) || (empty_))
			return;
		lfilter_ = LFilterOn;
		if (lineChar_ != 0)
		{
			lineChar_ -= 1;
		}
		filterEOL();

	}

	void lineFilteroff()
	{
		if (lfilter_ != NoLFilter)
		{
			lfilter_ = LFilterOff;
		}
	}
	void lineNoFilter()
	{
		lfilter_ = NoLFilter;
	}

	void setXmlVersion(int value)
	{
		xmlVersion_ = value;
	}

};

}; //namespace

#endif