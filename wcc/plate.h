#ifndef  PLATE_WCC_H
#define  PLATE_WCC_H

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
class Plate : public base_d
{
protected:

	// callback with plate argument
	obj_rc engine_; // Php engine instance object

	str_rc name_; // Plate leaf path, key for template

	str_rc path_; // full file path, when located

	htab_rc  data_; // data assigned, key->value

	htab_rc sections_; // section contents as strings indexed by name

	str_rc sectionName_; // section being rendered

	str_rc layout_; // layout (surrounder template)

	htab_rc  layoutData_; // data for surrounder.

	int   style_level_;

	bool isPushed_;

	str_rc raw_;   // if assigned raw html string	

	//htab_own publish_; // during render, values to be extracted

	void addSection(str_ptr name, str_ptr sdata);
	
	str_rc full_render(htab_rd data);

	// cached call methods

public:

	static base_obj_mgr<Plate> omg;

	static int getObLevel();

	//static obj_rc make(obj_ptr engine, str_ptr name);

	virtual void debug_info(htab_rw hw);

	virtual ~Plate()
	{
		//showme();
	}

	void construct(str_ptr name, obj_ptr engine);

	void setRaw(str_ptr html)
	{
		raw_ = html;
	}
	
	str_ptr    getPath();
	str_ptr    getName();

	void setData(htab_rd data);

	htab_rd   getData();
	str_rc   render(htab_rd data);

	str_rc fetch(str_ptr name, htab_rd data);

	str_rc insert(str_ptr name, htab_rd data);

	str_rc escape(str_ptr s, str_ptr func);
	
	str_rc getContent();

	bool     pathExists();

	htab_rc getPublish();
	
	str_ptr getSection(str_ptr name, str_ptr defaultval);

	void push(str_ptr name);

	void stop();
	void start(str_ptr name);
	void layout(str_ptr leaf, htab_rd data);
	
	void setLayout(str_ptr leaf);
	void setLayoutData(htab_rd data);
	
	void styleBegin();
	void styleEnd();

	void clean(); // lose engine object reference

#ifndef BASE_ZOBJPTR
	VIRTUAL_ZOBJPTR
#endif
		friend class  PlateEngine;

};

}; // namespace Wcc


#endif