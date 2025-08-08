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
		str_rc error_;
		str_rc name_;
		str_rc ext_;
		str_rc key_;
		str_rc realType_;
		str_rc tmp_name_;
		str_rc type_;
		zend_long size_;
	public:
		static base_obj_mgr<FileUpload> omg;

		virtual void debug_info(htab_rw hw);
		
		void construct(htab_rd file_data, str_ptr name);

		str_rc getRealType();

		bool isUploadedFile();

		bool moveTo(str_ptr destination);

		str_ptr getName()
		{
			return name_;
		}

		str_ptr getKey()
		{
			return key_;
		}

		zend_long getSize() 
		{
			return size_;
		}

		str_ptr getTempName()
		{
			return tmp_name_;
		}

		str_ptr getType() 
		{
			return type_;
		}

		str_ptr getError()
		{
			return error_;
		}
	};

};

//file_upload.h
#endif