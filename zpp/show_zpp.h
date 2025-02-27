#ifndef SHOW_ZPP_H
#define SHOW_ZPP_H

namespace zpp {

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