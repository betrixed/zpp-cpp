#ifndef DIR_CACHE_CPP
#define DIR_CACHE_CPP

#ifndef DIR_CACHE_H
#include "dircache.h"
#endif

#ifndef FN_CALL_H
#include "zpp/fn_call.h"
#endif


#ifndef DIR_CACHE_ARGINFO
#define DIR_CACHE_ARGINFO
extern "C" {
	#include "stub/dircache_arginfo.h"
};
#endif

#ifndef REFLECT_CACHE_H
#include "reflect_cache.h"
#endif

#ifndef DIRECTORY_SEPARATOR
#define DIRECTORY_SEPARATOR '/'
#endif

namespace wcc {

using namespace zpp;

base_obj_mgr<DirCache> DirCache::omg;

SFData SFDi;


int  SFData::get_fsi_flags()
{
	if (file_itflags)
	{
		return file_itflags;
	}
	class_data cdata(file_iterator);

	val_rc temp = cdata.constant_value(skip_dots);
	file_itflags = temp.zlong();
	temp = cdata.constant_value(key_as_pathname);
	file_itflags |= temp.zlong();
	temp = cdata.constant_value(current_as_fileinfo);
	file_itflags |= temp.zlong();

	return file_itflags;
}

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

	skip_dots = "SKIP_DOTS";
	key_as_pathname = "KEY_AS_PATHNAME";
	current_as_fileinfo = "CURRENT_AS_FILEINFO";

	valid_s = "valid";
	current_s = "current";
	key_s = "key";
	gettype_s = "gettype";

	getchildren_s = "getchildren";
	getextension_s = "getextension";
	getpath_s = "getpath";

	file_s = "file";
	dir_s = "dir";
	next_s = "next";
	fmode_w = "w";
	fmode_r = "r";
	flash_s = "flash";

	file_itflags = 0;
	
}


void 
DirCache::construct(val_ptr options)
{
	ICache::construct(options);

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
	/*
	This will interfere with extension modules link independence
	if (!cache_dir_.ok())
	{
		test = this->getService(SFDi.run_str);
		if (test.ok())
		{
			obj_ptr run = test.zobject();
			cache_dir_ = run.property(Run_i.temp_dir);
		}
	}
	*/
}

static bool is_expired(str_ptr file_name)
{
	file_res  fin(file_name, SFDi.fmode_r);

	str_rc expire_str = fgets(fin,28);
	str_rc dlen_str = fgets(fin,28);

	zend_long expire;
	bool result = false;
	zend_long now = time(nullptr);

	if ( expire_str.getLong(expire) && expire < now)
	{
		result = true;
	}
	return result;
}

struct ExpiredCollect {
public:
	str_rc 	 root_dir_;
	str_rc 	 extn_;
	htab_rc  expired_;
	zend_long now_;
	bool     recurse_;

	fn_call  dit_valid;
	fn_call  dit_current;
	fn_call  dit_gettype;
	fn_call  dit_key;
	fn_call  dit_getpath;
	fn_call  dit_getchildren;
	fn_call  dit_next;
	fn_call  cur_getExt;

protected:
	obj_rc newIterator(str_ptr root)
	{
		htab_rc args_array;
		htab_rw args(args_array);

		args.push_back(root);
		args.push_back(SFDi.get_fsi_flags());

		obj_rc dit = ReflectCache::staticInstanceArgs(
						SFDi.dir_iterator, args);

		return dit;
	}

public:
	ExpiredCollect()
	{
		dit_valid.set_fci(SFDi.valid_s);
		dit_current.set_fci(SFDi.current_s);
		dit_gettype.set_fci(SFDi.gettype_s);	
		dit_key.set_fci(SFDi.key_s);
		dit_getpath.set_fci(SFDi.getpath_s);
		dit_getchildren.set_fci(SFDi.getchildren_s);
		dit_next.set_fci(SFDi.next_s);
		cur_getExt.set_fci(SFDi.getextension_s);

	}

