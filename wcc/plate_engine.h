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
		obj_rc			    search_; // SearchList object

		htab_rc   				shared_data_;
		htab_rc				plates_data_;

		htab_rc            	extensions_;

		htab_rc              	stored_; // stored template objects

		htab_rc              	functions_; //stored Callable

		bool                    doLabel_;

		obj_rc                loadintf_; // name of class

	public:

		static base_obj_mgr<PlateEngine> omg;

		virtual void debug_info(htab_rw hw);

		void store(str_ptr name, obj_ptr plate);
	

		void setExtensions(val_ptr ext);
		val_rc getExtensions();

		void setFinder(obj_ptr pathobj);
		obj_ptr getFinder();
		
		void setLabel(bool value);
		bool getLabel();
		
		// output buffering fn
		void 	  setLoadHtml(obj_ptr obj);
		obj_ptr getLoadHtml();

		void mergePlateData(htab_rd data, str_ptr name);
		void shareWithAll(htab_rd data);
		void shareData(htab_rd data, val_ptr templates);

		htab_rc getData(str_ptr name);

		str_rc find(str_ptr name);
		str_rc dumpPaths();
		
		void registerFunction(str_ptr name, val_ptr callback);
		val_rc getFunction(str_ptr name);

		obj_rc    newPlate(str_ptr name, bool store=false);
		obj_rc 	getPlate(str_ptr name);
		void		storePlate(obj_ptr plate);


		//obj_rc makeRaw(str_ptr name, str_ptr raw, bool store = false);

		str_rc render(str_ptr name, htab_rd data);

		void clearPlates();
		void clearPaths();
		
		static str_rc fileLabel(str_ptr file);

		friend class Plate;

		VIRTUAL_ZOBJPTR

	};

	extern zend_class_entry* gIfLoadHtmlCE;
}; // namespace Wcc


#endif