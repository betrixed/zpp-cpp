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
		htab_mgr paths_;
	public:

		virtual void debug_info(htab_write hw);
		
		static base_obj_mgr<SearchList> omg;

		void construct(zval_user paths);

		void clear();

		bool hasPath(zstr_user name);

		void setPaths(zval_user sp);

		void addPath(zstr_user p);

		void addPaths(zval_user sp);

		bool try_path(
			const fs::path& dir, 
			const fs::path& file,  
			zstr_mgr& result);

		zstr_mgr findLeaf(zstr_user leaf, zval_user extensions);
		
		zstr_user getName();

		htab_read getPaths();

	};

	extern zend_class_entry* gIfFindLeafCE;
	
};

#endif