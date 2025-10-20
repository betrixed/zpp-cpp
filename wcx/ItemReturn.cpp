#ifndef ErrorStack_CPP
#define ErrorStack_CPP

#ifndef WCC_XML_PARSE_H
#include "xmlparse.h"
#endif

namespace wcx {


ItemReturn::ItemReturn() : itype_(RET_NULL) 
{}

ItemReturn::ItemReturn(int itype, str_ptr text, htab_ptr attr) 
	: itype_(itype), item_(text), attributes_(attr) 
{}

ItemReturn::ItemReturn(int itype, str_ptr text)  : itype_(itype), item_(text) 
{}

ItemReturn::ItemReturn(const ItemReturn& c ) 
	: itype_(c.itype_), item_(c.item_), attributes_(c.attributes_)
{}

ItemReturn::ItemReturn(ItemReturn &&m)
{
	attributes_ = std::move(m.attributes_);
	item_ = std::move(m.item_);
	itype_ = m.itype_;
	m.itype_ = RET_NULL;
}

void 
ItemReturn::set(int itype, str_ptr text)
{
	itype_ = itype;
	item_ = text;
	attributes_.init();
}

void 
ItemReturn::set(int itype, str_ptr text, htab_ptr attr)
{
	itype_ = itype;
	item_ = text;
	attributes_ = attr;
}

const ItemReturn& 
ItemReturn::operator=(const ItemReturn&c)
{
	if (this != &c)
	{
		itype_=c.itype_;
		item_ = c.item_;
		attributes_ = c.attributes_;
	}
	return *this;
}

ItemReturn& 
ItemReturn::operator=(ItemReturn&& m)
{
	if (this != &m)
	{
		attributes_ = std::move(m.attributes_);
		item_ = std::move(m.item_);
		itype_ = m.itype_;
		m.itype_ = RET_NULL;
	}
	return *this;
}

} // namespace wcx


#endif