#ifndef WCD_ISQL_CPP
#define WCD_ISQL_CPP

#ifndef WCD_ISQL_ARGINFO
#define WCD_ISQL_ARGINFO

extern "C" {
	#include "stub/isql_arginfo.h"
}
#endif

namespace wcd {
using namespace zpp;



base_obj_mgr<ISql> ISql::omg;

zend_class_entry* 	zintf_ce_Sql_IfSql;
zend_class_entry* 	zclass_isql;


str_rc
ISql::quoteName(str_ptr name)
{
	str_rc result;

	if (name.size() > 0)
	{
		char ic = *(name.data());
		if ((ic == '*')||(ic == '`'))
		{
			result = name;
			return result;
		}
		str_buf buf;
		buf << '`' << name << '`';
		result = buf.zstr();
	}
	else {
		result = SQSTR.mysql_empty;
	}
	return result;
}

str_rc 
ISql::columns(htab_ptr bd)
{
	str_buf buf;

	htab_walk wk;
	auto ix = wk.key();
	auto name = wk.value();

	for(wk.start(bd); wk.ok(); wk.next())
	{
		if (ix.isLong() && (ix.zlong() == 0))
		{
			buf << ' ';
		}
		else 
		{
			buf << ',';
		}
		str_rc colname(name.zstr());

		if (colname.size() && (colname.vstr() != "*"))
		{
			buf << this->quoteName(colname);
		}
		else {
			buf << '*';
		}
	}
	return buf.zstr();
}


void  
ISql::columnsTC(IColumns* tc, htab_rw col_list)
{
	str_rc cfrag;

	str_ptr alias = tc->getAlias();
	{
		htab_ptr colNames(tc->getColNames());
		htab_walk wk;
		auto name = wk.key();
		auto rename = wk.value();

		for(wk.start(colNames); wk.ok(); wk.next())
		{
			str_buf fbuf;

			if (alias.size()) {
				fbuf << alias << '.';
			}
			if (name.isString() && name.size())
			{
				if (name.vstr() == "*")
				{
					fbuf << '*';
				}
				else {
					fbuf << this->quoteName(name.zstr());
					if (rename.isString())
					{
						fbuf << " as " << this->quoteName(rename.zstr());
					}
				}
			}

			cfrag = fbuf.zstr();
			//showstr("cfrag:", cfrag);
			col_list.push_back(cfrag);
		}
	}
	{
		htab_ptr expr = tc->getExpr();
		htab_walk wk;
		auto exalias = wk.key();
		auto exfn = wk.value();

		for(wk.start(expr); wk.ok(); wk.next())
		{
			str_buf fbuf;
			str_rc name = this->quoteName(exalias.zstr());

			fbuf << exfn.zstr() << " as " << name;

			cfrag = fbuf.zstr();
			//showstr("expr:", cfrag);
			col_list.push_back(cfrag);
		}
	}
}

// 
str_rc
ISql::orderBy(htab_ptr obind)
{
	str_rc result;

	str_buf buf;

	buf << ' ';

	htab_walk wk;

	auto ix = wk.key();
	auto order_tab = wk.value();
	str_ptr alias;
	str_ptr attr;
	for(wk.start(obind); wk.ok(); wk.next())
	{
		if (order_tab.isArray())
		{
			htab_ptr order(order_tab.zarray());
			if (ix.zlong() > 0)
			{
				buf << ", ";
			}

			val_ptr col = order.get(SQSTR.column);

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
		}
	}
	result = buf.zstr();
	//showstr("orderby ", result);

	return result;

}

obj_return 
ISql::deleteSql(Bindings& bind)
{
	str_buf buf;
	obj_return result;

	buf << "DELETE FROM";

	htab_ptr tables = this->getTables(bind);
	obj_return paramList_ret = bind.getParamList();

	if (paramList_ret.has_errors())
	{
		result = std::move(paramList_ret);
		return result;
	}

	obj_rc& pobj = paramList_ret.value_;
	ParamList* plist = zobj_toc<ParamList> (pobj);

	htab_walk pos;
	// first table
	// TODO: shouldn't first table be in prime_??
	pos.start(tables);

	val_ptr tcobj = pos.value();

	IColumns* icol = zval_toc<IColumns>(tcobj);
	buf << ' ' << this->quoteName(icol->getName());

	val_ptr wh( bind.get(SQL_WHERE));

	if (wh.isArray())
	{
		htab_ptr wbind( wh.zarray());

		if (wbind.size())
		{
			str_return temp = this->where(bind, wbind);
			if (temp.has_errors())
			{
				result = std::move(temp);
				return result;
			}
			buf << " WHERE" << temp.value_;
		}
	}

	val_ptr order( bind.get(SQL_ORDER));

	if (order.isArray())
	{
		buf << " ORDER BY" << this->orderBy(order);
	}

	

	//htab_ptr params = plist->getParams();

	str_rc sql = buf.zstr();
	plist->setSql(sql);
	plist->useOwnValues();
	result.value_ = pobj;

	return result;
}

SqlPart_return   //static 
getPartObj(val_ptr ret)
{
	SqlPart_return result;
	bool missed = true;

	if (ret.isObject())
	{
		obj_ptr test(ret.zobject());
		if (test.instanceof(zclass_sql_ifipart))
		{
			result = zobj_toc<SqlPartId>(test);
			missed = false;
		}
	}
	if (missed)
	{
		result = nullptr;
		result.error() << "SqlPartId object expected";
	}
	return result;
}

//* called from a JoinExpr
str_return
ISql::emit(val_ptr sp, Bindings* bind, str_ptr lalias, str_ptr ralias)
{
	str_return result;
 	SqlPart_return partret = getPartObj(sp);
 	if (partret.has_errors())
 	{
 		result = std::move(partret);
 	}
 	else {

	 	SqlPartId* part = partret.value_;
	 	int partid = part->getPartId();
		str_buf buf;
 		str_rc str;
 		str_return temp;

	 	switch(partid)
	 	{
	 	case SqlPartId::TA_PID:
	 		{
	 			TableAttr* ta = static_cast<TableAttr*>(part);
	 			str_ptr ta_alias = ta->getTable();
	 			str_ptr ta_name = ta->getAttr();

	 			buf << ta_alias << '.' << this->quoteName(ta_name);
	 		}
	 		break;
	 	case SqlPartId::LIT_PID:
	 		{
	 			Literal* lit = static_cast<Literal*>(part);
	 			buf << lit->toString();
	 		}
	 		break;
	 	case SqlPartId::EXPR_PID:
	 		{
	 			Expr* expr = static_cast<Expr*>(part);
	 			str = expr->toString();
	 			buf << str;
	 		}
	 		break;
	 	case SqlPartId::JE_PID:
	 		{
	 			JoinExpr* je = static_cast<JoinExpr*>(part);
	 			temp = je->emit(0, bind, lalias, ralias);
	 			if (temp.has_errors())
	 			{
	 				result = std::move(temp);
	 				goto RET_ALL;
	 			}
	 			buf << " (" << temp.value_ << " )";
	 		}
	 		break;
	 	case SqlPartId::PARAM_PID:
	 		{
	 			Param* p = static_cast<Param*>(part);
	 			val_ptr pvalue = p->getValue();
				obj_return paramList = bind->getParamList();
				if (paramList.has_errors())
				{
					result = std::move(paramList);
					return result;
				}
				ParamList* list = zobj_toc<ParamList> (paramList.value_);
	 			buf << list->addParam(pvalue);
	 		}
	 		break;
	 	default:
	 		result.error() << "Unmatched partid in emit";
	 		goto RET_ALL;
	 		break;
	 	}
	 	result = buf.zstr();
 	}
 RET_ALL:
 	return result;
}

str_rc 
ISql::getTruncateSql()
{
	return SQSTR.delete_from;
}

htab_ptr
ISql::getTables(Bindings& bind)
{
	JoinTables* jt = bind.getJoinTables();
	return jt->getTables();
}

str_return 
ISql::truncate(Bindings& bind)
{
	str_buf buf;
	str_return result;

	buf << this->getTruncateSql();

	JoinTables* jt = bind.getJoinTables();

	bool missed = true;

	if (jt) {
		obj_rc pobj = jt->getPrime();
		if (pobj.ok())
		{
			IColumns* icol = zobj_toc<IColumns>(pobj);
			buf << ' ' << this->quoteName(icol->getName());
			result = buf.zstr();
			missed = false;
		}
	}

	if (missed)
	{
		result.error() << "Error: Truncate table not set";
	}
	return result;
}

str_return
ISql::insert_col_params(Bindings& bind, htab_ptr rowbind)
{
	str_return result;

	htab_walk wk;

	str_buf ptext;
	str_buf ctext;

	str_rc place;

	obj_return paramList = bind.getParamList();
	if (paramList.has_errors())
	{
		result = std::move(paramList);
		return result;
	}
	ParamList* params = zobj_toc<ParamList> (paramList.value_);

	val_rc   zpass;

	int pcount = 0;
	int ix = 0;

	ctext << " (";

	auto key = wk.key();
	auto val = wk.value();
	str_rc colname;

	for(wk.start(rowbind); wk.ok(); wk.next(), ix++)
	{
		if (ix > 0)
		{
			ptext << ',';
			ctext << ',';
		}
		colname = key.zstr();
		ctext << this->quoteName(colname);
		
		if (val.isArray()) //TODO: maybe string would do here
		{
			htab_ptr vdata(val.zarray());
			place = vdata.get(int(0));
			if (place.vstr() == "default")
			{
				place = vdata.get(int(1));
			}
		}
		else 
		{
			zpass = (zend_long) pcount;
			place = params->addParam(zpass);
			pcount++;
		}
		ptext << place;
	}
	str_rc plist = ptext.zstr();

	ctext << ") VALUES (" << plist << ")";
	result.value_ = ctext.zstr();
	return result;
}

str_return
ISql::setSeqValue(int value, htab_ptr data)
{
	str_return result;

	str_rc seq_name = data.get(SQSTR.seq_key);

	if (!seq_name.isNull())
	{
		str_buf buf;

		buf << "select setval('" << seq_name << "'," << value << ")";
		result = buf.zstr();
	}
	else {
		result.error() << "Missing name for setSeqValue";
	}
	return result;
}

str_rc
ISql::seqLastValue(str_ptr seq)
{
	str_buf buf;

	buf << "select LASTVAL(" << seq << ")";

	return buf.zstr();
}

static SqlPart_return getIColumns(val_ptr zv)
{
	SqlPart_return result;

	if (zv.isObject())
	{
		result = zobj_toc<IColumns>(zv.zobject());
	}
	else {
		result.error() << "IColumns object expected";
	}
	return result;
}

void extract_params(htab_ptr rowbind, htab_ptr plist, htab_rw params);

void extract_params(htab_ptr rowbind, htab_ptr plist, htab_rw params)
{
	//showarray("after create", ret_params);

	htab_walk wk;
	auto pix = wk.value();

	htab_rc row_values = htab_rc::getValues(rowbind);

	for(wk.start(plist); wk.ok(); wk.next())
	{
		val_ptr pvalue = row_values.get(pix.zlong());
		params.push_back(pvalue);
	}
	return;
}

obj_return 
ISql::insert(Bindings& bind)
{
	obj_return result;

	str_buf buf;

	buf << "INSERT INTO";

	htab_ptr jtables = this->getTables(bind);

	if (!jtables.size()) {
		result.error() << "No insert table set";
		return result;
	}

	htab_walk pos;
	pos.start(jtables);
	
	auto icolpart = getIColumns(pos.value());

	if (icolpart.has_errors())
	{
		result = std::move(icolpart);
		return result;
	}
	IColumns* icol = (IColumns*) icolpart.value_;

	buf << ' ' << this->quoteName(icol->getName());

	htab_rc sql_insert;

	if (!bind.getArray(ISql::SQL_INSERT, sql_insert))
	{
		result.error() << "Insert table not set";
		return result;
	}

	htab_walk  insert_wk;
	htab_ptr  rowbind;
	str_return temp;

	int pcount = 0;
	if (insert_wk.start(sql_insert))
	{
		rowbind = insert_wk.value().zarray();
		
		size_t rsize = rowbind.size();

		 
		if (rsize)
		{
			temp = this->insert_col_params(bind, rowbind);
			if (temp.has_errors())
			{
				result = std::move(temp);
				return result;
			}
			pcount += rsize;
			buf << temp.value_;
		}
	}
	if (pcount == 0)
    {
    	obj_rc self(this->vobj());
    	val_rc dtext = self.call(SQSTR.valuesdefault);
		buf << ' ' << val_ptr(dtext).zstr() << ' ';
    }
	htab_rc rettab;
	val_ptr  valset;
	if (bind.getArray(ISql::SQL_RETURN, rettab))
	{
		buf << " RETURNING ";
		valset = rettab.get(int(0));

		if (valset.isArray() && valset.size())
		{   
			int rix=0;
			rettab = valset.zarray();
			htab_walk ret_wk;
			auto rixval = ret_wk.value();
			for(ret_wk.start(rettab);ret_wk.ok();ret_wk.next(), rix++)
			{
				if (rix != 0)
				{
					buf << ',';
				}
				buf << this->quoteName(rixval.zstr());
			}
		}
	}
	
	result = bind.getParamList();
	if (result.has_errors())
	{
		return result;
	}

	ParamList* plist = zobj_toc<ParamList>(result.value_);

	htab_ptr params = plist->getParams();

	htab_rc   ret_params_mgr;
	htab_rw ret_params(ret_params_mgr);

	if (params.size())
	{
		extract_params(rowbind, params, ret_params);

		if (sql_insert.size() > 1)
		{
			htab_rc   multirow_mgr;
			htab_rw multirow(multirow_mgr);
			

			multirow.push_back(ret_params);
			while(insert_wk.next())
			{
				rowbind  = insert_wk.value().zarray();

				htab_rc   pset_mgr;
				htab_rw pset(pset_mgr);

				extract_params(rowbind, params, pset);
				multirow.push_back(pset);
			}
			ret_params_mgr = multirow;
		}

	}
	str_rc sql = buf.zstr();

	plist->setSql(sql);

	if (ret_params.size())
	{
		plist->setValues(ret_params_mgr);
	}
	if (rettab.size())
	{
		plist->setReturns(rettab);
	}

	return result;
}

str_return 
ISql::fromJT(Bindings& bind, JoinTables* jt)
{
	str_return result;
	str_return temp;

	str_buf buf;

	buf << " FROM";
	//zend_printf("buflen %ld, %ld\n", buf.size(), buf.len());

	htab_ptr joins(jt->getData());

	obj_rc prime_obj(jt->getPrime());

	IColumns* prime = zobj_toc<IColumns>(prime_obj);

	str_rc tname(prime->getName());
	str_rc alias(prime->getAlias());

	buf << ' ' << this->quoteName(tname);
	if (alias.size())
	{
		buf << ' ' << alias;
	}

	str_rc name;


	htab_walk wk;
	auto join_info = wk.value();
	for(wk.start(joins); wk.ok(); wk.next())
	{
		//zend_printf("buflen %ld, %ld\n", buf.size(), buf.len());
		//showmem("join_info", join_info);
		JoinInfo* ji = zval_toc<JoinInfo>(join_info);

		IColumns* left = ji->leftTable();
		str_rc  l_alias(left->getAlias());
		
		buf << " " << ji->joinTypeStr() << " JOIN ";

		int partid = left->getPartId();
		
		if (partid == SqlPartId::TCOL_PID)
		{
			name = this->quoteName(left->getName());
			//zend_printf("partid %lx %s\n", partid,name.data());
			buf << ' ' << name;
		}
		else  if (partid == SqlPartId::ICOL_PID)
		{
			TColumns* tcol = static_cast<TColumns*>(left);
			obj_rc  owner = tcol->getOwner();
			// Owner is an "Operation" , usually a Select
			// TODO: owner must exist?
			obj_rc plist_mgr(owner.call(SQSTR.get_sql_params));
		    ParamList* plist = zobj_toc<ParamList>(plist_mgr);
		    str_rc   sub_sql = plist->getSql();

			//val_rc subq_sql = subq.call(SQSTR.getsql);
			buf << " (" << sub_sql << ")";
		}
		buf << ' ' << l_alias;

		htab_ptr  expr = ji->getConditions();
		if (expr.size())
		{
			buf << " ON ";
			IColumns* right = ji->rightTable();
			str_rc r_alias;
			if (right != nullptr)
			{
				r_alias = right->getAlias();
			}
			htab_walk jwk;

			auto jexp_obj = jwk.value();
			int jix = 0;
			for(jwk.start(expr); jwk.ok(); jwk.next(), jix++)
			{
				JoinExpr* jex = zval_toc<JoinExpr>(jexp_obj);
				temp = jex->emit(jix, &bind, l_alias, r_alias );
				if (temp.has_errors())
				{
					result = std::move(temp);
					return result;
				}
				buf << temp.value_;
			}
		}
	}
	result.value_ = buf.zstr();
	return result;


}

str_return 
ISql::select_jt(Bindings& bind, JoinTables* jt)
{
	str_return result;
	str_return temp;

	str_buf buf;

	val_ptr aggregate = bind.get(ISql::SQL_AGGREGATE);
	val_ptr distinct = bind.get(ISql::SQL_DISTINCT);

	if (aggregate.isArray())
	{
		htab_ptr agg(aggregate.zarray());
		str_rc function = agg.get(SQSTR.function);

		buf << ' ' << function << '(';

		if (distinct.isTrue())
		{
			buf << " DISTINCT ";
		}
		
		val_ptr columns = agg.get(SQSTR.columns);
		if (columns.isArray())
		{

			buf << this->columns(columns.zarray());
		}

		str_ptr alias = agg.get(SQSTR.alias);
		if (!alias.ok())
		{
			alias = function;
		}
		buf << ") AS " << this->quoteName(alias);

		//zend_printf("Aggregate %s\n" , buf.data());
	}
	else 
	{
		if (distinct.isTrue())
		{
			buf << " DISTINCT ";
		}
		htab_rc col_list_mgr;
		htab_rw col_list(col_list_mgr);

		htab_ptr tables = jt->getTables();
		htab_walk wk;
		
		auto tc = wk.value();
		
		

		for(wk.start(tables); wk.ok(); wk.next())
		{
			//str_ptr tkey(wk.key().zstr());
			//zend_printf("Join order = %s\n", tkey.data());
			IColumns* icol = zval_toc<IColumns>(tc);
			this->columnsTC(icol, col_list);
		}
		val_rc columns = implode(SQSTR.comma_char, col_list);

		buf << ' ' << val_ptr(columns).zstr();
	}
	temp = this->fromJT(bind,jt);
	if (temp.has_errors())
	{
		result = std::move(temp);
		return result;
	}
	buf << temp.value_;
	result.value_ = buf.zstr();
	return result;
}


static str_return plimit(const char* s, ParamList* plist, val_ptr value, str_buf& buf)
{
	str_return result;
	if (!value.isNull())
	{
		result = plist->paramLiteral(value);
		if (result.has_errors())
		{
			return result;
		}
		buf << s << result.value_;
	}
	return result;
}

str_return 
ISql::limit(ParamList* plist, htab_ptr ltab)
{
	val_ptr limit_val = ltab.get(SQSTR.limit);
	val_ptr offset_val = ltab.get(SQSTR.offset);
	str_return result;
	str_buf buf;
	
	//showmem("limit_val", limit_val);
	//showmem("offset_val", offset_val);

	result = plimit(" LIMIT ", plist, limit_val, buf);

	if (!result.has_errors())
	{
		result = plimit(" OFFSET ", plist, offset_val, buf);
	}
	if (!result.has_errors())
	{
		result.value_ = buf.zstr();
	}
	return result;
}

obj_return
ISql::select(Bindings& bind)
{
	obj_return result;
	str_return temp;

	JoinTables* from = bind.getJoinTables();

	str_buf buf;

	str_return what_ret = this->select_jt(bind, from);
	if (what_ret.has_errors())
	{
		result = std::move(what_ret);
		return result;
	}

	buf << "SELECT" << what_ret.value_;

	// TODO: still need this, check for old-style join specification? 
	/*
	zval_own join = bind.get(ISql::SQL_JOIN);
	if (!join.isNull())
	{
		buf << this->join(join);
	}
	*/

	result = bind.getParamList();
	if (result.has_errors())
	{
		return result;
	}

	ParamList* plist = zobj_toc<ParamList>(result.value_);

	val_ptr where = bind.get(SQL_WHERE);
	if (where.isArray())
	{
		temp = this->where(bind, where.zarray());
		if (temp.has_errors())
		{
			result = std::move(temp);
			return result;
		}
		buf << " WHERE" << temp.value_;
	}

	val_ptr order = bind.get(SQL_ORDER);
	if (order.isArray())
	{
		temp.value_ = this->orderBy(order.zarray());
		buf << " ORDER BY" << temp.value_;
	}

	val_ptr limit = bind.get(SQL_LIMIT);

	if (limit.isArray())
	{
		str_return temp = this->limit(plist, limit.zarray());
		if (temp.has_errors())
		{
			result = std::move(temp);
			return result;
		}
		buf << temp.value_;
	}

	str_rc sql = buf.zstr();

	plist->setSql(sql);
	//showstr("sql", sql);
	plist->useOwnValues();
	return result;
}

obj_return
ISql::update(Bindings& bind)
{
	str_buf buf;
	obj_return result;

	JoinTables* joins = bind.getJoinTables();
	htab_ptr   tables = joins->getTables();
	result = bind.getParamList();

	obj_rc& pobj = result.value_;

	htab_walk wk;

	if (!wk.start(tables))
	{	
		result.error() << "No update table";
		return result;
	}

	auto current = wk.value();
	IColumns* tcol = zval_toc<IColumns>(current);

	buf << "UPDATE " << this->quoteName(tcol->getName()) << " SET";

	val_ptr upset = bind.get(SQL_UPDATE);
	

	ParamList* plist = zobj_toc<ParamList>(pobj);

	if (upset.isArray())
	{
		int ix = 0;
		for (wk.start(upset.zarray()); wk.ok(); wk.next(), ix++)
		{
			htab_ptr pair = current.zarray();
			str_ptr col_name = pair.get(SQSTR.column);
			val_ptr col_value = pair.get(SQSTR.value);

			if (ix > 0)
			{
				buf << ", ";
			}
			else {
				buf << ' ';
			}
			buf << this->quoteName(col_name) << " = " << plist->addParam(col_value);
		}
	}
	else {
		// This is for a single table update
		obj_rc prime = joins->getPrime();
		IColumns* icol = zobj_toc<IColumns>(prime);

		htab_ptr uset = icol->getColNames();
		val_rc bfalse;
		bfalse.set_bool(false);

		int ix = 0;
		auto name = wk.key();
		for(wk.start(uset); wk.ok(); wk.next(), ix++)
		{
			if (ix > 0)
			{
				buf << ", ";
			}
			else {
				buf << ' ';
			}
			buf << this->quoteName(name.zstr()) << " = " << plist->addParam(bfalse);
		}
	}

	val_ptr where_val = bind.get(SQL_WHERE);

	str_return temp = this->where(bind, where_val.zarray());

	if (temp.has_errors())
	{
		result = std::move(temp);
		return result;
	}
	if (where_val.isArray())
	{
		buf << " WHERE" << temp.value_;
	}

	str_rc sql = buf.zstr();
	plist->setSql(sql);
	plist->useOwnValues();
	return result;
}

str_rc 
ISql::valuesDefault()
{
	return SQSTR.default_values;
}

str_rc // static 
ISql::tableClass(str_ptr s)
{
	const char* c = s.data();
	size_t slen = s.size();

	str_buf buf;

	bool upperNext = true;
	size_t ix = 0;

	while (ix < slen)
	{
		char add = c[ix];
		ix++;

		switch(add)
		{
		case '-':
		case '_':
			upperNext = true;
			break;
		default:
			if (upperNext)
			{
				upperNext = false;
				add = toupper(add);
			}
			buf << add;
			break;
		}
	}
	return buf.zstr();
}

str_rc
ISql::entityClass(str_ptr s)
{
	return ISql::tableClass(s);
}

str_return
ISql::where(Bindings &bind, htab_ptr wtab)
{
	str_return result;


	str_buf buf;

	// TODO: jtables set wether needed or not!
	JoinTables* jtab = bind.getJoinTables();

	htab_ptr jtables(jtab->getTables());

	obj_return pobjret = bind.getParamList();
	if (pobjret.has_errors())
	{
		result = std::move(pobjret);
		return result;
	}
	ParamList* params = zobj_toc<ParamList> (pobjret.value_);

	str_rc bop; // sql boolean operator eg "AND"
	val_ptr wcol; // column name or object, being processed from where entry
	val_ptr value; // another temporary zval
	
	val_ptr not_value; // boolean true or false for "NOT"

	val_ptr harray; // a data array
	obj_rc part; // part, some kind of SqlPartId.
	SqlPart_return pret;

	SqlPartId* sqlpart;
	str_return temp;

	int partid; // vaguely obsolete and trad. way of identifying the part.
	str_rc col_name; // column name as string

	htab_walk wk;
	auto wix = wk.key();
	auto where_zval = wk.value();

	for(wk.start(wtab); wk.ok(); wk.next())
	{
		//showmem("where_zval", where_zval);
		htab_ptr where_tab(where_zval.zarray());

		if (wix.zlong() > 0)
		{
			bop = where_tab[SQSTR.boolean];
			buf << ' ' << bop;
		}
		wcol = where_tab[SQSTR.column];
		if (wcol.isObject())
		{
			SqlPart_return partret = getPartObj(wcol);
			if (partret.has_errors())
			{
				result = std::move(partret);
				return result;
			}
			SqlPartId* sqlpart = partret.value_;

			partid = sqlpart->getPartId();
			if (partid ==  SqlPartId::TA_PID)
			{
				TableAttr* ta = static_cast<TableAttr*>(sqlpart);
				str_buf coltemp;
				coltemp << ta->getTable() << '.' << this->quoteName(ta->getAttr());
				col_name = coltemp.zstr();
			}
			else if (partid == SqlPartId::JE_PID)
			{
				JoinExpr* je = static_cast<JoinExpr*>(sqlpart);
				temp = je->emit(0, &bind, zend_empty_string, zend_empty_string);
				if (temp.has_errors())
				{
					result = std::move(temp);
					return result;
				}
				buf << temp.value_;
				continue; // next where entry
			}
		}
		else {
			// TODO: expecting string? 
			//assert(wcol.isString());
			col_name = this->quoteName(wcol.zstr());
		}
		// presume finished with setting col_name
		wcol = where_tab[SQSTR.typekey];
		// original string
		auto wtype = wcol.vstr();

		if (wtype == "basic") 
		{
			bop = where_tab[SQSTR.operator_key];

			buf << ' ' << col_name << ' ' << bop;

			value = where_tab[SQSTR.valuekey];

			if (value.isObject())
			{ 
				pret = getPartObj(value);
				if (pret.has_errors())
				{
					result = std::move(pret);
					return result;
				}
				sqlpart = pret.value_;

				partid = sqlpart->getPartId();
				// 4 kinds at this level
				if (partid == SqlPartId::TA_PID)
				{
					TableAttr* ta = static_cast<TableAttr*>(sqlpart);
					str_ptr tname = ta->getTable();
					obj_ptr icol_obj = jtables[tname]; // IColumns always by unique name??
					
					IColumns* icol = zobj_toc<IColumns>(icol_obj);
					buf << ' ' << icol->getAlias() << '.' << ta->getAttr();
				}
				else if (partid == SqlPartId::LIT_PID)
				{
					Literal* lit = static_cast<Literal*>(sqlpart);
					buf << ' ' << lit->toString();
				}
				else if (partid == SqlPartId::PARAM_PID)
				{
					Param* pa = static_cast<Param*>(sqlpart);
					value = pa->getValue();
					buf << ' ' << params->addParam(value);
				}
				else if (partid == SqlPartId::EXPR_PID)
				{
					Expr* exp = static_cast<Expr*>(sqlpart);
					buf << ' ' << exp->toString();
				}
			}
			else { // value is some kind of Raw Param value??
				buf << ' ' << params->addParam(value);
			}
		}
		else if (wtype == "nested")
		{
			harray = where_tab[SQSTR.nested];
			//TODO: sure this will be array??
			temp = this->where(bind, harray.zarray());
			if (temp.has_errors())
			{
				result = std::move(temp);
				return result;
			}
			buf << " (" << temp.value_ << ')';
		}
		else if (wtype == "isNull") //NB: case sensitive
		{
			not_value = where_tab[SQSTR.not_key];
			buf << ' ' << col_name << " IS";
			if (not_value.isTrue()) {
				buf << " NOT";
			}
			buf << " NULL";
		}
		else if (wtype == "between")
		{
			not_value = where_tab[SQSTR.not_key];
			buf << ' ' << col_name;
			if (not_value.isTrue())
			{
				buf << " NOT";
			}
			harray = where_tab[SQSTR.value];
			if (!harray.isArray() || (harray.size() < 2))
			{
				result.error() << "SQL BETWEEN: requires needs 2 values";
				return result;
			}
			htab_ptr duo(harray.zarray());
			buf << " BETWEEN " << params->addParam(duo[int(0)]) << " AND " << params->addParam(duo[int(1)]);
		}
		else if (wtype == "in")
		{
			buf << ' ' << col_name;
			not_value = where_tab[SQSTR.not_key];
			if (not_value.isTrue())
			{
				buf << " NOT";
			}
			buf << " IN";
			value = where_tab[SQSTR.values_key];

			if (!value.isArray())
			{
				result.error() << "IN needs values array";
				return result;
			}
			buf << " ("  << params->addParamList(value) << ')';
		}
		else if (wtype =="raw")
		{
			str_ptr key = wcol.zstr();

			value = where_tab[key];
			pret = getPartObj(value);
			if (pret.has_errors())
			{
				result = std::move(pret);
				return result;
			}
			sqlpart = pret.value_;
			partid = sqlpart->getPartId();
			if (partid != SqlPartId::EXPR_PID)
			{
				result.error() << "'raw' needs Expr object";
				return result;
			}
			Expr* exp = static_cast<Expr*>(sqlpart);
			buf << exp->toString();
			value = where_tab[SQSTR.values_key]; // if associated parameters
			if (value.isArray()) {
				htab_ptr vlist(value);

				htab_rw plist(params->getParams());

				plist.merge(vlist);
				params->setParams(plist);
			}
		}
	}
	result = buf.zstr();
	return result;
}

//static 
zend_class_entry* 
ISql::register_class()
{

	zintf_ce_Sql_IfSql = register_class_Wcd_Sql_IfSql();

	zclass_isql = register_class_Wcd_Sql_ISql(zintf_ce_Sql_IfSql);

	ISql::omg.classEntry(zclass_isql);

	Postgres::register_class(zclass_isql);

	return zclass_isql;
}


}//namespace wcd

