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
	di.set(RQ_STR.error_key, error_);
	di.set(RQ_STR.namekey, name_);
	di.set(RQ_STR.ext_key, ext_);
	di.set(RQ_STR.key_key, key_);
	di.set(RQ_STR.realtype_key, realType_);
	di.set(RQ_STR.typekey, type_);
	di.set(RQ_STR.tmp_name, tmp_name_);
	di.set(RQ_STR.size_key, (int)size_);
}

void 
FileUpload::construct(htab_read file_data, zstr_user name)
{
	htab_ptr file(file_data);

	name_ = file[RQ_STR.namekey];

	if (name_.size())
	{
		ext_ = name_.pathinfo(Path::EXTENSION);
	}
	else {
		name_ = zend_empty_string;
		ext_ = zend_empty_string;
	}

	tmp_name_ = file[RQ_STR.tmp_name];
	size_ = zval_ptr(file[RQ_STR.size_key]).zlong();
	type_ = file[RQ_STR.typekey];
	error_ = file[RQ_STR.error_key];
	key_ = file[RQ_STR.key_key];
}

zstr_mgr 
FileUpload::getRealType()
{
	zstr_make<true> finfo_open("finfo_open");
	zval_own arg(FILEINFO_MIME_TYPE);

	zval_own finfo = finfo_open.callme(arg);

	if (!finfo.isResource())
	{
		return zstr_own(zend_empty_string);
	}
	zstr_make<true> finfo_file("finfo_file");
	
	zval_own arg2(tmp_name_);
	zstr_own mime = finfo_file.callme(finfo, arg2);
	
	zstr_make<true> finfo_close("finfo_close");
	finfo_close.callme(finfo);

	return mime;
}

bool 
FileUpload::isUploadedFile()
{
	if (tmp_name_.size()) {
		zval_own tmp(tmp_name_);
		zstr_make<true> is_uploaded("is_uploaded_file");
		return is_uploaded.callme(tmp).isTrue();
	}
	return false;
}

bool 
FileUpload::moveTo(const zstr_base& destination)
{
	if (tmp_name_.size()) {
		zval_own tmp(tmp_name_);
		zstr_make<true> move_uploaded("move_uploaded_file");
		zval_own dest(destination);
		return move_uploaded.callme(tmp, dest).isTrue();
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
	zstr_own result = fobj->getName();
	result.move_zv(return_value);
}
ZEND_METHOD(Wcc_FileUpload, getKey)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	FileUpload* fobj = zval_toc<FileUpload>(ZEND_THIS);
	zstr_own result = fobj->getKey();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_FileUpload, getRealType)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	FileUpload* fobj = zval_toc<FileUpload>(ZEND_THIS);
	zstr_own result = fobj->getRealType();
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
	zstr_own result = fobj->getTempName();
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_FileUpload, getType)
{
	ZEND_PARSE_PARAMETERS_START(0,0)
	ZEND_PARSE_PARAMETERS_END();

	FileUpload* fobj = zval_toc<FileUpload>(ZEND_THIS);
	zstr_own result = fobj->getType();
	result.move_zv(return_value);
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
	zstr_own result = fobj->getError();
	result.move_zv(return_value);

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