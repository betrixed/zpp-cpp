#ifndef SERIAL_FILE_H
#define SERIAL_FILE_H

#ifndef ICACHE_H
#include "icache.h"
#endif

namespace wcc {

using namespace zpp;
	
class  SFData : public state_init 
{
public:
	str_intern sfile_ext;
	str_intern file_iterator;
	str_intern dir_iterator;

	str_intern opt_cachedir;
	str_intern opt_dirtree;
	str_intern opt_keeplocal;
	str_intern opt_deferwrite;

	str_intern run_str;
	str_intern dos_str;
	str_intern user_session;
	str_intern rm_alldir;

	void init() override;
};


class  SerialFile : public ICache {
protected:
	str_rc  cache_dir_;
	bool    keep_local_;
	bool    dir_tree_;
	bool    defer_write_;

public:

	static base_obj_mgr<SerialFile> omg;

	void construct(val_ptr options, val_ptr services) override;
	bool clear() override;
	int  deleteExpired() override;
	bool set(str_ptr key, val_ptr data, zend_long ttl = 0) override;
	val_rc get(str_ptr key, val_ptr noval = val_ptr()) override;
	bool deleteKey(str_ptr key) override;

 	virtual void debug_info(htab_rw s);
 	
	VIRTUAL_ZOBJPTR
};


}; // namespace

//icache.h
#endif