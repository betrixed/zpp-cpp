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
	obj_rc db_;
	obj_rc bind_;
	obj_rc joiner_;
	obj_rc plist_;

	void genSql();

	Bindings& bindings() {
		return *zobj_toc<Bindings>(bind_);
	}

	IDriver& driver() {
		return *zobj_toc<IDriver>(db_);
	}
	
	JoinTables& joiner() 
	{
		obj_ptr jobj = getJoiner();
		return *zobj_toc<JoinTables>(jobj);
	}
public:

	static base_obj_mgr<Operation> omg;

	virtual void debug_info(htab_rw di);
	
	void construct(obj_ptr db);
	void destruct();

	obj_rc addPrime(str_ptr table, str_ptr alias, htab_rd cols);

	val_rc firstRow(int fetch);
	

	obj_ptr getJoiner();

	htab_rd getParams();

	val_rc getRows(int fetch = IDriver::FETCH_ASSOC);

	virtual obj_rc getSqlParams();

	str_rc getSql();

	void limit(val_ptr ct, val_ptr start);

	void orderBy(val_ptr column, bool descend);

	obj_rc prepare(int fetch);

	void returns(htab_rd list);

	val_rc run();

	void where(val_ptr lattr, val_ptr rattr, int op, int blogic);

	void wipe();
};

}; //namespace

#endif