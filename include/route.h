#ifndef  WCC_ROUTE_H
#define  WCC_ROUTE_H

#ifndef WC_BASE_H
#include "wc_base.h"
#endif

#include <map>
#include <vector>

namespace wcc {

enum html {
	V_GET = 1,
	V_POST = 2,
	V_PUT = 4,
	V_PATCH = 8,
	V_OPTIONS = 16,
	V_DELETE = 32,
	V_HEAD = 64,
	V_CONNECT = 128,
	V_TRACE = 256,
	V_PURGE = 512
};

enum ajax {
	AJ_NONE = 1,
	AJ_ONLY = 2,
	AJ_ALSO = 3
};

class  Route : public base_d {
public:

	
	zend_long    verbs_;
	zend_long	 ajax_;

	zstr_own     id_;

	zstr_own 	 pattern_;
	zstr_own 	 compiled_;

	zval_own 	 target_;

	// matched values of reg-expression
	htab_own 	 params_;



	Route();
	virtual ~Route();

	virtual void debug_info(HashTable *ht);

	static bool isMyClass(zend_object* obj);

	static zend_long getVerbInt(const zstr_base& sverb);
	static htab_own getVerbNames( zend_long flags );
	
	static zstr_own getVerb(zend_long verb);

	static zobj_own get(zstr_ptr pattern, zval_ptr target, int ajax = ajax::AJ_NONE);

	static zobj_own post(zstr_ptr pattern, zval_ptr target, int ajax = ajax::AJ_NONE);

	static zobj_own methods(int verbs, zstr_ptr pattern, zval_ptr target, int ajax = ajax::AJ_NONE);

	

	void construct(int verbs, zstr_ptr pattern, zval_ptr target);

	void name(zstr_ptr name);
	zstr_ptr  getName();

	bool hasParams()
	{
		return (params_.size() > 0) ? true : false;
	}

	htab_ptr getParams();

	zend_long getVerbs() const 
	{
		return verbs_;
	}
	
	void setParams(zval_ptr params);

	zval_own routeUrl(htab_ptr params);
	
	zstr_ptr getCompiled() const
	{
		return compiled_;
	}
	zstr_ptr getPattern() const
	{
		return pattern_;
	}

	const zval_own getTarget() const {
		return target_;
	}

	void setPattern(zstr_ptr p)
	{
		pattern_ = p;
	}

	void setCompiled(zstr_ptr c)
	{
		compiled_ = c;
	}

	htab_own __serialize();
	
	void __unserialize(htab_ptr htab);
};

class RouteMgr : public base_obj_mgr<Route> {
public:

	virtual void init_class_fn(); 
};

//typedef std::vector<zstr_perm> HttpVerbNames;
//typedef std::map<zstr_perm, int, MapComparator > HttpVerbBits;

class Route_init : public zpp::state_init {
public:
	zstr_intern GET_S;
	zstr_intern POST_S;
	zstr_intern PUT_S;
	zstr_intern PATCH_S;
	zstr_intern OPTIONS_S;
	zstr_intern DELETE_S;
	zstr_intern HEAD_S;
	zstr_intern CONNECT_S;
	zstr_intern TRACE_S;
	zstr_intern PURGE_S;

			// route
	zstr_intern cc_verbs;
	zstr_intern cc_ajax;
	zstr_intern cc_params;
	zstr_intern cc_compiled;
	zstr_intern cc_pattern;
	zstr_intern cc_target;
	zstr_intern cc_id;

	zstr_intern cc_fixed;
	zstr_intern cc_vary;
	zstr_intern cc_file;
	/* Hash tables cannot be dyanmically allocated during Module Init
	 and cannot be dynamically deallocated during Module Shutdown.
	 Static storage seems to work.
	*/
	HashTable  route_verbs;
	HashTable verb_names;

	//Route_init() : route_verbs((HashTable*)nullptr), verb_names((HashTable*)nullptr) {}

	virtual void init();
	virtual void end();

	void init_ce(zend_class_entry* ce);

};

extern RouteMgr   route_mgr;
extern Route_init route_data;


// These constants are internal only, for flag mask mapping.






}; // namespace


#endif