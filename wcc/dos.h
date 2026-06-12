#ifndef WCC_DOS_H
#define WCC_DOS_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

namespace wcc {

using namespace zpp;

class Dos : public base_d {
protected:
	str_rc cwd_;
public:

	static base_obj_mgr<Dos> omg;

	void construct(str_ptr cwd);

	void clean_dir(str_ptr path);

	error_return copy_all(str_ptr from, str_ptr to);

	bool_return copy_file(str_ptr from, str_ptr to);

	str_rc get_cwd();

	bool make_dir(str_ptr path, int permissions = 0755);

	str_return real_dirpath(str_ptr rpath);

	str_return real_filepath(str_ptr rpath);

	bool_return rm_all(htab_ptr flist);

	int  rm_alldir(str_ptr path, bool deldir = false);

	int rm_old(htab_ptr rlist, int tsecs);

	int_return sync_dir(str_ptr src, str_ptr dest);
};


}


#endif