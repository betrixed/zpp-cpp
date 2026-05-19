#ifndef SESSION_FLASH_H
#define SESSION_FLASH_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc {


	class FlashInit : public state_init {
	public:
		void init() override;
		str_intern lines_str;
		str_intern text_str;
		str_intern status_str;
	};

extern FlashInit FLi;

	class Flash : public base_d {
	protected:
		val_ptr lines_;
	public:
		void construct(htab_ptr data);

		bool hasData();

		void add(str_ptr text, str_ptr status);

		htab_rc getData();

		void clear();

	}
};


#endif 