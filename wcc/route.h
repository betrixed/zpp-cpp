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


using  permstr = str_perm;

//typedef std::vector<str_perm> HttpVerbNames;
//typedef std::map<str_perm, int, MapComparator > HttpVerbBits;

class Route_init : public zpp::state_init {
public:

	// str_perm rather than str_intern.
	// str_intern stored in Array risks double destructor.
	// PHP also does free of interned strings.
	// If using str_intern,  htab_persist must only use freehtmemory.
	// if using str_perm,    htab_persist can use zend_hash_graceful_destroy 

	permstr GET_S;
	permstr POST_S;
	permstr PUT_S;
	permstr PATCH_S;
	permstr OPTIONS_S;
	permstr DELETE_S;
	permstr HEAD_S;
	permstr CONNECT_S;
	permstr TRACE_S;
	permstr PURGE_S;

			// route
	permstr cc_verbs;
	permstr cc_ajax;
	permstr cc_params;
	permstr cc_compiled;
	permstr cc_pattern;
	permstr cc_target;
	permstr cc_id;

	// values for RouteSet
	permstr ARG_S;
	permstr FUN_S;
	permstr FUNX_S;
	permstr HITS_S;
	permstr MOD_S;
	permstr NSP_S;
	permstr OBJ_S;
	permstr OBJX_S;
	permstr ROLE_S;
	


	/* Hash tables cannot be dyanmically allocated during Module Init
	 and cannot be dynamically deallocated during Module Shutdown.
	 This static storage seems to work.
	*/
	htab_persist  route_verbs;
	htab_persist  verb_names;


	//Route_init() : route_verbs((HashTable*)nullptr), verb_names((HashTable*)nullptr) {}

	void init() override;
	void end() override;

	void init_ce(zend_class_entry* ce);

};

extern Route_init route_data;


// These constants are internal only, for flag mask mapping.






}; // namespace


#endif