using namespace wcd;
using namespace zpp;

	

	

	

	


/* public function delete(Bindings $bind) : ParamList {} */
ZEND_METHOD(Wcd_Sql_ISql, deleteSql)
{
	zval* bind;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(bind, zclass_bindings)
	ZEND_PARSE_PARAMETERS_END();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);
	Bindings& refbind = *zval_toc<Bindings>(bind);

	obj_return plist = cobj->deleteSql(refbind);
	plist.throw_errors();
	plist.value_.move_zv(return_value);
}

/* public function emit(
        IfSqlPart $sp, 
        Bindings $bind, 
        ?string $lalias = null, 
        ?string $ralias = null): string {} */
ZEND_METHOD(Wcd_Sql_ISql, emit)
{
	zval* part;
	zval* bind;
	zend_string* lalias = nullptr;
	zend_string* ralias = nullptr;

	ZEND_PARSE_PARAMETERS_START(2,4)
	Z_PARAM_OBJECT_OF_CLASS(part, zclass_sql_ifipart)
	Z_PARAM_OBJECT_OF_CLASS(bind, zclass_bindings)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(lalias)
	Z_PARAM_STR_OR_NULL(ralias)
	ZEND_PARSE_PARAMETERS_END();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);
	Bindings* pbind = zval_toc<Bindings>(bind);

	str_return result = cobj->emit(part, pbind, lalias, ralias);

	result.throw_errors();
	result.value_.move_zv(return_value);

}