	void setdirit(obj_ptr dit)
	{
		dit_valid.set_obj(dit);
		dit_current.set_obj(dit);
		dit_gettype.set_obj(dit);
		dit_key.set_obj(dit);
		dit_getpath.set_obj(dit);
		dit_getchildren.set_obj(dit);
		dit_next.set_obj(dit);
	}



	htab_rc collect(str_ptr root, str_ptr extn, bool recurse=false)
	{
		now_ = time(nullptr);
		
		extn_ = extn;
		root_dir_ = root;
		recurse_ = recurse;

		expired_ = htab_rc();

		obj_rc dit = this->newIterator(root);

		recurseDir(dit);

		return expired_;
	}

	

	int recurseDir(obj_ptr dit)
	{
		int result = 0;

		setdirit(dit);

		htab_rw files(expired_);
	
		fn_noparams fvalid(dit_valid);
		bool more = fvalid.zbool();

		while(more)
		{
			fn_noparams fgettype(dit_gettype);
			str_rc dtype = fgettype.str();

			if (zs_cmp_ci(dtype,SFDi.file_s)==0)
			{
				fn_noparams fcurr(dit_current);
				obj_rc cur = fcurr.obj();

				cur_getExt.set_obj(cur);

				fn_noparams fext(cur_getExt);
				str_rc ext = fext.str();

				if (zs_cmp_ci(ext, extn_)==0)
				{
					fn_noparams fkey(dit_key);
					str_rc path = fkey.str();

					if (is_expired(path))
					{
						files.push_back(path);
						result += 1;
					}
				}
			}
			else if (recurse_ && (zs_cmp_ci(dtype, SFDi.dir_s)==0))
			{
				fn_noparams fchildren(dit_getchildren);

				obj_rc chdit = fchildren.obj(); 

				result += recurseDir(chdit);
				//!! Recursion changes the dit object
				setdirit(dit); 
			}
			fn_noparams  fnext(dit_next);
			fnext.call_fn();

			more = fvalid.zbool();
		} 
		return result;
	}


};

error_return 
DirCache::flushCached()
{
	error_return result;

	htab_rc cached = ICache::getUnsaved();

	htab_walk wk;
	auto value = wk.value();

	for(wk.start(cached); wk.ok(); wk.next())
	{
		obj_rc pkg = value.zobject();
		if (pkg.ok() && !this->writePkg(pkg))
		{
			ICacheData* icd = zobj_toc<ICacheData>(pkg);
			str_rc key = icd->getKey();
			result.error() << "WritePkg failed for " << key << endl;
		}
	}
	return result;
}

bool 
DirCache::clear()
{
	bool result = false;

	obj_rc dos = this->getService(SFDi.dos_str);

	obj_rc sess = this->getService(SFDi.user_session);

	val_rc path(cache_dir_);
	val_rc depth(int(0));

	val_rc ctr = dos.call(SFDi.rm_alldir,  path, depth);
	int ct = ctr.zlong();

	result = (ct > 0);
	str_buf buf;

	if (sess.ok())
	{
		buf << "cleared " << ct << " from " << cache_dir_;
		val_rc arg1(buf.zstr());

		sess.call(SFDi.flash_s, arg1);
	}
	return result;
}

htab_rc //static
DirCache::getExpiredFiles(str_ptr dir, str_ptr ext)
{
	ExpiredCollect collect;

	return collect.collect(dir, ext, dir_tree_);
}

int  
DirCache::deleteExpired()
{
	int result  = 0;
	
	ICache::deleteExpired();

	htab_rc expired = getExpiredFiles(cache_dir_, SFDi.sfile_ext);
	result = expired.size();

	htab_walk wk;

	auto path = wk.value();
	for(wk.start(expired); wk.ok(); wk.next())
	{
		unlink(path.zstr());
	}

	return result;
}



