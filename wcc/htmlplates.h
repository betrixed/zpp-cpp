#ifndef HTML_PLATES_H
#define HTML_PLATES_H

#ifndef WCC_SERVICES_H
#include "services.h"
#endif

namespace wcc {

using namespace zpp;
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
		htab_mgr levels_;
		htab_mgr values_;
		zobj_mgr model_;
		zstr_mgr model_svc_;
	public:
		static base_obj_mgr<HtmlPlates> omg;

		virtual void debug_info(htab_write hw);

		void construct(zstr_user model_id);

		void initValues();

		/**
		 * Push from inner to outer
		 */
		void pushLevel(zstr_user name);

		zobj_mgr getModel();

		void setModel(zobj_user model);

		void mergeData(htab_read items);

		zstr_mgr render(htab_read options);

		zstr_mgr renderView(htab_write options);

		VIRTUAL_ZOBJPTR
	};

}; //namespace
//htmlplates.h
#endif