/* public static function tableClass(string $uname) : string {} */
ZEND_METHOD(Wcd_Sql_ISql, tableClass)
{
	zend_string* table;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(table)
	ZEND_PARSE_PARAMETERS_END();

	str_rc result = ISql::tableClass(table);
	result.move_zv(return_value);
}

/*public function entityClass(string $table) : string {}*/
ZEND_METHOD(Wcd_Sql_ISql, entityClass)
{
	zend_string* table;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(table)
	ZEND_PARSE_PARAMETERS_END();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);
	str_rc result = cobj->entityClass(table);
	result.move_zv(return_value);
}

/* public function getTruncateSql(): string {} */
ZEND_METHOD(Wcd_Sql_ISql, getTruncateSql)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);
	str_rc result = cobj->getTruncateSql();
	result.move_zv(return_value);
}

/*public function insert(Bindings $bind) : ParamList {}*/
ZEND_METHOD(Wcd_Sql_ISql, insert)
{
	zval* bind;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(bind, zclass_bindings)
	ZEND_PARSE_PARAMETERS_END();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);
	Bindings* refbind = zval_toc<Bindings>(bind);

	obj_return plist = cobj->insert(*refbind);
	plist.throw_errors();

	plist.value_.move_zv(return_value);
}

/* public function quoteName(string $name): string {} */
ZEND_METHOD(Wcd_Sql_ISql, quoteName)
{
	zend_string* name;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);

	str_rc qname = cobj->quoteName(name);
	qname.move_zv(return_value);
}

