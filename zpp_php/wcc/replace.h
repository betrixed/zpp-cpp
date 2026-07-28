#ifndef WCC_REPLACE_H
#define WCC_REPLACE_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc 
{
using namespace zpp;

class Replace_init : public state_init {
public:
	str_intern prop_expr;

	void init() override;

};

extern Replace_init REPi;


class Replace : public base_d {
	protected:
		preg   expr_;
		obj_rc src_;
	public:

		static base_obj_mgr<Replace> omg;

		static str_rc property(obj_ptr obj, str_ptr data);

		Replace(obj_ptr obj, str_ptr expr = str_ptr());

		Replace();

		void   construct(obj_ptr obj, str_ptr rexpr);

		str_rc eval(str_ptr src);
	};
}; // namespace wcc

#endif
