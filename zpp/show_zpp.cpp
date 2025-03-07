#ifndef SHOW_ZPP_CPP
#define SHOW_ZPP_CPP

// Debug dumpers

#include <string>
#include <sstream>
#include <iomanip>

namespace zpp {

#ifdef DEBUG_XTRA
class dump_info {
public:

	static bool run_state_;
	std::stringstream ss;
	int   indents_;

	dump_info(const char *s)
	{
		ss << s << " ";
		indents_ = 0;
	}

	void di_showmem(zval *m) 
	{
		for (int i = 0; i < indents_; i++)
		{
			ss << "  ";
		}
		ss << "zval " << m << " ";

		int  ztype = Z_TYPE_P(m);

		if (ztype == IS_STRING) {
			di_showstr(Z_STR_P(m));
			return;
		}
		if (ztype == IS_ARRAY) {
			di_showarray(Z_ARR_P(m));
			return;
		}
		else if (ztype == IS_OBJECT) 
		{
			di_showobj(Z_OBJ_P(m));
			return;
		}
		if (ztype == IS_LONG) 
		{
			ss << "is long = " << Z_LVAL_P(m);
		}
		else if (ztype == IS_NULL) {
			ss << "is null";
		}
		else  if (ztype == IS_TRUE) {
			ss << "++ TRUE ++ ";
		}
		else  if (ztype == IS_FALSE) {
			ss  << "** FALSE ** ";
		}
		else if (ztype == IS_INDIRECT) 
		{
			ss  << "--> INDIRECT ";
		}
		else if (ztype == IS_REFERENCE)
		{
			di_showref(Z_REF_P(m));
		}
		else
		{
			ss << "type # " << ztype;
		}

		return;
	}
	void di_showstr(zend_string* p) 
	{
		if (!p) {
			ss << "p = nullptr";
		}
		else {
		   const char* data = ZSTR_VAL(p);
		   size_t   slen = ZSTR_LEN(p);

			ss << " str(" << std::dec << slen << ") " << std::setw(4) << std::hex << p;
			ss << " gc " << GC_REFCOUNT(p) << " ";
			int gcflags = GC_FLAGS(p);
			if ((gcflags & GC_PERSISTENT) != 0) ss << "ps ";
			if ((gcflags & IS_STR_INTERNED) != 0) ss << "in ";
			ss << " : " << ZSTR_VAL(p);

			if (data[slen] == '\0') 
			{
				ss << " |";
			}
			else {
				ss <<" *";
			}
		}
	}

	void di_showarray(HashTable* ht)
	{

		if (!ht) {
			ss << "is nullptr" << std::endl;
			return;
		}

		size_t ct = zend_array_count(ht);
		if (ht->gc.u.type_info & GC_IMMUTABLE) {
			ss << "immutable ";
		}
		ss << std::dec << "arr(" << ct <<") " << std::setw(4) << std::hex << ht;
		/* if (ht->u.flags & htab_mgr::COW_VIOLATE) {
			ss << " vcow ";
		}
		else {
			ss << " cow ";
		}
		*/
		ss << " gc " << GC_REFCOUNT(ht);

	}
	void di_showdata(HashTable* ht)
	{
		if (!ht) {
			ss << "is nullptr" << std::endl;
			return;
		}

		di_showarray(ht);

		htab_walk wk;

		ss << "--[ " << std::endl;
		indents_++;

		auto val = wk.value();
		auto key = wk.key();
		for(wk.start(ht); wk.ok(); wk.next())
		{
			ss << "key: ";
			di_showmem(key);
			ss << "val: ";
			di_showmem(val);
			if (val.isArray())
			{
				di_showdata(val.zarray());
			}
		}
		indents_--;

		ss << "]-- " << std::endl;
	}
	void di_showobj(zend_object* obj)
	{
		ss << "obj " << std::setw(4) << std::hex << obj;
		if (obj) {
			ss << " gc " << std::dec << GC_REFCOUNT(obj);
			ss << " class " << ZSTR_VAL(obj->ce->name) << std::endl;
		}

	}

	void di_showref(zend_reference* ref)
	{
			ss << "ref-h " << std::setw(4) << std::hex << ref << " ";
			ss << " gc " << std::dec << GC_REFCOUNT(ref) << " ";
			zval* m = &ref->val;
			di_showmem(m);
	}

	void output()
	{
		std::string txt = ss.str();
		zend_printf("%s\n", txt.data());
	} 
}; // end class

void showmem(const char* s, zval* m)
{
	if (!dump_info::run_state_)
	{
		return;
	}
	if (m == nullptr) {
		zend_printf("%s passed nullptr\n", s);
		return;
	}
	dump_info di(s);
	di.di_showmem(m);
	di.output();
}
void showstr(const char* s, zend_string* zs)
{
	if (!dump_info::run_state_)
	{
		return;
	}
	dump_info di(s);
	di.di_showstr(zs);
	di.output();
}
void showarray(const char* s, HashTable* ht)
{
	if (!dump_info::run_state_)
	{
		return;
	}
	dump_info di(s);
	di.di_showarray(ht);
	di.output();
}
void showdata(const char* s, HashTable* ht)
{
	if (!dump_info::run_state_)
	{
		return;
	}
	dump_info di(s);
	di.di_showdata(ht);
	di.output();
}

void showobj(const char* s, zend_object* obj)
{
	if (!dump_info::run_state_)
	{
		return;
	}
	dump_info di(s);
	di.di_showobj(obj);
	di.output();
}

bool dump_info::run_state_ = false;

#else
	// to allow leaving extant trace/debug calls in source, but have them do nothing.
	void showmem(const char* s, zval* m)
	{
		
	}
	void showstr(const char* s, zend_string* zs)
	{
		
	}
	void showarray(const char* s, HashTable* ht)
	{
	
	}
	void showobj(const char* s, zend_object* obj)
	{
	
	}
	void showdata(const char* s, HashTable* ht)
	{
	
	}



#endif
}; // namespace zpp


#endif
//show_zpp.cpp