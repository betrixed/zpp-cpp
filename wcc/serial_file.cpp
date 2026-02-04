#ifndef SERIAL_FILE_CPP
#define SERIAL_FILE_CPP

#ifndef SERIAL_FILE_H
#include "serial_file.h"
#endif


#ifndef SERIALFILE_ARGINFO
#define SERIALFILE_ARGINFO
extern "C" {
	#include "stub/serialfile_arginfo.h"
};
#endif

#ifndef WCC_RUNSA_H
#include "run.h"
#endif

namespace wcc {

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

	class_data cdata(file_iterator);

	val_rc temp = cdata.constant_value(skip_dots);
	file_itflags = temp.zlong();
	temp = cdata.constant_value(key_as_pathname);
	file_itflags |= temp.zlong();
	temp = cdata.constant_value(current_as_fileinfo);
	file_itflags |= temp.zlong();
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
			obj_ptr run = test.zobject();
			cache_dir_ = run.property(Run_i.temp_dir);
		}
	}
}

struct ExpiredCollect {
public:
	str_rc 	 root_dir_;
	str_rc 	 extn_;
	htab_rc  expired_;
	zend_long now_;

	fn_call  dit_valid;
	fn_call  dit_current;
	fn_call  dit_gettype;
	fn_call  dit_key;
	fn_call  dit_getpath;
	fn_call  dit_getchildren;
	fn_call  dit_next;

	fn_call  cur_getExt;


	ExpiredCollect()
	{
		dit_valid.set_fname(SFDi.valid_s);
		dit_current.set_fname(SFDi.current_s);
		dit_gettype.set_fname(SFDi.gettype_s);	
		dit_key.set_fname(SFDi.key_s);
		dit_getpath.set_fname(SFDi.getpath_s);
		dit_getchildren.set_fname(SFDi.getchildren_s);
		dit_next.set_fname(SFDi.next_s);

		cur_getExt.set_fname(SFDi.getextension_s);

	}

	void setdirit(obj_ptr dit)
	{
		dit_valid.set_obj(dit);
		dit_current.set_obj(dit);
		dit_gettype.set_obj(dit);
		dit_key.set_obj(dit);
		dit_getpath.set_obj(dit);
		dit_getchildren.set_obj(dit);
	}

	htab_rc collect(str_ptr root, str_ptr extn)
	{
		now_ = time(nullptr);
		htab_rc args_array;

		htab_rw args(args_array);
		root_dir_ = root;
		extn_ = extn;

		args.push_back(root);
		args.push_back(SFDi.file_itflags);

		obj_rc dit = ReflectCache::staticInstanceArgs(
						SFDi.dir_iterator, args);

		expired_ = htab_rc();

		recurseDir(dit);

		return expired_;
	}

	int recurseDir(obj_ptr dit)
	{
		int result = 0;

		setdirit(dit);

		htab_rw files(expired_);
	
		val_rc is_valid = dit_valid.call_fn();

		while(is_valid.isTrue())
		{
			str_rc dtype = dit_gettype.call_fn();

			if (zs_cmp_ci(dtype,SFDi.file_s)==0)
			{
				obj_rc cur = dit_current.call_fn();
				cur_getExt.set_obj(cur);
				str_rc ext = cur_getExt.call_fn();
				if (zs_cmp_ci(ext, extn_)==0)
				{
					str_rc path = dit_key.call_fn();
					if (is_expired(path))
					{
						files.push_back(path);
						result += 1;
					}
				}
			}
			else if (zs_cmp_ci(dtype, SFDi.dir_str)==0)
			{
				obj_rc chdit = dit_getchildren.call_fn(); 
				result += recurseDir(chdir);
			}
			dit_next.call_fn();
			is_valid = dit_valid.call_fn();
		} 
		return result;
	}


};

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

static zend_long 
RecurseDir(obj_ptr dirit, str_ptr extension, htab_rw expired)
{
	zend_long now = time(nullptr);
}

htab_rc //static
SerialFile::getExpiredFiles(str_ptr dir, str_ptr ext)
{
	ExpiredCollect collect;

	return collect.collect(dir, ext);
}

int  
SerialFile::deleteExpired()
{
	int result  = 0;
	this->icache::deleteExpired();

	htab_rc expired = getExpiredFiles(cache_dir_, SFDi.sfile_ext);
	result = expired.size();

	htab_walk wk;

	auto path = wk.value();
	for(wk.start(expired); wk.ok(); wk.next())
	{
		unlink(path);
	}

	return result;
}

static bool toLong(str_ptr s, zend_long& ref)
{
	int slen = s.size();
	if (s == 0)
	{
		return false;
	}

	const char* p = s.data();
	char* endptr;
	ref = strtol(p, &endptr, 10);
	return (endptr > p);
}

static bool is_expired(str_ptr file)
{

}

val_rc 
SerialFile::getCached(str_ptr key)
{
	val_rc result;

	if (keeplocal_)
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
	val_rc fin = fopen(file_name, SFDi.fmode_r);

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

bool 
SerialFile::set(str_ptr key, val_ptr data, zend_long ttl = 0)
{
	if (ttl <= 0)
	{
		ttl = this->getTTL();
	}

	obj_rc pkg = ICacheData::obm.new_zobj();
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
SerialFile::get(str_ptr key, val_ptr noval = val_ptr())
{
	val_rc result;

	val_rc dataobj = this->getCached(key);

	if (dataobj.isObject())
	{
		ICacheData* icdata = zval_toc<ICacheData>(dataobj); 
		result  = icdata->getData();
	}

	return result;
}

bool 
SerialFile::deleteKey(str_ptr key)
{
	str_rc file = this->getFileName(key);
	if (file_exists(file))
	{
		return unlink(file);
	}
	return true;
}

bool SerialFile::writePkg(obj_ptr pkg)
{
	str_rc key = pkg->getKey();

	str_rc dir = this->getDirectory(key);

	if (!is_dir(dir))
	{
		if (!mkdir(dir, 0755, true))
		{
			return false;
		}
	}

	str_rc file_name = this->getFileName(key);

	ICacheData* ic = zobj_toc<ICacheData>(pkg);

	ic->setStored();

	int expiry = ic->getExpiry();

	str_rc sbin = serialize(pkg);

	size_t plen = sbin.size();

	val_rc fout = fopen(file_name, SFDi.fmode_w);

	sbuf buf;
	buf << expiry << endl;
	fwrite(fout, buf.zstr());

	buf << plen << endl;
	fwrite(fout, buf.zstr());

	fwrite(fout, sbin);

	fclose(fout);

	return true;

}

zend_class_entry* 
SerialFile::register_class(zend_class_entry* ce)
{
	zend_class_entry *sf = register_class_Wcc_Cache_SerialFile(ce);

	SerialFile::omg.classEntry(sf);
	return
}

void 
SerialFile::debug_info(htab_rw s)
{
	ICache::debug_info(s);

	s.set(SFDi.opt_cachedir, cache_dir_);
	s.set(SFDi.opt_keeplocal, keep_local_);
	s.set(SFDi.opt_dirtree, dir_tree_);
	s.set(SFDi.opt_deferwrite, defer_write_);
}


}//namespace wcd


#endif
