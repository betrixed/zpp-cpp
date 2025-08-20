#ifndef  WCC_ROUTE_H
#define  WCC_ROUTE_H

#ifndef ZPP_BASE_H
#include "zpp/base.h"
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

	str_rc     id_;

	str_rc 	 pattern_;
	str_rc 	 compiled_;

	val_rc 	 target_;

	// matched values of reg-expression
	htab_rc 	 params_;

	class RouteMgr : public base_obj_mgr<Route> {
	public:

		virtual void init_class_fn(); 
	};

	Route();
	virtual ~Route();

	static RouteMgr omg;

	virtual void debug_info(htab_rw hw);

	static bool isMyClass(str_ptr obj);

	static zend_long getVerbInt(str_ptr sverb);
	static htab_rc  getVerbNames( zend_long flags );
	
	static str_rc getVerb(zend_long verb);

	static obj_rc get(str_ptr pattern, val_ptr target, int ajax = ajax::AJ_NONE);

	static obj_rc post(str_ptr pattern, val_ptr target, int ajax = ajax::AJ_NONE);

	static obj_rc methods(int verbs, str_ptr pattern, val_ptr target, int ajax = ajax::AJ_NONE);

	void construct(int verbs, str_ptr pattern, val_ptr target);

	void name(str_ptr name);
	str_ptr  getName();

	zend_long getVerbs() const 
	{
		return verbs_;
	}

	bool hasParams()
	{
		return (htab_ptr(params_).size() > 0) ? true : false;
	}

	htab_ptr getParams();
	
	void setParams(htab_ptr params);

	str_rc routeUrl(htab_ptr pvalues);
	
	str_ptr getCompiled() const
	{
		return compiled_;
	}
	
	str_ptr getPattern() const
	{
		return pattern_;
	}

	val_ptr getTarget() const {
		return target_;
	}

	void setTarget(val_ptr tg)
	{
		target_ = tg;
	}
	
	void setPattern(str_ptr p)
	{
		pattern_ = p;
	}

	void setCompiled(str_ptr c)
	{
		compiled_ = c;
	}

	htab_rc __serialize();
	
	void __unserialize(htab_ptr htab);

	VIRTUAL_ZOBJPTR

	
};



//typedef std::vector<str_perm> HttpVerbNames;
//typedef std::map<str_perm, int, MapComparator > HttpVerbBits;

class Route_init : public zpp::state_init {
public:
	str_intern GET_S;
	str_intern POST_S;
	str_intern PUT_S;
	str_intern PATCH_S;
	str_intern OPTIONS_S;
	str_intern DELETE_S;
	str_intern HEAD_S;
	str_intern CONNECT_S;
	str_intern TRACE_S;
	str_intern PURGE_S;

			// route
	str_intern cc_verbs;
	str_intern cc_ajax;
	str_intern cc_params;
	str_intern cc_compiled;
	str_intern cc_pattern;
	str_intern cc_target;
	str_intern cc_id;

	// values for RouteSet
	str_intern ARG_S;
	str_intern FUN_S;
	str_intern FUNX_S;
	str_intern HITS_S;
	str_intern MOD_S;
	str_intern NSP_S;
	str_intern OBJ_S;
	str_intern OBJX_S;
	str_intern ROLE_S;
	


	/* Hash tables cannot be dyanmically allocated during Module Init
	 and cannot be dynamically deallocated during Module Shutdown.
	 This static storage seems to work.
	*/
	HashTable  route_verbs;
	HashTable  verb_names;

	static void nodestroy(zval* val);

	//Route_init() : route_verbs((HashTable*)nullptr), verb_names((HashTable*)nullptr) {}

	virtual void init();
	virtual void end();

	void init_ce(zend_class_entry* ce);

};

extern Route_init route_data;


// These constants are internal only, for flag mask mapping.






}; // namespace


#endif