#ifndef ROUTES_EXTN_CXX
#define ROUTES_EXTN_CXX


#include "wcc/userdata.cpp"

#include "wcc/usersession.cpp"

#include "wcc/finder.cpp"
#include "wcc/loader.cpp"

#include "wcc/service_access.cpp"

#include "wcc/hmap.cpp"

#include "wcc/icachedata.cpp"
#include "wcc/icache.cpp"
#include "wcc/dircache.cpp"
#include "wcc/cachemgr.cpp"

#include "wcc/file_upload.cpp"
#include "wcc/global_response.cpp"
#include "wcc/pair.cpp"
#include "wcc/request_globals.cpp"

#include "wcc/route.cpp"
#include "wcc/route_match.cpp"
#include "wcc/route_set.cpp"
#include "wcc/target.cpp"

#include "wcc/module.cpp"

#include "wcc/dispatch.cpp"

#include "wcc/debuglog.cpp"
#include "wcc/run.cpp"






void register_routes_extn(INIT_FUNC_ARGS)
{

#ifdef WCC_LOADER_CPP
	PHP_MINIT(wcc_loader_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_FINDER_CPP
	PHP_MINIT(Wcc_Finder_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_CONFIG_CPP
	PHP_MINIT(wcc_replace_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(Wcc_Config_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_HMAP_CPP
	PHP_MINIT(Wcc_Hmap_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef SERVICE_ACCESS_CPP
	PHP_MINIT(ServiceAccess_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef GLOBAL_RESPONSE_CPP
	PHP_MINIT(Wcc_Response_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif
	
#ifdef REQUEST_GLOBALS_CPP
	PHP_MINIT(RequestGlobals_reg)(INIT_FUNC_ARGS_PASSTHRU);
	PHP_MINIT(FileUpload_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_PAIR_CPP
PHP_MINIT(wcc_pair_d)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_ROUTE_CPP
	PHP_MINIT(wcc_route_d)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef ROUTE_MATCH_CPP
	PHP_MINIT(route_match_d)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_ROUTESET_CPP
	PHP_MINIT(wcc_routeset_d)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_TARGET_CPP
		PHP_MINIT(wcc_target)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef ICACHE_CPP
	PHP_MINIT(Wcc_ICache_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef DIR_CACHE_CPP
	PHP_MINIT(Wcc_DirCache_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_CACHEMGR_CPP
	PHP_MINIT(Wcc_CacheMgr_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif	

#ifdef WCC_MODULE_CPP
	PHP_MINIT(Wcc_Module_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_DISPATCH_CPP
	PHP_MINIT(Wcc_Dispatch_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif


#ifdef WCC_RUN_CPP
	PHP_MINIT(wcc_run_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_DEBUGLOG_CPP
	PHP_MINIT(wcc_debuglog_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef SESSION_USERDATA_CPP
	PHP_MINIT(Session_UserData_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

#ifdef WCC_USERSESSION_CPP
	PHP_MINIT(Wcc_UserSession_reg)(INIT_FUNC_ARGS_PASSTHRU);
#endif

}

#endif