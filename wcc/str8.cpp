#ifndef WCC_STR8_CPP
#define WCC_STR8_CPP

#ifndef WCC_STR8_H
#include "str8.h"
#endif

namespace wcc {

using namespace zpp;

Str8Iterator::Str8Iterator() : current(0), next(0), ucode(-1)
{
}

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
	zend_object_iterator* baseit = &iterator->phpit
	zend_iterator_init(baseit);
	

	obj_ptr s8 = Z_OBJ_P(zobj);
	str_ptr zstr = (zobj_toc<Str8>(s8))->toString();
    iterator->s8 = s8;
    iterator->zstr = zstr;
    iterator->next = 0;
    iterator->current = 0;
    iterator->ucode = -1;
    
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
	zval_ptr_dtor(&iterator->phpit.data);
	// leave other memory freeing to zend.
}


zend_result //static
Str8Iterator::it_valid(zend_object_iterator *iter)
{
	Str8Iterator *iterator = psi(iter);
	zend_result result = iterator->walk.ok() ? SUCCESS : FAILURE;
	return result;

}

zval * //static 
Str8Iterator::it_get_data(zend_object_iterator *iter)
{
	Str8Iterator *iterator = psi(iter);
	return iterator->walk.value();
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
	str_ptr s8 = Z_STR_P(&iterator->phpit.data);

	long next = iterator->next;

	long slen = s8.size() - next;

	if (slen > 0) {
		char32_t value = INVALID_CHAR;
		long units = ucode8Fore(s8.data() + next, slen, &value);
		
		if ((units > 0) && (value != INVALID_CHAR)) {
			iterator->current = next;
			iterator->next = next + units;
			iterator->ucode = value;
			return 1;
		}

	}
	iterator->current = next;
	iterator->ucode = INVALID_CHAR;
	return 0;
}

void  //static
Str8Iterator::it_rewind(zend_object_iterator *iter)
{
	Str8Iterator *iterator = psi(iter);
	iterator->walk.rewind();
}

void  //static
Str8Iterator::it_invalidate(zend_object_iterator *iter)
{
	Str8Iterator* mem = psi(iter);
	
}

} // namespace wcc

#endif