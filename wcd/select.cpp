#ifndef WCD_SELECT_CPP
#define WCD_SELECT_CPP

#ifndef WCD_SELECT_H
#include "select.h"
#endif

namespace wcd {

using namespace wcc;
using namespace zpp;

base_obj_mgr<Select> Select::omg;


zobj_mgr 
Select::getSqlParams()
{
	Bindings& bind = this->bindings();
	if (autoAlias_) {
		bind.aliasSelect();
	}
	zobj_mgr isql_mgr = bind.isql();
	ISql* isq = zobj_toc<ISql>(isql_mgr);

	zobj_mgr plist_mgr = isq->select(bind);
	bind.wipe();
	return plist_mgr;
}

void Select::construct(zobj_user db, bool autoAlias = false)
{
	Operation::construct(db);
	autoAlias_ = autoAlias;
	zobj_user self(vobj());

	icols_ = IColumns::omg.new_zobj();
	icol().construct(self);

}

void Select::destruct()
{
	icols_.init();
	Operation::destruct();
}

void Select::add(htab_read cols)
{
	icol().add(cols);
}

zobj_mgr 
Select::addJoin(zobj_user ltable, zobj_user rtable, int jtype)
{
	zobj_mgr ji_mgr = JoinInfo::omg.new_zobj();
	JoinInfo  ji = zobj_toc<JoinInfo>(ji_mgr);

	ji->construct(ltable, rtable, jtype);

	JoinTables& jt = this->joiner();
	
	jt.addJoin(ji_mgr);
	return ji_mgr;


}

zobj_mgr 
Select::addTable(zstr_user table, zstr_user alias, htab_read cols)
{
	zobj_mgr tc_mgr = TColumns::omg.new_zobj();
	TColumns* tc = zobj_toc<TColumns>(tc_mgr);
	tc->construct(table, alias, cols);

	JoinTables& jt = this->joiner();
	jt.addTable(tc);

	return tc_mgr;
}

zval_mgr 
Select::getRenamed()
{
	zval_mgr results = getRows();

	zval_mgr rename = bindings().get(ISql::SQL_RENAME);

	if (results.isArray() && rename.isArray())
	{
		htab_mgr objset;
		htab_write hw(objset);

		htab_read rows(results);
		htab_read rtab(rename);
		htab_walk wk;

		for(wk.start(rows); wk.ok(); wk.next())
		{
			zobj_mgr split = JoinTables::rowSplit(rows, rtab);
			hw.push_back(split);
		}
		results = objset;
	}
	return result;
}

zobj_mgr Select::iCols()
{
	return icols_;
}

void Select::setAlias(zstr_user alias)
{
	icol().setAlias(alias);
}


}; // namespace


#endif

