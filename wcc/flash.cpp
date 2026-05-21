#ifndef SESSION_FLASH_CPP
#define SESSION_FLASH_CPP

#ifndef SESSION_FLASH_H
#include "flash.h"
#endif

#ifndef USERDATA_ARGINFO_H
extern "C" {
	#include "stub/userdata_arginfo.h"
}
#endif
namespace wcc {

using namespace zpp;

base_obj_mgr<Flash> Flash::omg;

UserDataInit UDi;

void UserDataInit::init()
{
	lines_str = "lines";
	text_str = "text";
	status_str = "status";

	userName_p = "userName";
	roles_p = "roles";
	email_p = "email";
	id_p = "id";
	memberid_p = "memberid";
	status_p = "status";
	keys_p = "keys";
}

void 
Flash::construct(htab_ptr data)
{
	obj_ptr self(self_);
	lines_ = self.property_ptr(UDi.lines_str);
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

	writer.set(UDi.text_str, text);
	writer.set(UDi.status_str, status);

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
	lines_.init();
}


};//namespace wcc

using namespace wcc;
using namespace zpp;


ZEND_METHOD(Wcc_Session_Flash, __construct)
{
	zarg_rd args(execute_data);
	htab_ptr data = args.htab(args.need(0));

	if (!args.throw_errors())
	{
		Flash* cobj = zval_toc<Flash>(ZEND_THIS);
		cobj->construct(data);
	}
}

ZEND_METHOD(Wcc_Session_Flash, hasData)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Flash* cobj = zval_toc<Flash>(ZEND_THIS);
	bool value = cobj->hasData();
	RETURN_BOOL(value);

}

ZEND_METHOD(Wcc_Session_Flash, add)
{
	zarg_rd args(execute_data);
	str_ptr text = args.str(args.need(0));
	str_ptr status = args.str(args.need(1));

	if (!args.throw_errors())
	{
		Flash* cobj = zval_toc<Flash>(ZEND_THIS);
		cobj->add(text, status);
	}
}

ZEND_METHOD(Wcc_Session_Flash, getData)
{
	zarg_rd args(execute_data);

	if (!args.throw_errors())
	{
		Flash* cobj = zval_toc<Flash>(ZEND_THIS);

		htab_rc result = cobj->getData();
		result.move_zv(return_value);
	}
}

ZEND_METHOD(Wcc_Session_Flash, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Flash* cobj = zval_toc<Flash>(ZEND_THIS);

	cobj->clear();
}

PHP_MINIT_FUNCTION(Session_UserData_reg)
{
	auto ce = register_class_Wcc_Session_Flash();

	Flash::omg.classEntry(ce);

	STATE_INIT_ADD(UDi)
	
	return SUCCESS;
}
#endif