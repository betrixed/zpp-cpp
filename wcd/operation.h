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
	zobj_mgr db_;
	zobj_mgr bind_;
	zobj_mgr joiner_;
	zobj_mgr plist_;

	void genSql();

	Bindings& bindings() {
		return *zobj_toc<Bindings>(bind_);
	}

	IDriver& driver() {
		return *zobj_toc<IDriver>(db_);
	}
	
	JoinTables& joiner() 
	{
		zobj_user jobj = getJoiner();
		return *zobj_toc<JoinTables>(jobj);
	}
public:

	static base_obj_mgr<Operation> omg;

	virtual void debug_info(htab_write di);
	
	void construct(zobj_user db);
	void destruct();

	zobj_mgr addPrime(zstr_user table, zstr_user alias, htab_read cols);

	zval_mgr firstRow(int fetch);
	

	zobj_user getJoiner();

	htab_read getParams();

	zval_mgr getRows(int fetch);

	virtual zobj_mgr getSqlParams();

	zstr_mgr getSql();

	void limit(zval_user ct, zval_user start);

	void orderBy(zval_user column, bool descend);

	zobj_mgr prepare(int fetch);

	void returns(htab_read list);

	zval_mgr run();

	void where(zval_user lattr, zval_user rattr, int op, int blogic);

	void wipe();
};

}; //namespace

#endif