val_rc 
DirCache::getCached(str_ptr key)
{
	val_rc result;
	
	if (keep_local_)
	{
		result = ICache::getCached(key);
		if (result.isObject())
		{
			return result;
		}
	}
	int now = time(nullptr);

	str_rc file_name = this->getFileName(key);
	if (!is_file(file_name) || !is_readable(file_name))
	{
		return result;
	}

	file_res fin(file_name, SFDi.fmode_r);

	str_rc expire_str = fgets(fin,28);
	str_rc dlen_str = fgets(fin,28);

	zend_long expire;

	if (expire_str.getLong(expire) && expire > now)
	{
		zend_long dlen;
		

		if (dlen_str.getLong(dlen))
		{	
			str_rc fdata = fread(fin, dlen+3);
			if (fdata.ok())
			{
				result = unserialize(fdata);
			}
		}
	}
	return result;
}

str_rc
DirCache::getDirectory(str_ptr id)
{
    str_rc hash = sha1(id, false);
    
    str_ptr result = cache_dir_;
    
    if (dir_tree_)
    {
    	str_buf buf;

    	str_rc f1 = hash.substr(0,2);
    	str_rc f2 = hash.substr(2,2);

    	buf << result << '/' << f1 << '/' << f2;
    	result = buf.zstr();
        
    }
    return result;
}


str_rc
DirCache::getFileName(str_ptr id)
{
    str_rc dir = this->getDirectory(id);
    str_rc hash = sha1(id, false);

    str_buf buf;
    buf << dir << DIRECTORY_SEPARATOR << hash << '.' << SFDi.sfile_ext;

    str_rc result = buf.zstr();
    return result;
}

bool 
DirCache::set(str_ptr key, val_ptr data, zend_long ttl)
{
	if (ttl <= 0)
	{
		ttl = this->getTTL();
	}

	obj_rc pkg = ICacheData::omg.new_zobj();
	ICacheData* icd = zobj_toc<ICacheData>(pkg);
	icd->construct(key, data, ttl);

	if (keep_local_)
	{
		this->addLocal(pkg);
	}

	bool defer_write = (keep_local_) ? this->defer_write_ : false; 

	if (defer_write)
	{
		return true;
	}

	return this->writePkg(pkg);
}

val_rc 
DirCache::get(str_ptr key, val_ptr noval)
{
	val_rc result;

	//showstr("get key", key);

	val_rc dataobj = this->getCached(key);

	if (dataobj.isObject())
	{
		ICacheData* icdata = zval_toc<ICacheData>(dataobj); 
		result  = icdata->getData();
	}

	return result;
}

bool 
DirCache::deleteKey(str_ptr key)
{
	str_rc file = this->getFileName(key);
	if (file_exists(file))
	{
		return unlink(file);
	}
	return true;
}

bool DirCache::writePkg(obj_ptr pkg)
{
	ICacheData* icd = zobj_toc<ICacheData>(pkg);

	str_rc key = icd->getKey();

	str_rc dir = this->getDirectory(key);

	if (!is_dir(dir))
	{
		if (!mkdir(dir, 0755, true))
		{
			return false;
		}
	}

	str_rc file_name = this->getFileName(key);

	icd->setStored();

	int expiry = icd->getExpiry();

	val_rc sarg(pkg);
	str_rc sbin = serialize(sarg);

	size_t plen = sbin.size();

	val_rc fout = zpp::fopen(file_name, SFDi.fmode_w);

	str_buf buf;
	buf << expiry << endl;
	fwrite(fout, buf.zstr());

	buf << plen << endl;
	fwrite(fout, buf.zstr());

	fwrite(fout, sbin);

	fclose(fout);

	return true;

}

zend_class_entry* 
DirCache::register_class(zend_class_entry* ce)
{
	zend_class_entry *sf = register_class_Wcc_Cache_DirCache(ce);

	DirCache::omg.classEntry(sf);

	return sf;
}

void 
DirCache::debug_info(htab_rw s)
{
	ICache::debug_info(s);

	s.set(SFDi.opt_cachedir, cache_dir_);
	s.set(SFDi.opt_keeplocal, keep_local_);
	s.set(SFDi.opt_dirtree, dir_tree_);
	s.set(SFDi.opt_deferwrite, defer_write_);
}

PHP_MINIT_FUNCTION(Wcc_DirCache_reg)
{
	DirCache::register_class(ICache::omg.classEntry());
	STATE_INIT_ADD(SFDi);
	return SUCCESS;
}

}//namespace wcd


#endif
