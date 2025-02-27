#ifndef WCC_PAIR_H
#define WCC_PAIR_H

//pair.h

#ifndef WC_BASE_H
#include "wc_base.h"
#endif

namespace wcc {

	class Pair : public base_d 
	{
	protected:
		zval_own first_;
		zval_own second_;


	public:
		static base_obj_mgr<Pair> omg;

		void construct(zval_ptr f1, zval_ptr s2)
		{
			first_ = f1;
			second_ = s2;
		}

		const zval_own& first() const { return first_; }

		const zval_own& second() const { return first_; }
	};

}; // namespace

#endif