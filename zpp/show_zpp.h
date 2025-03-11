#ifndef SHOW_ZPP_H
#define SHOW_ZPP_H

#ifndef ZSTR_BUFFER_H
#include "zstr_buffer.h"
#endif

namespace zpp {

class dump_info {
private:
	

	zstr_output ss;
	int   indents_;

	zstr_mgr di_return();

	zstr_mgr di_dump(zval_user val, int level);
	void object_property_dump(
		zend_property_info *prop_info, 
		zval *zv, 
		zend_ulong index, 
		zend_string *key, 
		int level) ;

public:

	static bool run_state_;


	static zstr_mgr dump(zval_user val, int level = 0);

	void di_showmem(zval *m);
	void di_showstr(zend_string* p);
	void di_showarray(HashTable* ht, int refadj=0);
	void di_showdata(HashTable* ht);
	void di_showobj(zend_object* obj);
	void di_showref(zend_reference* ref);
	void di_show_resource(zval *r);
	void output();
	
	void show_properties(zend_object* zobj, HashTable* h, int level);

	dump_info()
	{
	}
	
	dump_info(const char* s)
	{
		ss << s;
	}
};



/*
	extern std::string di_showstr(const char* s, zend_string* p);
	extern std::string di_showobj(const char* s, zend_object *obj);
	extern std::string di_showarray(const char* s, HashTable *ht);
	extern std::string di_showmem(const char* s, zval *m);
	extern std::string di_showdata(const char* s, HashTable* ht);
*/
	extern void  showstr(const char* s, zend_string* p);
	extern void  showobj(const char* s, zend_object *obj);
	extern void  showarray(const char* s, HashTable *ht);
	extern void  showmem(const char* s, zval *m);
	extern void  showdata(const char* s, HashTable *m);
};

#endif