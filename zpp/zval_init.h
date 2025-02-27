 /*  
  *  PHP extension C++ classes - zpp 
  *  @author Michael Rynn <michael.rynn.500@gmail.com>
  *  @copyright 2024-2025 Michael Rynn
  */

#ifndef ZVAL_INIT_H
#define ZVAL_INIT_H

namespace zpp 
{
	struct zval_init {
	    zval zv_;

	    void init() { zv_ = {0}; ZVAL_NULL(&zv_); }

	    zval_init() { init(); }

	    operator zval*() const { return (zval*) &zv_; }
	};
};

#endif

//zval_init.h