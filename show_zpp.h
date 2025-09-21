#ifndef SHOW_ZPP_H
#define SHOW_ZPP_H

/**
 * @file zpp/show_zpp.h 
 * @author Michael Rynn <michael.rynn.500@gmail.com>
 * @brief Debug dumpers for PHP types. Output details of PHP types to console for trace/debugging.
 * @copyright Copyright (c) 2025 Michael Rynn
 * @license Artistic License 2.0
 */

#ifndef STR_BUF_H
#include "str_buf.h"
#endif

namespace zpp {


class dump_info {
private:
	str_out& ss;
	size_t       total_;

	static str_out dumper_d;

	void object_property_dump(
		zend_property_info *prop_info, 
		zval *zv, 
		zend_ulong index, 
		zend_string *key, 
		int level,
		int refadj = 0);

	void array_sub(HashTable* ht, int level);
	
public:

	static bool run_state_;

	static void msg_dump(const char* msg, val_ptr val);

	static void dump(val_ptr val, int level = 0);

	void di_dump(val_ptr val, int level = 0, int refadj=0);
	void indent(int ct);
	void di_showmem(zval *m);
	void di_showstr(zend_string* p);
	void di_showarray(HashTable* ht, int refadj=0);
	void di_showdata(HashTable* ht, int level = 0);
	void di_showobj(zend_object* obj);
	void di_showref(zend_reference* ref);
	void di_show_resource(zval *r);
	void output(const char* msg);
	void endl();
	
	void show_properties(zend_object* zobj, HashTable* h, int level, int refadj=0);

	dump_info() : ss(dumper_d), total_(0) {}
	
	dump_info(const char* s);

	dump_info(str_buf& buf) : ss(buf), total_(0) {}
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