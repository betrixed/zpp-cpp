#ifndef SEARCH_LIST_H
#define SEARCH_LIST_H

//search_list.h

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


#include <filesystem>

namespace wcc {

	namespace fs = std::filesystem;

	class  SearchList : public base_d {
	protected:
		htab_rc paths_;
	public:

		virtual void debug_info(htab_rw hw);
		
		static base_obj_mgr<SearchList> omg;

		void construct(val_ptr paths);

		void clear();

		bool hasPath(str_ptr name);

		void setPaths(val_ptr sp);

		void addPath(str_ptr p);

		void addPaths(val_ptr sp);

		bool try_path(
			const fs::path& dir, 
			const fs::path& file,  
			str_rc& result);

		str_rc findLeaf(str_ptr leaf, val_ptr extensions);
		
		str_ptr getName();

		htab_ptr getPaths();

		VIRTUAL_ZOBJPTR

	};

	extern zend_class_entry* gIfFindLeafCE;
	
};

#endif