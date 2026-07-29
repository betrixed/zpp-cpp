#ifndef ZHM_ZPP_H
#define ZHM_ZPP_H

/**
 * Headers for collection of zend handle managers - zhm
 * 
 * 
 */
// avoid methods that require classes using zpp::base_d
#define OMIT_BASE_D

#include "str_ptr.h"
#include "str_rc.h"
#include "obj_ptr.h"
#include "obj_rc.h"
#include "htab_ptr.h"
#include "htab_rc.h"
#include "htab_rw.h"
#include "htab_walk.h"

#include "val_ptr.h"
#include "val_rc.h"
#include "str_buf.h"
#include "state_init.h"
#include "fn_call.h"
#include "show_zpp.h"
#include "zarg_rd.h"
#include "alloc_phpreq.h"

#endif
