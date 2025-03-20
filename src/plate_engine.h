#ifndef  PLATE_ENGINE_H
#define  PLATE_ENGINE_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


#include <filesystem>
namespace fs = std::filesystem;

namespace wcc {


class Plate; // forward


	class PlateEngine : public base_d {
	protected:
		zobj_mgr			    search_; // SearchList object

		htab_mgr   				shared_data_;
		htab_mgr				plates_data_;

		htab_mgr            	extensions_;

		htab_mgr              	stored_; // stored template objects

		htab_mgr              	functions_; //stored Callable

		bool                    doLabel_;

		zobj_mgr                loadintf_; // name of class

	public:

		static base_obj_mgr<PlateEngine> omg;

		virtual void debug_info(htab_write hw);

		void store(zstr_user name, zobj_user plate);
	

		void setExtensions(zval_user ext);
		zval_mgr getExtensions();

		void setFinder(zobj_user pathobj);
		zobj_user getFinder();
		
		void setLabel(bool value);
		bool getLabel();
		
		// output buffering fn
		void 	  setLoadHtml(zobj_user obj);
		zobj_user getLoadHtml();

		void mergePlateData(htab_read data, zstr_user name);
		void shareWithAll(htab_read data);
		void shareData(htab_read data, zval_user templates);

		htab_mgr getData(zstr_user name);

		zstr_mgr find(zstr_user name);
		zstr_mgr dumpPaths();
		
		void registerFunction(zstr_user name, zval_user callback);
		zval_mgr getFunction(zstr_user name);

		zobj_mgr    newPlate(zstr_user name, bool store=false);
		zobj_mgr 	getPlate(zstr_user name);
		void		storePlate(zobj_user plate);

		//zobj_mgr makeRaw(zstr_user name, zstr_user raw, bool store = false);

		zstr_mgr render(zstr_user name, htab_read data);

		void clearPlates();
		void clearPaths();
		
		static zstr_mgr fileLabel(zstr_user file);

		friend class Plate;

	};

	extern zend_class_entry* gIfLoadHtmlCE;
}; // namespace Wcc


#endif