#ifndef HTML_PLATES_H
#define HTML_PLATES_H

#ifndef SERVICE_ACCESS_H
#include "service_access.h"
#endif

namespace wcc {

	/**
	 * Implements an up to 3 tier nested 
	 * templates, from inner to outer. 
	 * 
	 * Holds a "model" object, instance of Wcc\Config
	 * 
	 * Holds a "values" object. 
	 * Array values passed to Wcc\PlateEngine, including
	 * model using key "m".
	 */

	class HtmlPlates : public base_d {
	protected:
		htab_own levels_;
		htab_own values_;
		zobj_own model_;
		zstr_own model_svc_;
		zobj_own services_;

		Wcc_Services* svc_ptr() {
			return zobj_toc<Wcc_Services>(services_.ptr());
		}
	public:
		static const char* class_name;

		virtual void debug_info(HashTable *ht);

		void construct(zstr_ptr model_id);

		void initValues();

		/**
		 * Push from inner to outer
		 */
		void pushLevel(zstr_ptr name);

		zobj_own getModel();

		void setModel(zobj_ptr model);

		void mergeData(htab_ptr items);

		zstr_own render(htab_ptr options);

		zstr_own renderView(htab_ptr options);
	};

	typedef base_obj_mgr<HtmlPlates>  HtmlPlates_Mgr;

	extern HtmlPlates_Mgr htmlplates_mgr;
}; //namespace
//htmlplates.h
#endif