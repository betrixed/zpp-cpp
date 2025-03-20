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
class Plate 
{
protected:

	// callback with plate argument
	zobj_mgr engine_; // Php engine instance object

	zstr_mgr name_; // Plate leaf path, key for template

	zstr_mgr path_; // full file path, when located

	htab_mgr  data_; // data assigned, key->value

	htab_mgr sections_; // section contents as strings indexed by name

	zstr_mgr sectionName_; // section being rendered

	zstr_mgr layout_; // layout (surrounder template)

	htab_mgr  layoutData_; // data for surrounder.

	int   style_level_;

	bool isPushed_;

	zstr_mgr raw_;   // if assigned raw html string	

	//htab_own publish_; // during render, values to be extracted

	void addSection(zstr_user name, zstr_user sdata);
	
	zstr_mgr full_render(htab_read data);

	// cached call methods

public:

	static base_obj_mgr<Plate> omg;

	static int getObLevel();

	//static zobj_mgr make(zobj_user engine, zstr_user name);

	virtual void debug_info(htab_write hw);

	virtual ~Plate()
	{
		//showme();
	}

	void construct(zobj_user engine, zstr_user name);

	void setRaw(zstr_user html)
	{
		raw_ = html;
	}
	
	zstr_user    getPath();
	zstr_user    getName();

	void setData(htab_read data);

	htab_read   getData();
	zstr_mgr   render(htab_read data);

	zstr_mgr fetch(zstr_user name, htab_read data);

	zstr_mgr insert(zstr_user name, htab_read data);

	zstr_mgr escape(zstr_user s, zstr_user func);
	
	zstr_mgr getContent();

	bool     pathExists();

	htab_mgr getPublish();
	
	zstr_user getSection(zstr_user name, zstr_user defaultval);

	void push(zstr_user name);

	void stop();
	void start(zstr_user name);
	void layout(zstr_user leaf, htab_read data);
	
	void setLayout(zstr_user leaf);
	void setLayoutData(htab_read data);
	
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