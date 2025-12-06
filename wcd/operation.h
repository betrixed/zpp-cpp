#ifndef WCD_OPERATION_H
#define WCD_OPERATION_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#ifndef WCD_IDRIVER_H
#include "idriver.h"
#endif



namespace wcd {

	using namespace zpp;
	using namespace wcc;


class Operation : public base_d {
protected:

	str_rc db_name_;
	obj_rc driver_;
	obj_rc bind_;
	obj_rc joiner_;
	
	void genSql();

	bool bindPtr(error_return& e, Bindings*&);
	bool dbPtr(error_return& e, IDriver*&);
public:

	static base_obj_mgr<Operation> omg;

	virtual void debug_info(htab_rw di);
	
	void construct(obj_ptr db);
	void destruct();

	obj_return addPrime(str_ptr table, str_ptr alias, htab_ptr cols);

	val_return firstRow(int fetch);
	
	obj_return getBind();

	obj_return getDb();

	obj_return getJoiner();

	val_return getRows(int fetch = IDriver::FETCH_ASSOC);

	virtual obj_return getSqlParams();

	str_return getSql();

	error_return limit(val_ptr ct, val_ptr start);

	error_return orderBy(val_ptr column, bool descend);

	obj_return prepare(int fetch);

	error_return returns(htab_ptr list);

	val_return run();

	error_return where(val_ptr lattr, val_ptr rattr, int op, int blogic);

	virtual void wipe();
};

}; //namespace

#endif
