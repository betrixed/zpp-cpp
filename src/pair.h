#ifndef WCC_PAIR_H
#define WCC_PAIR_H

//pair.h

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc {
using namespace zpp;

	class Pair : public base_d 
	{
	protected:
		zval_mgr first_;
		zval_mgr second_;


	public:
		static base_obj_mgr<Pair> omg;

		void construct(zval_user f1, zval_user s2)
		{
			first_ = f1;
			second_ = s2;
		}

		zval_user first() const { return first_; }

		zval_user second() const { return second_; }
	};

}; // namespace

#endif