#ifndef WCC_REPLACE_CPP
#define WCC_REPLACE_CPP

#ifndef WCC_REPLACE_H
#include "replace.h"
#endif

#ifndef WCC_REPLACE_ARGINFO
#define WCC_REPLACE_ARGINFO
extern "C" {
	#include "stub/replace_arginfo.h"
};

#endif

namespace wcc {
	
base_obj_mgr<Replace> Replace::omg;

Replace_init REPi;

void
Replace_init::init()
{
	prop_expr = R"(#@([a-zA-Z][\w\d]*)#)";
}

Replace::Replace(obj_ptr obj, str_ptr exp) 
	: base_d(), expr_(exp, preg::OFFSET_CAPTURE, true)
	, src_(obj)
{
	if (!exp.size())
	{
		expr_.setExpr(REPi.prop_expr);
	}
}

Replace::Replace() : base_d(), expr_()
{
}

void 
Replace::construct(obj_ptr obj, str_ptr rexpr)
{
	expr_.init(rexpr, preg::OFFSET_CAPTURE, true);
	src_ = obj;
}

str_rc //static
Replace::property(obj_ptr obj, str_ptr data)
{
	Replace temp(obj, REPi.prop_expr);

	return temp.eval(data);
}

str_rc 
Replace::eval(str_ptr subj)
{
	int ct = expr_.matches(subj);
	if (ct > 0) {
		htab_ptr m = expr_.results();

		htab_ptr replace_list = m.get((int)0);
		htab_ptr keys_list = m.get(1);

		std::string_view original = subj.vstr();

		str_buf result;
		size_t ipos = 0;

		for(int i = 0; i < ct; i++)
		{
			htab_ptr  k1 = keys_list.get(i);
			val_ptr fkey = k1.get((int)0);
			//showmem("get key", fkey);
			
			val_rc rval = src_.property(fkey);
			
			//showmem("replace value", rval);
			str_ptr replace_str = val_ptr(rval).zstr();

			htab_ptr f1 = replace_list.get(i);
			str_ptr  slen_f1 = f1.get((int)0);
			val_ptr  soffset_f1 = f1.get(1);

			size_t slen = slen_f1.size();
			zend_long soffset = soffset_f1.zlong();

			if (!replace_str)
			{
				result << original.substr(ipos, soffset-ipos);
			} 
			else {
				result << original.substr(ipos, soffset-ipos);
				result << replace_str;
			}
			ipos = soffset + slen;
		}
		if (ipos < original.size()) {
			result << original.substr(ipos);
		}
		return result.zstr();
	}
	else {
		return str_rc(subj);
	}
}

};

using namespace wcc;
using namespace zpp;


/*public function __construct(object $obj, ?string $rexpr = null);*/
ZEND_METHOD(Wcc_Replace, __construct)
{
	zarg_rd args(execute_data);

	obj_ptr obj;
	str_ptr rexpr;

	args.obj(obj, args.need(0));
	args.zstring_null(rexpr, args.option(1));
	if (!args.throw_errors())
	{
		Replace* cobj = zval_toc<Replace>(ZEND_THIS);
		cobj->construct(obj, rexpr);
	}
}

/*public function eval(string $subj): string {}*/

ZEND_METHOD(Wcc_Replace, eval)
{
	zarg_rd args(execute_data);

	str_ptr subj;

	args.zstring(subj, args.need(0));
	if (!args.throw_errors())
	{
		Replace* cobj = zval_toc<Replace>(ZEND_THIS);
		str_rc value = cobj->eval(subj);
		value.move_zv(return_value);
	}
}

/*static public function property(object $obj, string $data) : string {}*/
ZEND_METHOD(Wcc_Replace, property) 
{
	zarg_rd args(execute_data);
	obj_ptr obj;
	str_ptr subj;

	if (args.obj(obj, args.need(0)))
	{
		args.zstring(subj, args.need(1));
	}
	if (!args.throw_errors())
	{
		str_rc value = Replace::property(obj, subj);
		value.move_zv(return_value);
	}
}

PHP_MINIT_FUNCTION(wcc_replace_reg)
{
	Replace::omg.classEntry(register_class_Wcc_Replace());

	return SUCCESS;

}


#endif
