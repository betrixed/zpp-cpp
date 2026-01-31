#ifndef SERIAL_FILE_CPP
#define SERIAL_FILE_CPP

#ifndef SERIAL_FILE_H
#include "serial_file.h"
#endif

namespace wcd {

using namespace zpp;

base_obj_mgr<SerialFile> SerialFile::omg;

SFData SFDi;

void SFData::init()
{
	sfile_ext = "sfe";
	file_iterator = "filesystemiterator";
	dir_iterator = "recursivedirectoryiterator";

	opt_cachedir = "cache_dir";
	opt_dirtree = "dirtree";
	opt_keeplocal = "keep-local";
	opt_deferwrite = "defer-write";

	user_session = "user_session";
	run_str = "run";
	dos_str = "dos";
	rm_alldir = "rm_alldir";
}


void 
SerialFile::construct(val_ptr options, val_ptr services)
{
	ICache::construct(options, services);

	val_rc test;

	if (options_.size())
	{
		this->cache_dir_ = options_.get(SFDi.opt_cachedir);

		test = options_.get(SFDi.opt_dirtree);
		this->dir_tree_ = test.isTrue();

		test = options_.get(SFDi.opt_keeplocal);
		this->keep_local_ = test.isTrue();

		test = options_.get(SFDi.opt_deferwrite);
		this->defer_write_ = test.isTrue();
	}
	if (!cache_dir_.ok())
	{
		test = this->getService(SFDi.run_str);
		if (test.ok())
		{
			cache_dir_ = test.property(Run_i.temp_dir);
		}
	}
}

bool 
SerialFile::clear()
{
	bool result = false;

	obj_rc dos = this->getService(SFDi.dos_str);

	obj_rc sess = this->getService(SFDi.user_session);

	zval_rc path(cache_dir_);
	zval_rc depth(int(0));

	int ct = dos.call(SFDi.rm_alldir,  path, depth);
	result = (ct > 0);
	sbuf buf;

	if (sess.ok())
	{
		buf << "cleared " << ct << " from " << cache_dir_;
		sess->call(SFDi.flash, buf.zstr());
	}
	return result;
}

int  
SerialFile::deleteExpired()
{

}

bool 
SerialFile::set(str_ptr key, val_ptr data, zend_long ttl = 0)
{

}

val_rc 
SerialFile::get(str_ptr key, val_ptr noval = val_ptr())
{

}

bool 
SerialFile::deleteKey(str_ptr key)
{

}

void 
SerialFile::debug_info(htab_rw s)
{

}


}//namespace wcd


#endif
