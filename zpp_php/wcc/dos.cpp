#ifndef WCC_DOS_CPP
#define WCC_DOS_CPP

#ifndef WCC_DOS_H
#  include "dos.h"
#endif

#ifndef DOS_ARGINFO_H
#define DOS_ARGINFO_H
extern "C" {
	# include "stub/dos_arginfo.h"
}
#endif

namespace wcc {
using namespace zpp;

base_obj_mgr<Dos> Dos::omg;

void 
Dos::construct(str_ptr cwd)
{
	cwd_  = cwd.ok() ? cwd : getcwd();
}


error_return 
Dos::clean_dir(str_ptr path)
{
	error_return result;
	if (file_exists(path) && is_dir(path))
	{
		int_return test = rm_alldir(path);
		if (test.has_errors())
		{
			result = test.move_error();
		}
	}
	else {
		Dos::make_dir(path);
	}
	return result;
}


static bool std_dotfile(str_ptr fname)
{
	unsigned nsize = fname.size();
	const char* cp = fname.data();
	switch(nsize)
	{
	case 0:
		return true;
	case 1:
		if (cp[0]=='.') {
			return true;
		}
		break;
	case 2:
		if ((cp[0]=='.')&&(cp[1]=='.'))
		{
			return true;
		}
		break;
	default:
		break;
	}
	return false;
}

error_return 
Dos::copy_all(str_ptr from, str_ptr to)
{
	error_return result;

	val_rc dh = opendir(from);
	Dos::make_dir(to);

	htab_rc dirlist;
	htab_rc filelist;

	htab_rw dlist(dirlist);
	htab_rw flist(filelist);
	str_buf buf;

	while(true)
	{
		val_rc file = readdir(dh);
		if (file.isFalse())
		{
			break;
		}
		str_ptr fname = file.zstr();
		if (std_dotfile(fname))
		{
			continue;
		}
		buf << from << '/' << fname;
		str_rc src = buf.zstr();
		buf << to << '/' << fname;
		str_rc dest = buf.zstr();

		if (is_dir(src)) {
			result = copy_all(src, dest);
			if (result.has_errors())
			{
				break;
			}
		} 
		else {
			bool_return test = Dos::copy_file(src, dest);

			if (test.has_errors())
			{
				result = test.move_error();
				return result;
			}
		}
	}
	return result;
}


bool_return
Dos::copy_file(str_ptr from, str_ptr to)
{
	bool_return result;
	str_rc one = realpath(from);
	if (!one.ok()) {
		result.error() << "Cannot find source file " << from;
		result.value_ = false;
		return result;
	}
	str_return two = this->real_filepath(to);

	if (two.has_errors())
	{
		result = two.move_error();
		return result;
	}
	result.value_ = zpp::copy_file(one, two.value_);
	return result;
}

str_rc 
Dos::get_cwd()
{
	return zpp::getcwd();
}


bool 
Dos::make_dir(str_ptr path, int permissions)
{
	if (is_dir(path))
	{
		return true;
	}
	return zpp::mkdir(path, permissions, true);
}


str_return
Dos::real_dirpath(str_ptr rpath)
{
	str_return result;

	str_buf buf;
	str_rc  temp;
	if (rpath.find('/')==0)
	{
		temp = rpath;
	}
	else {
		buf << cwd_ << '/' << rpath;
		temp = buf.zstr();
	}
	str_rc test = realpath(temp);
	if (!test.ok())
	{
		val_rc pinfo = pathinfo(rpath, PathInfo::ALL);
		htab_ptr pdata(pinfo.zarray());

		str_rc dname = pdata.get(FTAB.s_dirname);
		str_rc bname = pdata.get(FTAB.s_basename);

		if (!dname.ok() || !bname.ok())
		{
			result.error() << "Cannot ensure directory from " << rpath;
			return result;
		}
		if (dname.find('/') == 0)
		{
			temp = dname;
		}
		else {
			buf << cwd_ << '/' << dname;
			temp = buf.zstr();
		}
		if (!Dos::make_dir(temp))
		{
			result.error() << "Cannot make directory " << temp;
			return result;
		}
		buf << temp << '/' << bname;
		result.value_ = buf.zstr();
	}
	else {
		result.value_ = test;
	}
	return result;

}


str_return 
Dos::real_filepath(str_ptr rpath)
{
	str_return result;
	str_buf buf;

	str_rc test = realpath(rpath);
	if (!test.ok())
	{
		val_rc pinfo = pathinfo(rpath, PathInfo::ALL);
		htab_ptr pdata(pinfo.zarray());

		str_rc dname = pdata.get(FTAB.s_dirname);
		str_rc bname = pdata.get(FTAB.s_basename);

		if (!dname.ok() || !bname.ok())
		{
			result.error() << "Cannot ensure file path from " << rpath;
			return result;
		}
		if (dname.find('/')==0)
		{
			test = dname;
		}
		else  {
			buf << cwd_ << '/' << dname;
			test = buf.zstr();
		}
		if (!Dos::make_dir(test))
		{
			result.error() << "Cannot make dir " << test;
			return result;
		}
		buf << test << '/' << bname;
		test = buf.zstr();
	}
	result.value_ = test;
	return result;
}


bool_return 
Dos::rm_all(htab_ptr flist)
{
	bool_return result;

	htab_walk wk;
	for(wk.start(flist); wk.ok(); wk.next())
	{
		str_rc fname(wk.value());
		if (is_file(fname) || is_link(fname)) {
			if (!unlink(fname))
			{
				result.error() << "Failed to unlink " << fname;
				return result;
			}
		}
	}
	result.value_ = true;
	return result;
}


int_return  
Dos::rm_alldir(str_ptr path, bool deldir)
{
	int_return result;

	unsigned nsize = path.size();
	if (nsize==0) {
		result.error() << "Empty path value";
		return result;
	}
	if (!is_dir(path))
	{
		result.error() << "Path " << path << " not a directory";
		return result;
	}

	str_rc dpath;
	str_buf buf;

	if (!path.ends_with('/')) 
	{
		buf << path << '/';
		dpath = buf.zstr();
	}
	else {
		dpath = path;
	}
	val_rc dh = opendir(dpath);

	if (dh.isFalse())
	{
		result.error() << "Unable to list directory " << dpath;
		return result;
	}
	
	str_rc fpath;

	htab_rc dlist, flist;
	htab_rw dirlist(dlist);
	htab_rw filelist(flist);

	while( true ) 
	{
		val_rc hfile = readdir(dh);
		str_rc fname = hfile.zstr();
		if (hfile.isFalse())
		{
			break;
		}
		if (std_dotfile(fname))
		{
			continue;
		}
		buf << dpath << fname;
		fpath = buf.zstr();
		if (is_dir(fpath)) {
			dirlist.push_back(fname);
		}
		else {
			filelist.push_back(fname);
		}
	}
	closedir(dh);

	htab_walk wk;
	int ix = 0;
	for(wk.start(flist); wk.ok(); wk.next())
	{
		buf << dpath << wk.value().zstr();
		fpath = buf.zstr();
		unlink(fpath);
		ix++;
	}
	for(wk.start(dlist); wk.ok(); wk.next())
	{
		buf << dpath << wk.value().zstr();
		fpath = buf.zstr();
		int_return test = rm_alldir(fpath, true);
		if (test.has_errors())
		{
			result = test.move_error();
			return result;
		}
		ix += test.value_;
	}
	if (deldir)
	{
		if (rmdir(dpath)) ix++;
	}
	result.value_ = ix;
	return result;
}


int 
Dos::rm_old(htab_ptr flist, int tsecs)
{
	zend_long now = time(nullptr);
	int ct = 0;

	htab_walk wk;

	for(wk.start(flist); wk.ok(); wk.next())
	{
		str_rc file = wk.value().zstr();

		if (is_file(file))
		{
			zend_long mtime = filemtime(file);
			if (now - mtime > tsecs) {
				if (unlink(file))
				{
					ct++;
				}
			}
		}
	}
	return ct;
}


int_return 
Dos::sync_dir(str_ptr src, str_ptr dest)
{
	int_return result;

	str_rc gstr = src + "/*";

	if (!is_dir(dest) || !make_dir(dest))
	{
		return 0;
	}
	int ct = 0;
	htab_rc flist = glob(gstr);
	htab_walk wk;
	str_buf buf;

	for(wk.start(flist); wk.ok(); wk.next())
	{
		str_rc sfile = wk.value().zstr();
		str_rc bname = basename(sfile);
		buf << dest << '/' << bname;
		str_rc destfile = buf.zstr();
		if (!file_exists(destfile) || (filemtime(destfile) < filemtime(sfile)))
		{
			bool_return test = copy_file(sfile, destfile);
			if (test.has_errors())
			{
				result = test.move_error();
				return result;
			}
			if (test.value_)
			{
				ct++;
			}
		}
	}
	result.value_ = ct;
	return result;
}

}//wcc

