#ifndef WCC_STR8_CPP
#define WCC_STR8_CPP

#ifndef WCC_STR8_H
#include "str8.h"
#endif

#ifndef UCODE8_H
#include "zpp/ucode8.h"
#endif

extern "C" {
  //#include <Zend/zend_attributes.h>
  #include <Zend/zend_interfaces.h>
  //#include <Zend/zend_iterators.h>
};

#ifndef WCC_STR8_ARGINFO
#define WCC_STR8_ARGINFO
extern "C" {
	#include "stub/str8_arginfo.h"
}
#endif


namespace wcc {

using namespace zpp;

Str8_mgr Str8::omg;

Str8Iterator::Str8Iterator() : current(0), next(0)
{
}

zend_object_iterator_funcs 
Str8Iterator::it_fntab_ = {
	it_dtor,
	it_valid,
	it_get_data,
	it_get_key,
	it_forward,
	it_rewind,
	it_invalidate,
	nullptr
}; 

void* Str8Iterator::operator new(size_t count) 
{
    //zend_printf("operator new for HmapIterator\n");
    void* p = emalloc(count);
    memset(p, 0, sizeof(zend_object_iterator));
    return p;
}

void Str8Iterator::operator delete(void* ptr) {
    // Let PHP to call efree(ptr):
}


zend_object_iterator* //static
Str8Iterator::create(zend_class_entry* ce, zval* zobj, int byref)
{
        
	Str8Iterator* iterator = new Str8Iterator();
	zend_object_iterator* baseit = &iterator->phpit;

	zend_iterator_init(baseit);
	
	iterator->s8 = Z_OBJ_P(zobj);
    iterator->next = 0;
    iterator->current = 0;
    
    ZVAL_NULL(&baseit->data);
	iterator->phpit.funcs = &it_fntab_;

	//Str8* cobj = zobj_toc<Str8>(s8);

	return baseit; 
}

void //static
Str8Iterator::it_dtor(zend_object_iterator *iter)
{
	//zend_object* obj = Z_OBJ_P(&iter->data);
	//obj_rc::try_decref(obj);
	Str8Iterator *iterator = psi(iter);      
	ZVAL_NULL(&iterator->phpit.data);
	(iterator->s8).init();
	iterator->next = 0;

}


zend_result //static
Str8Iterator::it_valid(zend_object_iterator *iter)
{
	Str8Iterator *iterator = psi(iter);

	return (iterator->next > iterator->current) ? SUCCESS : FAILURE;

}

zval * //static 
Str8Iterator::it_get_data(zend_object_iterator *iter)
{
	return &iter->data;
}

void //static
Str8Iterator::it_get_key(zend_object_iterator *iter, zval *key)
{
	Str8Iterator *iterator = psi(iter);
	ZVAL_LONG(key, iterator->current);
}

void //static
Str8Iterator::it_forward(zend_object_iterator *iter)
{
	Str8Iterator *iterator = psi(iter);

	//get the string
	Str8* s8ptr = zobj_toc<Str8>(iterator->s8);

	str_ptr zstr = s8ptr->zstr_;
	long next = iterator->next;
	long slen = zstr.size() - next;

	if (slen > 0) {
		char32_t value = INVALID_CHAR;
		long units = ucode8Fore(zstr.data() + next, slen, value);
		
		if (units > 0) {
			iterator->current = next;
			iterator->next += units;
			ZVAL_LONG(&iterator->phpit.data, value);
			return;
		}

	}
	ZVAL_NULL(&iterator->phpit.data);
	iterator->current = next;
	return;
}

void  //static
Str8Iterator::it_rewind(zend_object_iterator *iter)
{
	Str8Iterator *iterator = psi(iter);
	ZVAL_NULL(&iterator->phpit.data);
	iterator->current = 0;
	iterator->next = 0;
}

void  //static
Str8Iterator::it_invalidate(zend_object_iterator *iter)
{
	Str8Iterator* mem = psi(iter);
	ZVAL_NULL(&mem->phpit.data);
	mem->current = 0;
	mem->next = 0;
}

void 	 
Str8::construct(str_ptr s)
{
	zstr_ = s;
}

str_rc 
Str8::toString() const
{
	return zstr_;
}


str_rc // static
Str8::ucode8str(zend_long ucode)
{
	char ustr[8];

	int slen = utf32_str8(ucode, &ustr);
	return str_rc(&ustr[0], slen);
}

} // namespace wcc

using namespace wcc;
using namespace zpp;

PHP_METHOD(Str8, __construct)
{
	zarg_rd args(execute_data);

	str_ptr s;

	args.zstring(s, args.need(0));

	if (!args.throw_errors())
	{
		Str8* cobj = zval_toc<Str8>(ZEND_THIS);
		cobj->construct(s);
	}
}

/* Create a new iterator from instance. */
PHP_METHOD(Str8, getIterator)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	zend_create_internal_iterator_zval(return_value, ZEND_THIS);
}

/* {{{ Return the iterated string */
PHP_METHOD(Str8, __toString)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	Str8* cobj = zval_toc<Str8>(ZEND_THIS);
	str_rc zstr = cobj->zstr_;
	zstr.move_zv(return_value);
}

/* {{{ Return the iterated string */
PHP_METHOD(Str8, ucode8str)
{
	zarg_rd args(execute_data);

	zend_long value;

	args.zlong(value, args.need(0));

	if (!args.throw_errors())
	{
		str_rc zstr = Str8::ucode8str(value);
		zstr.move_zv(return_value);
	}
}

PHP_MINIT_FUNCTION(Wcc_Str8_reg)
{
	auto ce = register_class_Wcc_Str8();
	Str8::omg.classEntry(ce);
	return SUCCESS;
}


#endif