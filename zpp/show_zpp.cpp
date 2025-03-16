#ifndef SHOW_ZPP_CPP
#define SHOW_ZPP_CPP

// Debug dumpers

#ifndef FOR_KEY_VALUE_H
#include "for_key_value.h"
#endif

extern "C" {
	#include <Zend/zend_lazy_objects.h>
}

namespace zpp {

zstr_output dump_info::dumper_d;


void 
dump_info::object_property_dump(
	zend_property_info *prop_info, 
	zval *zv, 
	zend_ulong index, 
	zend_string *key, 
	int level) 
{
	const char *prop_name, *class_name;
	indent(level);

	ss << '[';
	if (key == NULL) 
	{ /* numeric key */
		ss << (int)index;
	} 
	else { /* string key */
		zend_unmangle_property_name(key, &class_name, &prop_name);
		if (class_name) {
			if (class_name[0] == '*') {
				ss.quote_name(prop_name);
				ss << ":protected";
			} else {
				ss.quote_name(prop_name);
				ss << ':';
				ss.quote_name(class_name); 
				ss << ":private";
			}
		} else {
			ss.quote_name(prop_name);
		}
		ss << "] ";
	}

	if (prop_info && Z_TYPE_P(zv) == IS_UNDEF) {
		zend_string *type_str = zend_type_to_string(prop_info->type);
		ss << "uninitialized(" << type_str << ")";
		zend_string_release(type_str);
	} else {
		di_dump(zv, level + 1);
	}
}

static const char *get_object_prefix(zend_object *obj) 
{
	if (EXPECTED(!zend_object_is_lazy(obj))) {
		return "";
	}

	if (zend_object_is_lazy_proxy(obj)) {
		return "lazy proxy ";
	}

	return "lazy ghost ";
}

void 
dump_info::di_show_resource(zval* zu)
{
	const char *type_name = zend_rsrc_list_get_rsrc_type(Z_RES_P(zu));
	if (!type_name)
	{
		type_name = "Unknown";
	}
	int gc = Z_REFCOUNT_P(zu);
	ss << "resource " << Z_RES_P(zu)->handle << " type(" << type_name << ")" 
		<< " refcount(" << gc << ")\n";
}

void // static
dump_info::dump(zval_user val, int level)
{
	dump_info di;

	di.di_dump(val, level);

}

void // static
dump_info::msg_dump(const char* msg, zval_user val)
{
	dump_info di;

	di.output(msg);

	di.di_dump(val);
}

dump_info::dump_info(const char* s) : ss(dumper_d)
{
	ss << s << ' ';
}

void 
dump_info::show_properties(zend_object* zobj, HashTable* myht, int level)
{
	zend_long index;
	zend_string *key;
	zval *val;
	for_key_value fkv;
	indent(level);
	ss << "{\n";
	for(fkv.start(myht); fkv.ok(); fkv.next())
	{
		zend_property_info *prop_info = nullptr;

		zval* val = fkv.value();
		zend_string* key = fkv.key();

		if (Z_TYPE_P(val) == IS_INDIRECT) {
			val = Z_INDIRECT_P(val);
			if (key) {
				prop_info = zend_get_typed_property_info_for_slot(zobj, val);
			}
		}

		if (!Z_ISUNDEF_P(val) || prop_info) {
			object_property_dump(prop_info, val, index, key, level+1);
		}
	}
	indent(level);
	ss << "} " << level << '\n';
}


void 
dump_info::di_dump(zval_user zu, int level)
{
	HashTable *myht = NULL;
	zend_string *class_name;
	// for each values
	zend_long index;
	zend_string *key;
	zval *val;
	int refadjust;
	int count;
	const char *packed;
	for_key_value fkv;
	bool imflag;

	//zend_printf("di_dump %ld level %d\n",zu.ztype(), level);
	
	switch (zu.ztype()) 
	{
	case IS_FALSE:
		ss << "bool(false)\n";
		break;
	case IS_TRUE:
		ss << "bool(true)\n";
		break;
	case IS_NULL:
		ss << "NULL\n";
		break;
	case IS_LONG:
		ss << "int(" << zu.zlong() << ")\n";
		break;
	case IS_DOUBLE:
		ss << "float(" << zu.zdouble() << ")\n";
		break;
	case IS_STRING:
		di_showstr(zu.zstr());
		endl();
		break;
	case IS_ARRAY:
		myht = zu.zarray();
		refadjust = 0;
		imflag = (GC_FLAGS(myht) & GC_IMMUTABLE);

		if (!imflag) {
			if (GC_IS_RECURSIVE(myht)) {
				di_showarray(myht,refadjust);
				ss << "*RECURSION*\n";
				return;
			}
			GC_ADDREF(myht);
			refadjust = -1;
			GC_PROTECT_RECURSION(myht);
		}
		packed = HT_IS_PACKED(myht) ? "packed " : "";

		di_showarray(myht,refadjust);
		indent(level);
		ss << packed << "{\n";
		
		for (fkv.start(myht); fkv.ok(); fkv.next())
		{
			key = fkv.key();
			index = fkv.index();
			val = fkv.value();
			indent(level+1);
			if (key)
			{
				ss << '[' << key << ']';
			}
			else {
				ss << '[' << index << ']';
			}
			ss << " => ";
			di_dump(val, level+1);
		}
		
		if (!imflag) {
			GC_UNPROTECT_RECURSION(myht);
			GC_DELREF(myht);
		}
		indent(level);
		ss << '}' << level << '\n';
		break;
	case IS_OBJECT: 
	{
		/* Check if this is already recursing on the object before calling zend_get_properties_for,
		 * to allow infinite recursion detection to work even if classes return temporary arrays,
		 * and to avoid the need to update the properties table in place to reflect the state
		 * if the result won't be used. (https://github.com/php/php-src/issues/8044) */
		zend_object *zobj = zu.zobject();
		const char *prefix = get_object_prefix(zobj);

		ss << prefix;

		
		if (GC_IS_RECURSIVE(zobj)) {

				ss << "*RECURSION* ";
				di_showobj(zobj);
				return;
		}
		
		GC_PROTECT_RECURSION(zobj);
		di_showobj(zobj);
		myht = zend_get_properties_for(zu, ZEND_PROP_PURPOSE_DEBUG);

		if (myht) 
		{
			htab_mgr adopter;

			adopter.adopt(myht);

			show_properties(zobj, myht, level);

		}

		GC_UNPROTECT_RECURSION(zobj);
		break;
	}
	case IS_RESOURCE: {
		di_show_resource(zu);
		break;
	}
	case IS_REFERENCE:
		indent(level);
		di_showref(Z_REF_P(zu));
		di_dump(Z_REFVAL_P(zu), level + 1);
		break;
	default:
		indent(level);
		di_showmem(zu);
		ss << "UNKNOWN:0\n";
		break;
	}
}

void dump_info::indent(int ct)
{
	for (int i = 0; i < ct; i++)
	{
		ss << "  ";
	}
}	


