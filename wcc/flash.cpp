#ifndef SESSION_FLASH_CPP
#define SESSION_FLASH_CPP

namespace wcc {

using namespace zpp;

FlashInit FLi;

void FlashInit::init()
{
	lines_str = "lines";
	text_str = "text";
	status_str = "status";
}

void 
Flash::construct(htab_ptr data)
{
	obj_ptr self(self_);
	lines_ = self.property_ptr(FLi.lines_str);
}


bool 
Flash::hasData()
{
	return (lines_.size() == 0);
}

void 
Flash::add(str_ptr text, str_ptr status)
{
	
	htab_rc line;
	htab_rw writer(line);

	writer.set(FLi.text_tr, text);
	writer.set(FLi.status, status);

	htab_rw lines(lines_);

	lines.push_back(line);
}


htab_rc 
Flash::getData()
{
	return htab_rc(lines_.zarray());
}


void 
Flash::clear()
{
	htab_rw writer( lines_);
	writer.reset();
}


};//namespace wcc

#endif