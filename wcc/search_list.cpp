#ifndef SEARCH_LIST_CPP
#define SEARCH_LIST_CPP

#ifndef SEARCH_LIST_H
#include "search_list.h"
#endif

namespace wcc {
zend_class_entry* gIfFindLeafCE;

base_obj_mgr<SearchList>  SearchList::omg;

class SearchListInit : public state_init {
public:
	SearchListInit() : state_init() {}

	zstr_intern paths_key;
	zstr_intern name_key;

	virtual void init()
	{
		paths_key = "paths";
		name_key = "name";
	}
};

SearchListInit SLdata;

namespace fs = std::filesystem;

void SearchList::debug_info(htab_write hw)
{
	hw.set(SLdata.paths_key, paths_);
}

void 
SearchList::construct(zval_user paths)
{
	if (paths.isArray())
	{
		paths_ = paths.zarray();
	}
}

htab_read
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
SearchList::hasPath(zstr_user name)
{
	htab_read hr(paths_);

	size_t pcount = hr.size();

	if (!pcount)
		return false;

	while(pcount)
	{
		--pcount;
		zstr_user test = hr.get(pcount);
		if (zs_equal(test,name))
		{
			return true;
		}
	}
	return false;
}

void 
SearchList::setPaths(zval_user sp)
{
	paths_ = sp.zarray();
}

void 
SearchList::addPath(zstr_user p)
{
	if (!hasPath(p))
	{
		htab_write hw(paths_);
		hw.push_back(p);
	}
}

void 
SearchList::addPaths(zval_user sp)
{
	htab_read list(sp.zarray());
	size_t pcount = list.size();
	while(pcount > 0) 
	{
	 	--pcount;
	 	zstr_user test = list.get(pcount);
	 	if (test.size()) {
	 		addPath(test);
	 	}
	}
}

bool 
SearchList::try_path(
	const fs::path& dir, 
	const fs::path& file,  
	zstr_mgr& result)
{
	fs::path fpath = dir /  file;
	if (fs::is_regular_file(fpath)) 
	{
		std::string temp = fpath.string();
		result = zstr_temp(temp.data(), temp.size());
		return true;
	}
	return false;
}

zstr_mgr 
SearchList::findLeaf(zstr_user leaf, zval_user extensions)
{

	zstr_mgr result;

	htab_read hr(paths_);

	size_t pcount = hr.size();
	if (pcount == 0)
	{
		return result;
	}
	fs::path pleaf(leaf.vstr());
	fs::path pext = pleaf.extension();

	bool hasExt = !pext.empty();
	
	htab_read exlist(extensions.zarray());
	while(pcount)
	{
		--pcount;
		zstr_user sp = hr.get(pcount);
		fs::path fpath(sp.vstr());
		if (try_path(fpath, pleaf, result))
		{
			return result;
		}
		size_t extct = exlist.size();
		for(size_t i = 0; i < extct; i++)
		{
			zstr_user extry(exlist.get(i));
			fs::path fext(extry.vstr());
			pleaf.replace_extension(fext);
			if (try_path(fpath, pleaf, result))
			{
				return result;
			}
		}
	}
	return result;
}
}; //namespace &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&@@@@@@@@@@@@@@
//
ZEND_METHOD(Wcc_SearchList, __construct)
{
	zval*        paths;

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
	zval*        paths;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_ARRAY(paths)
	ZEND_PARSE_PARAMETERS_END();

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
	zval*        extensions;

	ZEND_PARSE_PARAMETERS_START(1, 2)
	Z_PARAM_STR(leaf)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_OR_NULL(extensions)
	ZEND_PARSE_PARAMETERS_END();

	SearchList* cobj = zval_toc<SearchList>(ZEND_THIS);
	zstr_mgr path = cobj->findLeaf(leaf, extensions);
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

	htab_read result = cobj->getPaths();
	result.return_zv(return_value); 
}


PHP_MINIT_FUNCTION(SearchList_reg)
{
	gIfFindLeafCE = register_class_Wcc_IfFindLeaf();

	auto ce = register_class_Wcc_SearchList(gIfFindLeafCE);

	SearchList::omg.classEntry(ce);

	return SUCCESS;
}

//search_list.cpp
#endif