	void dump_info::di_showmem(zval *m) 
	{

		ss << "zval " << (void*)m << " ";

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

	void dump_info::di_showstr(zend_string* p) 
	{
		if (!p) {
			ss << "p = nullptr";
		}
		else {
		   const char* data = ZSTR_VAL(p);
		   size_t   slen = ZSTR_LEN(p);

			ss << " str(" << iform(Numf::DEC) << (int) slen << ") " << iform(Numf::HEX) << p;
			ss << " gc " << (int) GC_REFCOUNT(p) << " ";
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

	void dump_info::di_showarray(HashTable* ht, int refadj)
	{
		ss << ' ';
		if (!ht) {
			ss << "is nullptr\n";
			return;
		}

		size_t ct = zend_array_count(ht);
		if (ht->gc.u.type_info & GC_IMMUTABLE) {
			ss << "immutable ";
		}
		ss << iform(Numf::DEC) << "arr(" << ct <<") "  << iform(Numf::HEX) << ht;
		/* if (ht->u.flags & htab_mgr::COW_VIOLATE) {
			ss << " vcow ";
		}
		else {
			ss << " cow ";
		}
		*/
		ss << " gc " << (size_t) GC_REFCOUNT(ht)+refadj;

	}
	void dump_info::di_showdata(HashTable* ht, int level)
	{
		if (!ht) {
			ss << "is nullptr\n";
			return;
		}

		di_showarray(ht);

		htab_walk wk;
		indent(level);
		ss << "--[\n";

		auto val = wk.value();
		auto key = wk.key();	

		for(wk.start(ht); wk.ok(); wk.next())
		{

			indent(level+1);
			ss << "key: ";
			di_showmem(key);
			ss << '\n';
			indent(level+1);
			ss << "val: ";
			di_showmem(val);
			ss << '\n';
			if (val.isArray())
			{
				di_showdata(val.zarray(), level+1);
			}
		}
		indent(level);
		ss << "]--\n";
	}
	void dump_info::di_showobj(zend_object* obj)
	{
		ss << "obj " << iform(Numf::HEX) << obj;
		if (obj) 
		{
			ss << " gc " << iform(Numf::DEC) << (size_t) GC_REFCOUNT(obj);
			ss << " class " << ZSTR_VAL(obj->ce->name) << '\n';
		}

	}

	void dump_info::di_showref(zend_reference* ref)
	{
			ss << "ref-h " << iform(Numf::HEX) << ref << " ";
			ss << " gc " << iform(Numf::DEC) << (size_t) GC_REFCOUNT(ref) << " ";
			zval* m = &ref->val;
			di_showmem(m);
	}

	void dump_info::output(const char* msg)
	{
		ss << msg;
		//zstr_mgr txt(std::move(ss));
		//zend_printf("%s\n", txt.data());
	} 
	void dump_info::endl()
	{
		ss << '\n';
	}

void showmem(const char* s, zval* m)
{
	if (!dump_info::run_state_)
	{
		return;
	}
	if (s == nullptr) {
		zend_printf("passed nullptr\n");
		return;
	}
	dump_info di(s);
	di.di_showmem(m);
	di.endl();
	
}
void showstr(const char* s, zend_string* zs)
{
	if (!dump_info::run_state_)
	{
		return;
	}
	dump_info di(s);
	di.di_showstr(zs);
	di.endl();
}
void showarray(const char* s, HashTable* ht)
{
	if (!dump_info::run_state_)
	{
		return;
	}
	dump_info di(s);
	di.di_showarray(ht);
	di.endl();
}
void showdata(const char* s, HashTable* ht)
{
	if (!dump_info::run_state_)
	{
		return;
	}
	dump_info di(s);
	di.di_showdata(ht);
	di.endl();

}

void showobj(const char* s, zend_object* obj)
{
	if (!dump_info::run_state_)
	{
		return;
	}
	dump_info di(s);
	di.di_showobj(obj);
	di.endl();

}


bool dump_info::run_state_ = false;

}; // namespace zpp


#endif
//show_zpp.cpp