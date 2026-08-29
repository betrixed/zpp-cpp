#ifndef STATE_MODULE_H
#define STATE_MODULE_H

#include "zpp/state_init.h"

#define STATE_INIT_ADD(si) STATE_LIST_NAME.add_si(&si);
#define STATE_MOD_INIT     STATE_LIST_NAME.call_mod_init();
#define STATE_MOD_END      STATE_LIST_NAME.call_mod_end();
#define STATE_REQ_INIT     STATE_LIST_NAME.call_req_init();
#define STATE_REQ_END      STATE_LIST_NAME.call_req_end();

#define DECLARE_STATE_LIST extern zpp::state_list  STATE_LIST_NAME;
#define DEFINE_STATE_LIST  zpp::state_list  STATE_LIST_NAME( EXTN_MODULE_NAME );

#endif