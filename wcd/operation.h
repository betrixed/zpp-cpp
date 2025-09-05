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

	Bindings& bindings() {
		obj_ptr bind = getBind();
		return *zobj_toc<Bindings>(bind);
	}

	IDriver& driver()
	{
		obj_ptr db = getDb();
		return  *zobj_toc<IDriver>(db);
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

	obj_rc addPrime(str_ptr table, str_ptr alias, htab_ptr cols);

	val_rc firstRow(int fetch);
	
	obj_ptr getBind();

	obj_ptr getDb();

	obj_ptr getJoiner();

	val_rc getRows(int fetch = IDriver::FETCH_ASSOC);

	virtual obj_rc getSqlParams();

	str_rc getSql();

	void limit(val_ptr ct, val_ptr start);

	void orderBy(val_ptr column, bool descend);

	obj_rc prepare(int fetch);

	void returns(htab_ptr list);

	val_rc run();

	void where(val_ptr lattr, val_ptr rattr, int op, int blogic);

	virtual void wipe();
};

}; //namespace

#endif