#ifndef TOML_PHP_CPP
#define TOML_PHP_CPP

#ifndef TOML_PHP_H
#include "toml_php.h"
#endif

#ifndef TOML_ARGINFO_H
#define TOML_ARGINFO_H
extern "C" {
	#include "stub/toml_arginfo.h"
}
#endif

#include <filesystem>

#include "daytime.cxx"


namespace wcc {

base_obj_mgr<Toml> Toml::omg;

namespace fs = std::filesystem;

using namespace toml;

Toml::Toml()
{

}

Toml::~Toml()
{

}

void
Toml::name_table(toml_table_t* st)
{
		//zend_printf("In name_table\n");
		htab_wr  array(top_->table_);
		//showarray("old top", top_->table_);

		htab_rc 	 ztab; // new table
		htab_wr   hw(ztab); //writer to it.

		zstr_temp tkey(st->key);
		//zend_printf("New table named %s\n", st->key);
		array.set(tkey, ztab);

		stack_htab levelup(hw, top_, &top_);
	   
	  //showarray("top table", top_->table_);
		// immediate bring refcount to 1
		ztab.init();
	  	z_table(st);
}	


void
Toml::array_table(toml_table_t* st)
{
		//zend_printf("In array_table\n");

	  htab_wr  array(top_->table_);

	  htab_rc ztab;
	  htab_wr hw(ztab); //make writable array
	  array.push_back(ztab);

	  stack_htab levelup(hw, top_, &top_);
	  // immediate bring refcount to 1
	  ztab.init();
	  z_table(st);
}

void 
Toml::z_table(toml_table_t* st)
{
	//zend_printf("In z_table\n");
	if (!st)
	{
		//zend_throw_error(zend_ce_error, "null table",0);
		return;
	}
	if (st->nkval > 0)
	{
	  	w_values(toml::ppref(st->nkval,st->kval));
	}
	if (st->narr > 0) 
	{
		w_arrays(toml::ppref(st->narr, st->arr));
	}
	if (st->ntab > 0)
	{
		w_tables(toml::ppref(st->ntab, st->tab));
	}
}

void 
Toml::z_array(toml_array_t* arr)
{
		htab_wr ctop(top_->table_);

		int kind = arr->kind;
		int vtype = arr->type;

		htab_rc   ztab;
		htab_wr hw(ztab);

		if (arr->key)
		{
			zstr_temp akey(arr->key);
			//zend_printf("Array key %s\n", arr->key);
			ctop.set(akey, hw);
		}
		else {
			//zend_printf("No array key\n");
			ctop.push_back(hw);
		}
		stack_htab build(hw, top_, &top_);
		ctop = ztab;
		ztab.init();
		
		if (arr->nitem > 0)
		{
				for(int i = 0; i < arr->nitem; i++)
				{
					  const toml_arritem_t& ai = arr->item[i];
					  switch(kind)
					  {
					  case 'a':
					      z_array(ai.arr);
					      break;
					  case 't':
					      array_table(ai.tab);
					      break;
					  case 'v':
					  	  toml_keyval_t temp;
					  	  temp.key = nullptr;
					  	  temp.val = ai.val;
					      z_value(&temp,vtype);
					      break;
					  default:
					      break;
					  }
				}
		}

}

#ifdef DEBUG_SHOW_TS
static void show_ts(toml_timestamp_t& ts)
{
	str_buf buf;

	if (ts.year) {
		 buf << *ts.year << '-' << *ts.month << '-' <<  *ts.day;
	}
	if (ts.hour) {
		buf << *ts.hour << ':' << *ts.minute << ':' <<  *ts.microsec;
	}
	str_rc dump(std::move(buf));

	//zend_printf("ts = %s\n", dump.data());
}
#endif

void 
Toml::z_value(toml_keyval_t* st, int expect)
{

	const char* key = st->key; // Can be nullptr!!
	const char* val = st->val; // must not be nullptr!!

	//zend_printf("In z_value for %lx\n", key);

	htab_wr table = top_->table_;

	int  checked = -1; // not checked!
	val_rc 	  store; // managed zval struct
	toml_datum_t   p;
	toml_timestamp_t ts;
	
	//zend_printf("raw value %s\n", val);

	p = {0};
	if (expect==0) 
	{
		 expect = 's';
	}

	while(checked != 0)
	{
		switch(expect)
		{
		case 's':
			checked = toml_rtos(val, &p.u.s);
			if (checked==0)
			{
				store = zstr_temp(p.u.s);
				xfree(p.u.s);
				break;
			} // fall through!
		case 'b' : 
			checked = toml_rtob(val, &p.u.b);
			if (checked==0)
			{
					store.set_bool((bool) p.u.b);
					break;
			}
			// fall through!
		case 'i': //integer
			checked = toml_rtoi(val, &p.u.i);
			if (checked==0)
			{
				store = p.u.i;
				break;
			}
		case 'd' : //double
			checked = toml_rtod(val, &p.u.d);
			if (checked==0)
			{
				store = p.u.d;
				break;
			}
		case 't':
		case 'D':
		case 'T':
			//zend_printf("expect %c for %s\n", expect, val);
			checked = toml_rtots(val, &ts);
			if (checked==0)
			{
				//show_ts(ts);
				//make a datetime object
				datetime_obj dt;


			  if (ts.year) {
			  	dt.setDate(*(ts.year), *(ts.month), *(ts.day));
			  }
			  if (ts.hour) {
			  	dt.setTime(*(ts.hour), *(ts.minute), *(ts.second), *(ts.microsec));
			  }
			  store = dt;
			  break;
			}
			else {
				//zend_printf("toml_rtots fail\n");
			}
		default:
			break;
		}
		if (checked==0)
		{
				if (key)
				{
					 zstr_temp vkey(key);
					 table.set(vkey, store);
				}
				else {
					  table.push_back(store);
				}
		}
		if (expect=='s')
		{
			checked = 0;
		}
	}
}

void 
Toml::w_tables(const toml::ppref<toml_table_t>& tables)
{
	auto zit = tables.end();
  auto ait = tables.begin();
	for(; ait != zit; ait++)
	{
		toml_table_t* st = *ait;
		if (st->key)
		{
			name_table(st);
		}
		else {
			array_table(st);
		}
	}
}

void Toml::w_arrays(const toml::ppref<toml_array_t>& arrays)
{
	auto zit = arrays.end();
	auto ait = arrays.begin();
	for(; ait != zit; ait++)
	{
		z_array(*ait);
	}
}

void Toml::w_values(const toml::ppref<toml_keyval_t>& values)
{
	auto zit = values.end();
	auto ait = values.begin();
	for(; ait != zit; ait++)
	{
		z_value(*ait,0); // type not available yet
	}
}


htab_rd
Toml::parseFile(str_ptr path)
{
	if (!fs::is_regular_file(path.data()))
	{
		zend_throw_error(zend_ce_error,"File path does not exist %s",path.data());
		return htab_empty();
	}

	str_rc toml = file_get_contents(path);
	return parse(toml);
}

htab_rd
Toml::parse(str_ptr toml)
{
	hold_ = toml;
	
	//zstr_own text = toml.substr(0,50);
	//zend_printf("toml parse \n",text.data());
	toml::Result tree = toml::parse(toml);

	toml::Table* tb = tree.table.get();
	//zend_printf(" parse returned %lx!!\n", tb);

	root_.init();
	if (!tb) 
	{
    	zend_throw_error(zend_ce_error, "cannot parse as toml : %s", tree.errmsg.c_str());
    	return root_;
	}

	auto base_ptr = tb->ptr();

	stack_htab stick(root_, nullptr, &top_);

	z_table(base_ptr);
	//zend_printf("toml parse done \n");
	return root_;
}


htab_rd //static
Toml::decode(str_ptr toml)
{
	obj_rc obj = Toml::omg.new_zobj();
	Toml*  cobj = zobj_toc<Toml>(obj);
	htab_rd result = cobj->parse(toml);
	return result;
}

htab_rd //static
Toml::decodeFile(str_ptr path)
{
	obj_rc obj = Toml::omg.new_zobj();
	Toml*  cobj = zobj_toc<Toml>(obj);

	htab_rd result = cobj->parseFile(path);
	return result;
}


};// namespace

using namespace wcc;

ZEND_METHOD(Toml, decode)
{
	zend_string* s;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(s)
	ZEND_PARSE_PARAMETERS_END();

	htab_rd result = Toml::decode(s);
	result.return_zv(return_value);

}

ZEND_METHOD(Toml, decodeFile)
{
	zend_string* path;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(path)
	ZEND_PARSE_PARAMETERS_END();

	htab_rd result = Toml::decodeFile(path);
	result.return_zv(return_value);

}


ZEND_METHOD(Toml, parse)
{
	zend_string* s;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(s)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Toml>(ZEND_THIS);

	htab_rd result = cobj->parse(s);
	result.return_zv(return_value);
}

ZEND_METHOD(Toml, parseFile)
{
	zend_string* path;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(path)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Toml>(ZEND_THIS);

	htab_rd result = cobj->parseFile(path);
	result.return_zv(return_value);
}

PHP_MINIT_FUNCTION(Toml_reg)
{
	zend_class_entry* ce = register_class_Toml();

	Toml::omg.classEntry(ce);

	return SUCCESS;
}

#endif
