#ifndef  PLATE_WCP_H
#define  PLATE_WCP_H

#ifndef PLATE_ENGINE_H
#include "plate_engine.h"
#endif

#ifndef SERVICE_ACCESS_H
#include "service_access.h"
#endif

namespace wcc {


/**
 *  Very confusing point.
 *  PHP-CPP documentation says do not need 
 *  for C++ inheritcance heirarchy to follow
 *  PHP object inheritance heirachy.
 * 
 *  Only need to init_ext with derivedClass.extends(myClass);
 * 
 *  Then their will be two objects, a Template and a ServiceAccess
 *    when Template object is created.
 * 
 *  It is opaque to me, how Zend engine implements object inheritance in memory layout.
 *  Maybe object instance has a sequential memory block?
 *  
 * in classimpl.h -- so class implmentation is
 *  PHP-CPP void extends(const std::shared_ptr<ClassImpl> &base) { _parent = base; }
 *  
 * 
 * And initialization in classimpl.cpp checks that parent is registered first.
 * 
 * Each ObjectImpl registers a single zend object, and PHP-CPP has no pointer to a parent instance.
 * It importantly sets the base address of the C++ object below the zend_object 
 * 
 * So where does zend store child's dynamic properties? in the parent instance?
 * 
 * 
 * Try declaring this inheritance in C++, and see if PHP-CPP and Zend get it right.
 * 
 * Template functionality does not use ServiceAccess directly. 
 * It is be callable for the phtml templates.
 * 
 * 
 * 
 */
	class Wcc_Plate : public ServiceAccess 
	{
	protected:

		// callback with plate argument
		zobj_own engine_; // Php engine instance object

		zstr_own leaf_; // Plate leaf path, key for template

		zstr_own path_; // full file path, when located

		htab_own  data_; // data assigned, key->value

		htab_own sections_; // section contents as strings indexed by name

		zstr_own sectionName_; // section being rendered

		zstr_own layoutLeaf_; // layout (surrounder template)

		htab_own  layoutData_; // data for surrounder.

		int   style_level_;

		bool pushedSection_;

		zstr_own raw_;   // if assigned raw html string	

		//htab_own publish_; // during render, values to be extracted

		Wcc_PlateEngine* engine();

		void addSection(zend_string* name, zstr_own& sdata);
		
		zstr_own full_render(htab_ptr data);
	public:


		static const char* class_name;

		static Wcc_Plate* plate(zend_object* obj);

		static int getObLevel();

		static Wcc_Plate* make(zend_object* engine, zend_string* name);

		virtual void debug_info(HashTable* ht);

		virtual ~Wcc_Plate()
		{
			//showme();
		}
		/*
		void showme() {
			showarray("layoutData_",layoutData_);
			showarray("data_",data_);

			showarray("sections_",sections_);			
		}
		*/
		void construct(zval_ptr engine, zend_string* name);

		void setRaw(zend_string* html)
		{
			raw_ = html;
		}
		
		 zstr_own&    getPath();
		 zstr_own&    getName();

		void setData(zval_ptr data);

		htab_own&  getData();
		zstr_own   render(htab_ptr data);

		zstr_own fetch(zend_string* name, zval_ptr data);

		zstr_own insert(zend_string* name, zval_ptr data);

		zstr_own escape(zend_string* s, zend_string* func);
		
		zstr_own getContent();

		bool      pathExists();

		htab_own getPublish();
		
		zstr_own getSection(zstr_ptr name, zstr_ptr defaultval);

		void push(zend_string* name);

		void stop();
		void start(zend_string* name);
		void layout(zend_string* leaf, zval_ptr data);
		
		void setLayoutLeaf(zend_string* leaf);
		void setLayoutData(zval_ptr data);
		
		void styleBegin();
		void styleEnd();

		void clean(); // lose engine object reference
		friend class Wcc_PlateEngine;

	};
	typedef base_obj_mgr<Wcc_Plate>  Wcc_Plate_Mgr;

	extern Wcc_Plate_Mgr plate_mgr;


	Wcc_Plate* Wcc_Plate::plate(zend_object* obj) {
		return (Wcc_Plate*) plate_mgr.cpp(obj);
	}

	Wcc_PlateEngine* Wcc_Plate::engine() {
		return (Wcc_PlateEngine*) plate_engine_mgr.cpp(engine_);
	}

}; // namespace Wcc


#endif