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
		htab_rc levels_;
		htab_rc values_;
		obj_rc model_;
		str_rc model_svc_;
	public:
		static base_obj_mgr<HtmlPlates> omg;

		virtual void debug_info(htab_wr hw);

		void construct(str_ptr model_id);

		void initValues();

		/**
		 * Push from inner to outer
		 */
		void pushLevel(str_ptr name);

		obj_rc getModel();

		void setModel(obj_ptr model);

		void mergeData(htab_rd items);

		str_rc render(htab_rd options);

		str_rc renderView(htab_wr options);

		VIRTUAL_ZOBJPTR
	};

}; //namespace
//htmlplates.h
#endif