/* public function select(Bindings $bind) : ParamList {} */
ZEND_METHOD(Wcd_Sql_ISql, select)
{
	zval* bind;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(bind, zclass_bindings)
	ZEND_PARSE_PARAMETERS_END();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);
	Bindings& refbind = *zval_toc<Bindings>(bind);

	obj_return plist = cobj->select(refbind);
	plist.throw_errors();
	plist.value_.move_zv(return_value);
}

/* public function seqLastValue(string $seq_name): string {} */
ZEND_METHOD(Wcd_Sql_ISql, seqLastValue)
{
	zend_string* name;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);

	str_rc qname = cobj->seqLastValue(name);
	qname.move_zv(return_value);
}

/* public function setSeqValue(int $value, array $data): string {} */
ZEND_METHOD(Wcd_Sql_ISql, setSeqValue)
{
	zend_long    value;
	zval* 		 data;
	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_LONG(value)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);

	str_return sqlret = cobj->setSeqValue(value, data);
	sqlret.throw_errors();
	sqlret.value_.move_zv(return_value);
}

/* public function truncate(Bindings $bindings) : string {} */
ZEND_METHOD(Wcd_Sql_ISql, truncate)
{
	zval* bind;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(bind, zclass_bindings)
	ZEND_PARSE_PARAMETERS_END();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);
	Bindings& refbind = *zval_toc<Bindings>(bind);

	str_return result = cobj->truncate(refbind);
	result.throw_errors();
	result.value_.move_zv(return_value);
}

/*public function update(Bindings $bind) : ParamList {}*/
ZEND_METHOD(Wcd_Sql_ISql, update)
{
	zval* bind;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(bind, zclass_bindings)
	ZEND_PARSE_PARAMETERS_END();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);
	Bindings& refbind = *zval_toc<Bindings>(bind);

	obj_return pobjret = cobj->update(refbind);
	pobjret.throw_errors();
	pobjret.value_.move_zv(return_value);
}

/* public function valuesDefault(): string {} */
ZEND_METHOD(Wcd_Sql_ISql, valuesDefault)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);
	str_rc result = cobj->valuesDefault();
	result.move_zv(return_value);
}

#endif