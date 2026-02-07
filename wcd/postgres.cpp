#ifndef WCD_POSTGRES_CPP
#define WCD_POSTGRES_CPP

#ifndef WCD_SQL_POSTGRES_H
#include "postgres.h"
#endif

#ifndef SQL_ISQL_H
#include "sql_isql.h"
#endif

#ifndef WCD_POSTGRES_ARGINFO
#define WCD_POSTGRES_ARGINFO
extern "C" {
	#include "stub/postgres_arginfo.h"
}
#endif

namespace wcd {

base_obj_mgr<Postgres> Postgres::omg;

PGSInit PGSd;

void PGSInit::init() 
{
	rex_pgname = "/^[a-z_][a-z_0-9]*$/";
}


Postgres::Postgres()
{
	pgname.setExpr(PGSd.rex_pgname);
}

str_rc 
Postgres::quoteName(str_ptr name)
{
	str_rc result;
	if (name.ok())
	{
		const char* z0 = name.data();

		switch(*z0)
		{
		case '*':
		case '"':
			result = name;
			break;
		default:
			if (!pgname.matches(name))
			{
				str_buf buf;
				buf << '"' << name << '"';
				result = buf.zstr();
			}
			else {
				result = name;
			}
		}
	}
	return result;
}

str_rc 
Postgres::seqLastValue(str_ptr seqname)
{
	str_rc result;
	str_buf buf;

	result = this->quoteName(seqname);
	buf << "select last_value from " << result;
	result = buf.zstr();
	return result;
}

str_return 
Postgres::truncate(Bindings& bind)
{
	str_return result;

	JoinTables* jt = bind.getJoinTables();
	obj_rc prime = jt->getPrime();

	if (!prime.ok())
	{
		result.error() << "No table / IColumns object set";
		return result;
	}
	IColumns* ic = zobj_toc<IColumns>(prime);

	str_rc name = ic->getName();

	str_buf sql;

	sql << "DELETE FROM " << name;

	result.value_ = sql.zstr();

	return result;
}


str_rc 
Postgres::orderField(htab_ptr order)
{
	str_buf buf;
	str_rc result;

	val_ptr col = order.get(SQSTR.column);
	str_rc attr, alias;

	if (col.isObject())
	{
		TableAttr* ta = zval_toc<TableAttr>(col);
		alias = ta->getTable();
		attr = ta->getAttr();
		buf << alias << '.' << this->quoteName(attr);
	}
	else {
		attr = col.zstr();
		buf << this->quoteName(attr);
	}
	val_ptr descend = order.get(SQSTR.desc);

	if (descend.isTrue()) {
		buf << " DESC";
	}
	else { // ?usually the default?
		buf << " ASC";
	}

	val_ptr nulls_last = order.get(SQSTR.nulls_last);
	if (nulls_last.isTrue())
	{
		buf << " NULLS LAST";
	}
	result = buf.zstr();
	return result;
}

}  // namespace wcd


using namespace wcd;
using namespace zpp;

//static called by sql_isql.cpp
zend_class_entry* 
Postgres::register_class(zend_class_entry* isql_class)
{
	zend_class_entry* me =  register_class_Wcd_Sql_Postgres(isql_class);
	Postgres::omg.classEntry(me);
	return me;
}

#endif
