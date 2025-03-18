#ifndef  PLATE_ENGINE_H
#define  PLATE_ENGINE_H

#ifndef WC_BASE_H
#include "wc_base.h"
#endif


#include <filesystem>
namespace fs = std::filesystem;

namespace wcc {


class Wcc_Plate; // forward


	class Wcc_PlateEngine : public base_d {
	protected:
		zobj_own			    finder_; // SearchList object
		htab_own   				shared_data_;
		htab_own				plates_data_;

		htab_own            	extensions_;

		htab_own              	made_; // stored template objects

		htab_own              	functions_; //stored Callable

		bool                    doLabel_;

		zobj_own                loadIntf_; // name of class

		zobj_ptr			    getNamedPath(zend_string* name);
	public:

		static const char* class_name;

		virtual void debug_info(HashTable* ht);

		void __construct();

		void store(zend_string* name, zval_ptr plate);
	

		void setExtensions(zval_ptr ext);
		zval_own getExtensions();

		void setFinder(zval_ptr pathobj);
		zval_own getFinder();
		
		void setLabel(bool value);
		bool getLabel();
		
		// output buffering fn
		void 	 setLoadHtml(zval_ptr obj);
		zobj_own getLoadHtml();

		void mergePlateData(zval_ptr data, zend_string* name);
		void shareWithAll(zval_ptr data);
		void shareData(zval_ptr data, zval_ptr templates);

		htab_own getData(zend_string* name);

		zstr_own find(zend_string* name);
		zstr_own dumpPaths();
		
		void registerFunction(zend_string* name, zval_ptr callback);
		zval_own getFunction(zend_string* name);

		zobj_own make(zend_string* name, bool store = false, zend_string* raw = nullptr);
		zobj_own makeRaw(zend_string* name, zend_string* raw, bool store = false);

		zstr_own render(zend_string* name, htab_ptr data);

		void clearPlates();
		void clearPaths();
		
		static zstr_own fileLabel(zend_string* file);

		friend class Wcc_PlateName;
		friend class Wcc_Plate;

	};

	typedef base_obj_mgr<Wcc_PlateEngine>  Wcc_PlateEngine_Mgr;

	extern Wcc_PlateEngine_Mgr plate_engine_mgr;

	extern zend_class_entry* gIfLoadHtmlCE;
}; // namespace Wcc


#endif