using namespace wcc;
using namespace zpp;

ZEND_METHOD(Wcc_Dos, __construct)
{
	zarg_rd args(execute_data);
	str_ptr  cwd = args.str(args.option(0));
	if (!args.throw_errors(__FUNCTION__))
	{
		Dos* cobj = zval_toc<Dos>(ZEND_THIS);
		cobj->construct(cwd);
	}
}

ZEND_METHOD(Wcc_Dos, clean_dir)
{
	zarg_rd args(execute_data);
	str_ptr  path = args.str(args.need(0));
	if (!args.throw_errors())
	{
		Dos* cobj = zval_toc<Dos>(ZEND_THIS);
		error_return result = cobj->clean_dir(path);
		result.throw_errors();
	}
}

ZEND_METHOD(Wcc_Dos, copy_all)
{
	zarg_rd args(execute_data);
	str_ptr  from = args.str(args.need(0));
	str_ptr  to = args.str(args.need(1));
	if (!args.throw_errors(__FUNCTION__))
	{
		Dos* cobj = zval_toc<Dos>(ZEND_THIS);
		cobj->copy_all(from, to);
	}
}

ZEND_METHOD(Wcc_Dos, get_cwd)
{
	if (!zarg_rd::zero_args(execute_data ,__FUNCTION__))
	{
		return;
	}
	Dos* cobj = zval_toc<Dos>(ZEND_THIS);
	str_rc result = cobj->get_cwd();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_Dos, copy_file)
{
	zarg_rd args(execute_data);
	str_ptr  from = args.str(args.need(0));
	str_ptr  to = args.str(args.need(1));
	if (!args.throw_errors(__FUNCTION__))
	{
		Dos* cobj = zval_toc<Dos>(ZEND_THIS);
		bool_return result = cobj->copy_file(from, to);
		if (!result.throw_errors())
		{
			RETURN_BOOL(result.value_);
		}
		
	}

}

