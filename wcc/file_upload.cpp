#ifndef FILE_UPLOAD_CPP
#define FILE_UPLOAD_CPP

#ifndef FILE_UPLOAD_H
#include "file_upload.h"
#endif

#ifndef REQUEST_GLOBALS_ARGINFO
#define REQUEST_GLOBALS_ARGINFO
extern "C" {
	#include "stub/request_globals_arginfo.h"
};
#endif

#ifndef REQUEST_GLOBALS_H
#include "request_globals.h"
#endif

namespace wcc {
	base_obj_mgr<FileUpload> FileUpload::omg;

using namespace zpp;

void FileUpload::debug_info(htab_write di)
{
	di.set(RQit.error_key, error_);
	di.set(RQit.namekey, name_);
	di.set(RQit.ext_key, ext_);
	di.set(RQit.key_key, key_);
	di.set(RQit.realtype_key, realType_);
	di.set(RQit.typekey, type_);
	di.set(RQit.tmp_name, tmp_name_);
	di.set(RQit.size_key, (int)size_);
}

void 
FileUpload::construct(htab_read file_data, zstr_user name)
{
	name_ = file_data.get(RQit.namekey);

	if (name_.size())
	{
		zval_mgr info = FTAB.pathinfo.call(name_,(PathInfo::EXTENSION));
		ext_ = zval_user(info).zstr();
	}
	else {
		name_ = zend_empty_string;
		ext_ = zend_empty_string;
	}

	tmp_name_ = file_data.get(RQit.tmp_name);
	size_ = zval_user(file_data.get(RQit.size_key)).zlong();
	type_ = file_data.get(RQit.typekey);
	error_ = file_data.get(RQit.error_key);
	key_ = file_data.get(RQit.key_key);
}

zstr_mgr 
FileUpload::getRealType()
{
	zstr_mgr result;

	zval_mgr finfo = finfo_open(FILEINFO_MIME_TYPE);
	if (!zval_user(finfo).ok())
	{
		result = zstr_empty();
		return result;
	}
	result = finfo_file(finfo, tmp_name_);
	finfo_close(finfo);

	return result;
}

bool 
FileUpload::isUploadedFile()
{
	if (tmp_name_.size()) {
		return is_uploaded_file(tmp_name_);
	}
	return false;
}

bool 
FileUpload::moveTo(zstr_user destination)
{
	if (tmp_name_.size()) {
		return move_uploaded_file(tmp_name_, destination);
	}
	return false;
}

}; // end namespace


ZEND_METHOD(Wcc_FileUpload, __construct)
{
	zval* fdata;
	zend_string* fkey;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_ARRAY(fdata)
	Z_PARAM_STR(fkey)
	ZEND_PARSE_PARAMETERS_END();

	FileUpload* fobj = zval_toc<FileUpload>(ZEND_THIS);
	fobj->construct(fdata, fkey);
}

ZEND_METHOD(Wcc_FileUpload, getName)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	FileUpload* fobj = zval_toc<FileUpload>(ZEND_THIS);
	zstr_user result = fobj->getName();
	result.return_zv(return_value);
}
ZEND_METHOD(Wcc_FileUpload, getKey)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	FileUpload* fobj = zval_toc<FileUpload>(ZEND_THIS);
	zstr_user result = fobj->getKey();
	result.return_zv(return_value);
}

ZEND_METHOD(Wcc_FileUpload, getRealType)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	FileUpload* fobj = zval_toc<FileUpload>(ZEND_THIS);
	zstr_mgr result = fobj->getRealType();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_FileUpload, getSize)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	FileUpload* fobj = zval_toc<FileUpload>(ZEND_THIS);
	zend_long result = fobj->getSize();
	RETURN_LONG(result);
}

ZEND_METHOD(Wcc_FileUpload, getTempName)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	FileUpload* fobj = zval_toc<FileUpload>(ZEND_THIS);
	zstr_user result = fobj->getTempName();
	result.return_zv(return_value);
}

ZEND_METHOD(Wcc_FileUpload, getType)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	FileUpload* fobj = zval_toc<FileUpload>(ZEND_THIS);
	zstr_user result = fobj->getType();
	result.return_zv(return_value);
}

ZEND_METHOD(Wcc_FileUpload, isUploadedFile)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	FileUpload* fobj = zval_toc<FileUpload>(ZEND_THIS);
	bool result = fobj->isUploadedFile();
	RETURN_BOOL(result);
}

ZEND_METHOD(Wcc_FileUpload, getError)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	FileUpload* fobj = zval_toc<FileUpload>(ZEND_THIS);
	zstr_user result = fobj->getError();
	result.return_zv(return_value);

}

ZEND_METHOD(Wcc_FileUpload, moveTo)
{
	zend_string* destination;
	
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(destination)
	ZEND_PARSE_PARAMETERS_END();

	FileUpload* fobj = zval_toc<FileUpload>(ZEND_THIS);
	bool result = fobj->moveTo(destination);
	RETURN_BOOL(result);
}

PHP_MINIT_FUNCTION(FileUpload_reg)
{
	auto ce = register_class_Wcc_FileUpload();

	FileUpload::omg.classEntry(ce);

	return SUCCESS;
}


//file_upload.cpp
#endif