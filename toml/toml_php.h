#ifndef TOML_PHP_H
#define TOML_PHP_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef TOML_H
#include "toml.h"
#include "toml.c"
#include "tomlcpp.hpp"
#include "tomlcpp.cxx"
#endif

namespace wcc {

using namespace zpp;

class Toml : public base_d {
protected:
	str_rc 		 hold_; // src_ referenced zend_string*
	htab_rc		 root_;	// returned HashTable*

	struct stack_htab {
		htab_rw 	  table_;
		stack_htab*   parent_;
		stack_htab**  top_;

		//stack_htab() : table_((HashTable*)nullptr), parent_(nullptr) {}

		stack_htab(htab_rw stab, stack_htab* p, stack_htab** top)
			: table_(stab), parent_(p), top_(top) 
		{
			*top_ = this;
		}

		void set_top(stack_htab** t) { top_ = t; }

		~stack_htab() {
			*top_ = parent_;
		}
	};

	stack_htab*	  	  top_;

protected:
	void z_table(toml_table_t* st);
	void z_array(toml_array_t* st);
	void z_value(toml_keyval_t* st, int expect);

	void name_table(toml_table_t* st);
	void array_table(toml_table_t* st);

	void w_tables(const toml::ppref<toml_table_t>& tables);
	void w_arrays(const toml::ppref<toml_array_t>& arrays);
	void w_values(const toml::ppref<toml_keyval_t>& values);

	#ifdef TOML_DBG
	void show_status(const char* sp);
	#endif
public:
	// regular expressions index, and token ids
	
	Toml();
	virtual ~Toml();

	htab_rd parseFile(str_ptr path);
	htab_rd parse(str_ptr toml);

	static htab_rd decode(str_ptr toml);
	static htab_rd decodeFile(str_ptr toml);
	static base_obj_mgr<Toml> omg;

};


}; // namespace wcc
//toml_stream.h
#endif