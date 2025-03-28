#ifndef FILE_UPLOAD_H
#define FILE_UPLOAD_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


namespace wcc {

	enum {
		// from ext/fileinfo/libmagic/magic.h
		FILEINFO_MIME_TYPE = 0x0000010,
	};

	class FileUpload : public base_d 
	{
	protected:
		zstr_mgr error_;
		zstr_mgr name_;
		zstr_mgr ext_;
		zstr_mgr key_;
		zstr_mgr realType_;
		zstr_mgr tmp_name_;
		zstr_mgr type_;
		zend_long size_;
	public:
		static base_obj_mgr<FileUpload> omg;

		virtual void debug_info(htab_write hw);
		
		void construct(htab_read file_data, zstr_user name);

		zstr_mgr getRealType();

		bool isUploadedFile();

		bool moveTo(zstr_user destination);

		zstr_user getName()
		{
			return name_;
		}

		zstr_user getKey()
		{
			return key_;
		}

		zend_long getSize() 
		{
			return size_;
		}

		zstr_user getTempName()
		{
			return tmp_name_;
		}

		zstr_user getType() 
		{
			return type_;
		}

		zstr_user getError()
		{
			return error_;
		}
	};

};

//file_upload.h
#endif