ZEND_METHOD(Wcc_Dos, make_dir)
{
	zarg_rd args(execute_data);
	str_ptr  path = args.str(args.need(0));
	zend_long permissions = 0755;

	args.zlong(permissions, args.option(1));
	
	if (!args.throw_errors(__FUNCTION__))
	{
		Dos* cobj = zval_toc<Dos>(ZEND_THIS);
		bool result = cobj->make_dir(path, permissions);
		RETURN_BOOL(result);
	}	
	
}

ZEND_METHOD(Wcc_Dos, real_dirpath)
{
	zarg_rd args(execute_data);
	str_ptr  path = args.str(args.need(0));
	if (!args.throw_errors(__FUNCTION__))
	{
		Dos* cobj = zval_toc<Dos>(ZEND_THIS);
		str_return result = cobj->real_dirpath(path);
		if (!result.throw_errors())
		{
			result.value_.move_zv(return_value);
		}
		
	}

}

ZEND_METHOD(Wcc_Dos, real_filepath)
{
	zarg_rd args(execute_data);
	str_ptr  path = args.str(args.need(0));
	if (!args.throw_errors(__FUNCTION__))
	{
		Dos* cobj = zval_toc<Dos>(ZEND_THIS);
		str_return result = cobj->real_filepath(path);
		if (!result.throw_errors())
		{
			result.value_.move_zv(return_value);
		}
	}
	
}

ZEND_METHOD(Wcc_Dos, rm_all)
{
	zarg_rd args(execute_data);
	htab_ptr  paths = args.htab(args.need(0));
	if (!args.throw_errors(__FUNCTION__))
	{
		Dos* cobj = zval_toc<Dos>(ZEND_THIS);
		bool_return result = cobj->rm_all(paths);
		if (!result.throw_errors())
		{
			RETURN_BOOL(result.value_);
		}
	}
	
}

ZEND_METHOD(Wcc_Dos, rm_alldir)
{
	zarg_rd args(execute_data);
	str_ptr  path = args.str(args.need(0));
	bool delpath = false;

	args.zbool(delpath, args.option(1));
	
	if (!args.throw_errors(__FUNCTION__))
	{
		Dos* cobj = zval_toc<Dos>(ZEND_THIS);
		int_return result = cobj->rm_alldir(path, delpath);
		if (!result.throw_errors())
		{
			RETURN_LONG(result.value_);
		}
	}	
	
}

ZEND_METHOD(Wcc_Dos, rm_old)
{
	zarg_rd args(execute_data);
	zend_long time;
	htab_ptr  flist = args.htab(args.need(0));
	
	args.zlong(time, args.need(1));
	
	if (!args.throw_errors(__FUNCTION__))
	{
		Dos* cobj = zval_toc<Dos>(ZEND_THIS);
		zend_long result = cobj->rm_old(flist, time);
		RETURN_LONG(result);
	}
}

ZEND_METHOD(Wcc_Dos, sync_dir)
{
	zarg_rd args(execute_data);
	str_ptr src = args.str(args.need(0));
	str_ptr dest = args.str(args.need(1));
	
	if (!args.throw_errors(__FUNCTION__))
	{
		Dos* cobj = zval_toc<Dos>(ZEND_THIS);
		int_return result = cobj->sync_dir(src, dest);

		if (!result.throw_errors())
		{
			RETURN_LONG(result.value_);
		}
		
	}
}

ZEND_MINIT_FUNCTION(wcc_dos_reg)
{
	auto ce = register_class_Wcc_Dos();
	Dos::omg.classEntry(ce);

	return SUCCESS;
}

#endif