#ifndef SEARCH_LIST_CPP
#define SEARCH_LIST_CPP

#ifndef SEARCH_LIST_H
#include "search_list.h"
#endif

#ifndef SEARCH_LIST_ARGINFO_H
extern "C" {
	#include "stub/searchlist_arginfo.h"
}
#endif

namespace wcc {
zend_class_entry* gIfFindLeafCE;

base_obj_mgr<SearchList>  SearchList::omg;

class SearchListInit : public state_init {
public:

	str_intern paths_key;
	str_intern name_key;

	void init() override
	{
		paths_key = "paths";
		name_key = "name";
	}
};

SearchListInit SLdata;

namespace fs = std::filesystem;

void SearchList::debug_info(htab_rw hw)
{
	hw.set(SLdata.paths_key, paths_);
}

void 
SearchList::construct(val_ptr paths)
{
	if (paths.isArray())
	{
		paths_ = paths.zarray();
	}
	else {
		paths_ = htab_ptr::empty_array();
	}
}

htab_ptr
SearchList::getPaths()
{
	return paths_;
}

void
SearchList::clear()
{
	paths_.init();
}

bool 
SearchList::hasPath(str_ptr name)
{
	htab_ptr hr(paths_);

	size_t pcount = hr.size();

	if (!pcount)
		return false;

	while(pcount)
	{
		--pcount;
		str_ptr test = hr.get(pcount);
		if (!zs_cmp(test,name))
		{
			return true;
		}
	}
	return false;
}

void 
SearchList::setPaths(val_ptr sp)
{
	paths_ = sp.zarray();
}

void 
SearchList::addPath(str_ptr p)
{
	if (!hasPath(p))
	{
		htab_rw hw(paths_);
		hw.push_back(p);
	}
}

void 
SearchList::addPaths(htab_ptr list)
{
	size_t pcount = list.size();
	while(pcount > 0) 
	{
	 	--pcount;
	 	str_ptr test = list.get(pcount);
	 	if (test.size()) {
	 		addPath(test);
	 	}
	}
}

bool 
SearchList::try_path(
	const fs::path& dir, 
	const fs::path& file,  
	str_rc& result)
{
	fs::path fpath = dir /  file;
	if (fs::is_regular_file(fpath)) 
	{
		std::string temp = fpath.string();
		result = str_temp(temp.data(), temp.size());
		return true;
	}
	return false;
}

str_rc 
SearchList::findLeaf(str_ptr leaf, htab_ptr exlist)
{

	str_rc result;

	htab_ptr hr(paths_);

	size_t pcount = hr.size();
	if (pcount == 0)
	{
		return result;
	}
	fs::path pleaf(leaf.vstr());
	fs::path pext = pleaf.extension();

	while(pcount)
	{
		--pcount;
		str_ptr sp = hr.get(pcount);
		fs::path fpath(sp.vstr());
		if (try_path(fpath, pleaf, result))
		{
			return result;
		}
		if (exlist.size())
		{
			for_key_value kv1;
			for(kv1.start(exlist); kv1.ok(); kv1.next())
			{
				str_ptr extry(kv1.value());
				fs::path fext(extry.vstr());
				pleaf.replace_extension(fext);
				if (try_path(fpath, pleaf, result))
				{
					return result;
				}
			}
		}
	}
	return result;
}
}; //namespace &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&@@@@@@@@@@@@@@
//
ZEND_METHOD(Wcc_SearchList, __construct)
{
	zval*        paths = nullptr;

	ZEND_PARSE_PARAMETERS_START(0, 1)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(paths)
	ZEND_PARSE_PARAMETERS_END();

	SearchList* cobj = zval_toc<SearchList>(ZEND_THIS);
	cobj->construct(paths);
}

ZEND_METHOD(Wcc_SearchList, addPath)
{
	zend_string*        path;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(path)
	ZEND_PARSE_PARAMETERS_END();

	SearchList* cobj = zval_toc<SearchList>(ZEND_THIS);
	cobj->addPath(path);

}

ZEND_METHOD(Wcc_SearchList, addPaths)
{
	htab_ptr	paths;
	zarg_rd args(execute_data);

	args.zarray(paths, args.need(0));
	SearchList* cobj = zval_toc<SearchList>(ZEND_THIS);
	cobj->addPaths(paths);
}

ZEND_METHOD(Wcc_SearchList, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();

	SearchList* cobj = zval_toc<SearchList>(ZEND_THIS);

	cobj->clear();
}

ZEND_METHOD(Wcc_SearchList, findLeaf)
{
	zend_string* leaf;
	zval*        extensions = nullptr;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_STR(leaf)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(extensions)
	ZEND_PARSE_PARAMETERS_END();

	SearchList* cobj = zval_toc<SearchList>(ZEND_THIS);
	str_rc path = cobj->findLeaf(leaf, extensions);
	path.move_zv(return_value); 
}

ZEND_METHOD(Wcc_SearchList, hasPath)
{
	zend_string*        path;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(path)
	ZEND_PARSE_PARAMETERS_END();

	SearchList* cobj = zval_toc<SearchList>(ZEND_THIS);
	bool result = cobj->hasPath(path);
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_SearchList, setPaths)
{
	zval*        paths;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(paths)
	ZEND_PARSE_PARAMETERS_END();

	SearchList* cobj = zval_toc<SearchList>(ZEND_THIS);
	cobj->setPaths(paths);
}

ZEND_METHOD(Wcc_SearchList, getPaths)
{
	ZEND_PARSE_PARAMETERS_NONE();

	SearchList* cobj = zval_toc<SearchList>(ZEND_THIS);

	htab_ptr result = cobj->getPaths();
	result.copy_zv(return_value); 
}


PHP_MINIT_FUNCTION(SearchList_reg)
{
	gIfFindLeafCE = register_class_Wcc_IfFindLeaf();

	auto ce = register_class_Wcc_SearchList(gIfFindLeafCE);

	SearchList::omg.classEntry(ce);

	STATE_INIT_ADD(SLdata)
	
	return SUCCESS;
}

//search_list.cpp
#endif