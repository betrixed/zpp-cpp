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
	public:


		void construct(val_ptr p1, val_ptr p2);
		
		val_ptr first() const;

		val_ptr second() const;

		val_ptr key() const;

		val_ptr value() const;
		
		double test_calc() const 
		{
			long a, b;
			a = first().zlong();
			b = second().zlong();
			return (a + b) / double(a);
		}

	public:
		static base_obj_mgr<Pair> omg;
		
		VIRTUAL_ZOBJPTR


	};

}; // namespace

#endif