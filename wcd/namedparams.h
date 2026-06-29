#ifndef WCD_NAMEDPARAMS_H
#define WCD_NAMEDPARAMS_H
/**
 * @Author: Michael Rynn, michael.rynn.500@gmail.com, 2026.06.27
 * 
 */
#ifndef WCD_SIMPLEPARAMS_H
#include "iparams.h"
#endif

namespace wcd
{
	using namespace zpp;

	class NamedParams : public IParams {
	public:

		static base_obj_mgr<NamedParams>  omg;

		static zend_class_entry* register_class(zend_class_entry* ce_simpleparams);

		static str_rc	n_param(int ct);

		str_rc paramStr(int ct) override;

		str_rc addParamEquals(val_ptr value) override;

	};

} //namespace wcd


#endif