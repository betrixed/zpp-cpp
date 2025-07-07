#ifndef SQL_ISQL_CPP
#define SQL_ISQL_CPP

#ifndef SQL_ISQL_H
#include "sql_isql.h"
#endif

#include <ctype.h>

#ifndef WCD_RUNSQL_H
#include "runsql.h"
#endif

#ifndef WCD_MODEL_H
#include "model.h"
#endif

#ifndef SQL_ARGINFO_H
#define SQL_ARGINFO_H
extern "C" {
	#include "stub/sqlipart_arginfo.h"
};
#endif

namespace wcd {

zend_class_entry* zintf_ce_Sql_IfSql;

base_obj_mgr<ParamList> 	ParamList::omg;
base_obj_mgr<ISql>  		ISql::omg;
base_obj_mgr<Bindings>  	Bindings::omg;
base_obj_mgr<JoinTables> 	JoinTables::omg;
base_obj_mgr<JoinInfo> 		JoinInfo::omg;

zend_class_entry* zclass_isql;
zend_class_entry* zclass_join_info;
zend_class_entry* zclass_join_tables;
zend_class_entry* zclass_bindings;
zend_class_entry* zclass_param_list;

using namespace zpp;

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void ParamList::debug_info(htab_write di)
{
	if (!params_.isNull())
		di.set(SQSTR.params, params_);
	if (!ret_values_.isNull())
		di.set(SQSTR.returns_str, ret_values_);
	if (!sql_.isNull())
		di.set(SQSTR.sql, sql_);
	if (!val_params_.isNull())
		di.set(SQSTR.values_key, val_params_);

	di.set(SQSTR.driver, driver_);
}

void 
ParamList::construct(zval_user driver)
{
	driver_ = driver.zobject();
}

zstr_mgr 
ParamList::paramStr(int ct)
{
	return driver_.call(SQSTR.param, zval_mgr(ct));
}

zstr_mgr 
ParamList::addParam(zval_user value)
{
	htab_write hw(params_);
	hw.push_back(value);

	return paramStr((int) params_.size());
}


void 
ParamList::useOwnValues()
{
	val_params_ = params_;
}

zstr_mgr 
ParamList::paramLiteral(zval_user value)
{
	zval_mgr temp;

	if (value.isObject())
	{
		zobj_user obj(value.zobject());

		if (obj.instanceof(zclass_sql_ifipart)) 
		{
			SqlPartId* part = zobj_toc<SqlPartId>(obj);

			switch(part->getPartId())
			{
			case SqlPartId::PARAM_PID:
				temp = static_cast<Param*>(part)->getValue();
				return this->addParam(temp);
			case SqlPartId::LIT_PID:
				temp = static_cast<Literal*>(part)->getValue();
				return zval_user(temp).to_zstr();
			}
		}
	}
	else {
		return this->addParam(value);
	}
	throw std::runtime_error("Unhandled paramLiteral argument");
}

zstr_mgr 
ParamList::addParamList(htab_read values)
{
	zstr_buffer buf;

	int ix = (int) params_.size();

	htab_walk wk;
	auto item = wk.value();

	int bufct = 0;

	htab_write pw(params_);

	for(wk.start(values); wk.ok(); wk.next(), bufct++)
	{
		pw.push_back(item);
		ix++;
		if (bufct > 0)
		{
			buf << ',';
		}
		buf << paramStr(ix);
	}
	return buf.zstr();
}

zstr_mgr 
ParamList::makeList(int start, int count)
{
	zstr_buffer buf;
	for(int ix = start; ix <= count; ix++)
	{
		if (ix > start)
		{
			buf << ',';
		}
		buf << paramStr(ix);
	}
	return buf.zstr();
}

void
ParamList::wipe()
{
	sql_.init();
	params_.reset();
	ret_values_.reset();
	val_params_.reset();
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void 
JoinInfo::construct(zobj_user ltable, zobj_user rtable, int jtype)
{
	this->leftTable_ = ltable;
	this->rightTable_ = rtable;
	this->joinType_ = jtype;
}

IColumns* 
JoinInfo::leftTable()
{
	if (leftTable_.isNull())
		return nullptr;
	return zobj_toc<IColumns>(leftTable_);
}

IColumns* 
JoinInfo::rightTable()
{
	if (rightTable_.isNull())
		return nullptr;
	return zobj_toc<IColumns>(rightTable_);
}

// add a join expression
void 
JoinInfo::add(zval_user lexp, zval_user rexp, int jtype, int logic)
{
	if ( (logic == JoinExpr::B_NULL) && joinExpr_.size())
	{
		logic = JoinExpr::B_AND;
	}

	auto jobj = JoinExpr::omg.new_zobj();
	JoinExpr* je = zobj_toc<JoinExpr>(jobj);

	je->construct(lexp, rexp, jtype, logic);

	addExpr(jobj);

	//jw.push_back(jobj);
}

void 
JoinInfo::addExpr(zobj_user jexpr)
{
	htab_write(joinExpr_).push_back(jexpr);
}

zstr_user// static
JoinInfo::joinStr(int jid)
{
	return SQSTR.joinstr[jid];
}

zstr_user  
JoinInfo::joinTypeStr() const
{
	return SQSTR.joinstr[joinType_];
}

int //static
JoinInfo::getJoinType(zstr_user s)
{
	if (s.size() == 0)
	{
		return 0;
	}
	const char* c = s.data();
	int c1 = toupper(c[0]);
	switch(c1)
	{
	case 'I':
		return J_INNER;
	case 'L':
		return J_LEFT;
	case 'R':
		return J_RIGHT;
	case 'F':
		return J_FULL;
	default:
		break;
	}
	throw std::runtime_error("Join Type string not recognized");

}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


void
JoinTables::debug_info(htab_write di)
{
	di.set(SQSTR.by_alias, byAlias_);
	di.set(SQSTR.results, results_);
	di.set(SQSTR.where, where_);
	di.set(SQSTR.joins, joins_);
	di.set(SQSTR.orderby, orderby_);
	di.set(SQSTR.prime, prime_);
	di.set(SQSTR.model, model_);

}
void 
JoinTables::setPrime(zobj_user obj)
{
	this->addTable(obj);
	prime_ = obj;
}

zobj_mgr 
JoinTables::addJoin(zobj_user jiobj)
{
	zobj_mgr result(jiobj);

	htab_write(joins_).push_back(jiobj);

	JoinInfo* ji = zobj_toc<JoinInfo>(jiobj);

	this->addTable(ji->leftTable_);

	return result;
}

void 
JoinTables::addTable(zobj_user icol)
{
	IColumns* tc = zobj_toc<IColumns>(icol);
	zstr_user name = tc->getAlias();
	if (name.isNull())
	{	
		name = tc->getName();
	}
	if (name.isNull())
	{
		zend_throw_error(zend_ce_error, "addTable with no Alias or Name");
		return;
	}
	htab_write(byAlias_).set(name, icol);
}

void 
JoinTables::addResult(zobj_user ta)
{
	htab_write(results_).push_back(ta);
}

void 
JoinTables::addWhere(zval_user leftAttr, zval_user rightAttr, int op, int logic)
{
	zobj_mgr jobj = JoinExpr::omg.new_zobj();

	JoinExpr* je = zobj_toc<JoinExpr>(jobj);

	je->construct(leftAttr, rightAttr, op, logic);

	htab_write(where_).push_back(jobj);
}

zobj_mgr
JoinTables::getPivot()
{
	zobj_mgr result;

	if (joins_.size())
	{
		zobj_user first = joins_.get(int(0));

		JoinInfo* ji = zobj_toc<JoinInfo>(first);
		result = ji->leftTable_;
	}
	return result;
}

zobj_mgr
JoinTables::getTable(zstr_user name)
{
	zobj_mgr result;

	if (byAlias_.size())
	{
		htab_walk wk;

		auto tcobj = wk.value();

		for(wk.start(byAlias_); wk.next(); wk.ok())
		{
			if (tcobj.isObject())
			{
				zobj_user test(tcobj.zobject());
				IColumns* ic = zobj_toc<IColumns>(test);
				zstr_user tcname = ic->getName();
				if (zs_cmp(name, tcname)==0)
				{
					result = test;
					break;
				}
			}
		}
	}
	return result;
}

zobj_mgr
JoinTables::getTableAlias(zstr_user name)
{

	zobj_mgr result = byAlias_.get(name);
	return result;
}

void 
JoinTables::order(zstr_user name, bool ascend)
{
	htab_mgr pair;
	zval_mgr bval;

	bval.set_bool(ascend);
	htab_write hw(pair);

	hw.set(SQSTR.column, name);
	hw.set(SQSTR.ascend, bval);

	htab_write(orderby_).push_back(pair);
}

zobj_mgr //static
JoinTables::rowSplit(htab_read row, htab_read rename)
{
	htab_mgr rec_temp;
	htab_write rec(rec_temp);

	htab_mgr ok_temp;
	htab_write ok(ok_temp);

	zobj_mgr obj = class_data::std_object();

	{
		htab_walk wk;
		auto name = wk.key();
		auto val = wk.value();
		for(wk.start(row); wk.ok(); wk.next())
		{
			zval_user orig;

			zstr_mgr namekey(name.zstr());

			if (rename.try_fetch(namekey, orig))
			{
				zstr_mgr tail = namekey.substr(-3);
				htab_mgr tails;


				htab_write(tails).set(orig.zstr(), val);
				rec.set(tail, tails);
			}
			else {
				ok.set(namekey, val);
			}
		}
	}
	if (ok.size())
	{
		zval_mgr temp(ok);
		obj.property(SQSTR.ok, temp);
	}
	{
		htab_walk wk;
		auto key = wk.key();
		auto sub = wk.value();
		for(wk.start(rec); wk.ok(); wk.next())
		{
			obj.property(key.zstr(), sub);
		}
	}
	return obj;
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
zstr_mgr
ISql::quoteName(zstr_user name)
{
	zstr_mgr result;

	if (name.size() > 0)
	{
		char ic = *(name.data());
		if ((ic == '*')||(ic == '`'))
		{
			result = name;
			return result;
		}
		zstr_buffer buf;
		buf << '`' << name << '`';
		result = buf.zstr();
	}
	else {
		result = SQSTR.mysql_empty;
	}
	return result;
}

zstr_mgr 
ISql::columns(htab_read bd)
{
	zstr_buffer buf;

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
		zstr_mgr colname(name.zstr());

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
ISql::columnsTC(IColumns* tc, htab_write col_list)
{
	zstr_mgr cfrag;

	zstr_user alias = tc->getAlias();
	{
		htab_read colNames(tc->getColNames());
		htab_walk wk;
		auto name = wk.key();
		auto rename = wk.value();

		for(wk.start(colNames); wk.ok(); wk.next())
		{
			zstr_buffer fbuf;

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

			cfrag = std::move(fbuf);
			//showstr("cfrag:", cfrag);
			col_list.push_back(cfrag);
		}
	}
	{
		htab_read expr = tc->getExpr();
		htab_walk wk;
		auto exalias = wk.key();
		auto exfn = wk.value();

		for(wk.start(expr); wk.ok(); wk.next())
		{
			zstr_buffer fbuf;
			fbuf << exfn.zstr() << " as " << this->quoteName(exalias.zstr());
			cfrag = fbuf.zstr();
			//showstr("expr:", cfrag);
			col_list.push_back(cfrag);
		}
	}
}

// 
zstr_mgr
ISql::orderBy(htab_read obind)
{
	zstr_buffer buf;

	buf << ' ';

	htab_walk wk;

	auto ix = wk.key();
	auto order_tab = wk.value();

	for(wk.start(obind); wk.ok(); wk.next())
	{
		if (order_tab.isArray())
		{
			htab_read order(order_tab.zarray());
			if (ix.zlong() > 0)
			{
				buf << ", ";
			}

			zval_user col = order.get(SQSTR.column);

			if (col.isObject())
			{
				TableAttr* ta = zval_toc<TableAttr>(col);
				buf << ta->getTable() << '.' << this->quoteName(ta->getAttr());
			}
			else {
				buf << this->quoteName(col.zstr());
			}
			zval_user descend = order.get(SQSTR.descend);
			if (descend.isTrue()) {
				buf << " DESC";
			}
			else {
				buf << " ASC";
			}
		}
	}
	return buf.zstr();
}

zobj_mgr 
ISql::deleteSql(Bindings& bind)
{
	zstr_buffer buf;

	buf << "DELETE FROM";

	htab_read tables = this->getTables(bind);

	htab_walk pos;
	// first table
	// TODO: shouldn't first table be in prime_??
	pos.start(tables);

	zval_user tcobj = pos.value();

	IColumns* icol = zval_toc<IColumns>(tcobj);
	buf << ' ' << this->quoteName(icol->getName());

	zval_user wh( bind.get(SQL_WHERE));

	if (wh.isArray())
	{
		htab_read wbind( wh.zarray());

		if (wbind.size())
		{
			buf << " WHERE" << this->where(bind, wbind);
		}
	}

	zval_user order( bind.get(SQL_ORDER));

	if (order.isArray())
	{
		buf << " ORDER BY" << this->orderBy(order);
	}

	zobj_mgr paramList = bind.getParamList();
	ParamList* plist = zobj_toc<ParamList> (paramList);
	//htab_read params = plist->getParams();

	zstr_mgr sql = buf.zstr();
	plist->setSql(sql);
	plist->useOwnValues();

	return paramList;
}

SqlPartId* getPartObj(zval_user ret)
{
	if (ret.isObject())
	{
		zobj_user test(ret.zobject());
		if (test.instanceof(zclass_sql_ifipart))
		{
			return zobj_toc<SqlPartId>(test);
		}
	}
	throw std::runtime_error("SqlPartId object expected");
}

//* called from a JoinExpr
zstr_mgr
ISql::emit(zval_user sp, Bindings* bind, zstr_user lalias, zstr_user ralias)
{
 	SqlPartId* part = getPartObj(sp);
 	int partid = part->getPartId();

 	zstr_buffer buf;
 	zstr_mgr str;

 	switch(partid)
 	{
 	case SqlPartId::TA_PID:
 		{
 			TableAttr* ta = static_cast<TableAttr*>(part);
 			zstr_user ta_alias = ta->getTable();
 			zstr_user ta_name = ta->getAttr();

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
 			buf << " (" << je->emit(0, bind, lalias, ralias) << " )";
 		}
 		break;
 	case SqlPartId::PARAM_PID:
 		{
 			Param* p = static_cast<Param*>(part);
 			zval_user pvalue = p->getValue();
			zobj_mgr paramList = bind->getParamList();
			ParamList* list = zobj_toc<ParamList> (paramList);
 			buf << list->addParam(pvalue);
 		}
 		break;
 	default:
 		throw std::runtime_error("Unmatched partid in emit");
 	}
 	
 	return buf.zstr();		
}

zstr_mgr 
ISql::getTruncateSql()
{
	return SQSTR.delete_from;
}

htab_read
ISql::getTables(Bindings& bind)
{
	JoinTables* jt = bind.getJoinTables();
	return jt->getTables();
}

zstr_mgr 
ISql::truncate(Bindings& bind)
{
	zstr_buffer buf;

	buf << this->getTruncateSql();

	JoinTables* jt = bind.getJoinTables();

	if (jt) {
		zobj_mgr pobj = jt->getPrime();
		if (pobj.ok())
		{
			IColumns* icol = zobj_toc<IColumns>(pobj);
			buf << ' ' << this->quoteName(icol->getName());
			return buf.zstr();
		}
	}
	throw std::runtime_error("Truncate table not set");
}

zstr_mgr
ISql::insert_col_params(Bindings& bind, htab_read rowbind)
{
	htab_walk wk;

	zstr_buffer ptext;
	zstr_buffer ctext;

	zstr_mgr place;

	zobj_mgr paramList = bind.getParamList();

	ParamList* params = zobj_toc<ParamList> (paramList);

	zval_mgr   zpass;

	int pcount = 0;
	int ix = 0;

	ctext << " (";

	auto key = wk.key();
	auto val = wk.value();
	zstr_mgr colname;

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
			htab_read vdata(val.zarray());
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
	zstr_mgr plist = ptext.zstr();

	ctext << ") VALUES (" << plist << ")";

	return ctext.zstr();
}

zstr_mgr
ISql::setSeqValue(int value, htab_read data)
{
	zstr_mgr seq_name = data.get(SQSTR.seq_key);

	if (!seq_name.isNull())
	{
		zstr_buffer buf;

		buf << "select setval('" << seq_name << "'," << value << ")";
		return buf.zstr();
	}
	throw std::runtime_error("Missing name for setSeqValue");
}

zstr_mgr
ISql::seqLastValue(zstr_user seq)
{
	zstr_buffer buf;

	buf << "select LASTVAL(" << seq << ")";

	return buf.zstr();
}

IColumns* getIColumns(zval_user zv)
{
	if (zv.isObject())
	{
		IColumns* icol = zobj_toc<IColumns>(zv.zobject());
		return icol;
	}
	else {
		throw std::runtime_error("IColumns object expected");
	}
	return nullptr;
}

void extract_params(htab_read rowbind, htab_read plist, htab_write params);

void extract_params(htab_read rowbind, htab_read plist, htab_write params)
{
	//showarray("after create", ret_params);

	htab_walk wk;
	auto pix = wk.value();

	htab_mgr row_values = htab_mgr::getValues(rowbind);

	for(wk.start(plist); wk.ok(); wk.next())
	{
		zval_user pvalue = row_values.get(pix.zlong());
		params.push_back(pvalue);
	}
	return;
}

zobj_mgr 
ISql::insert(Bindings& bind)
{
	zobj_mgr result;

	zstr_buffer buf;

	buf << "INSERT INTO";

	htab_read jtables = this->getTables(bind);

	if (!jtables.size()) {
		zend_throw_error(zend_ce_error, "No tables set");
		return result;
	}

	htab_walk pos;
	pos.start(jtables);
	
	IColumns* icol = getIColumns(pos.value());

	buf << ' ' << this->quoteName(icol->getName());

	htab_mgr sql_insert;

	if (!bind.getArray(ISql::SQL_INSERT, sql_insert))
	{
		zend_throw_error(zend_ce_error, "Insert table not set");
		result = buf.zstr();
		return result;
	}


	htab_walk insert_wk;
	htab_read  rowbind;


	int pcount = 0;
	if (insert_wk.start(sql_insert))
	{
		rowbind = insert_wk.value().zarray();
		
		size_t rsize = rowbind.size();

		if (rsize)
		{
			pcount += rsize;
			buf << this->insert_col_params(bind, rowbind);
		}
	}
	if (pcount == 0)
    {
    	//zend_printf("\nValues Default\n");
    	zobj_mgr self(this->vobj());
    	zval_mgr dtext = self.call(SQSTR.valuesdefault);
		buf << ' ' << zval_user(dtext).zstr() << ' ';
    }
	htab_mgr rettab;
	zval_user  valset;
	if (bind.getArray(ISql::SQL_RETURN, rettab))
	{
		buf << " RETURNING ";
		valset = rettab[int(0)];
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

	zobj_mgr   pobj = bind.getParamList();
	ParamList* plist = zobj_toc<ParamList>(pobj);

	htab_read params = plist->getParams();
	htab_mgr   ret_params_mgr;
	htab_write ret_params(ret_params_mgr);

	if (params.size())
	{
		extract_params(rowbind, params, ret_params);
		//ret_params.show_data("ret_params");

		if (sql_insert.size() > 1)
		{
			htab_mgr   multirow_mgr;
			htab_write multirow(multirow_mgr);
			

			multirow.push_back(ret_params);
			while(insert_wk.next())
			{
				rowbind  = insert_wk.value().zarray();

				htab_mgr   pset_mgr;
				htab_write pset(pset_mgr);

				extract_params(rowbind, params, pset);
				multirow.push_back(pset);
			}
			ret_params_mgr = multirow;
		}

	}
	zstr_mgr sql = buf.zstr();

	plist->setSql(sql);

	if (ret_params.size())
	{
		plist->setValues(ret_params_mgr);
	}
	if (rettab.size())
	{
		plist->setReturns(rettab);
	}
	//showobj("return pobj", pobj);
	return pobj;
}

zstr_mgr 
ISql::fromJT(Bindings& bind, JoinTables* jt)
{
	zstr_buffer buf;
	buf << " FROM";
	//zend_printf("buflen %ld, %ld\n", buf.size(), buf.len());

	htab_read joins(jt->getData());

	zobj_mgr prime_obj(jt->getPrime());

	IColumns* prime = zobj_toc<IColumns>(prime_obj);

	zstr_mgr tname(prime->getName());
	zstr_mgr alias(prime->getAlias());

	buf << ' ' << this->quoteName(tname);
	if (alias.size())
	{
		buf << ' ' << alias;
	}

	zstr_mgr name;
	htab_walk wk;
	auto join_info = wk.value();
	for(wk.start(joins); wk.ok(); wk.next())
	{
		//zend_printf("buflen %ld, %ld\n", buf.size(), buf.len());
		//showmem("join_info", join_info);
		JoinInfo* ji = zval_toc<JoinInfo>(join_info);

		IColumns* left = ji->leftTable();
		zstr_mgr  l_alias(left->getAlias());
		
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
			zobj_mgr  subq(tcol->getOwner());
			//showobj("subq owner",subq);
			zval_mgr subq_sql = subq.call(SQSTR.getsql);
			buf << " (" << zval_user(subq_sql).zstr() << ")";
		}
		buf << ' ' << l_alias;

		htab_read  expr = ji->getConditions();
		if (expr.size())
		{
			buf << " ON ";
			IColumns* right = ji->rightTable();
			zstr_mgr r_alias;
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
				name = jex->emit(jix, &bind, l_alias, r_alias );
				buf << name;
			}
		}
	}

	return buf.zstr();


}

zstr_mgr 
ISql::select_jt(Bindings& bind, JoinTables* jt)
{
	zstr_buffer buf;

	zval_user aggregate = bind.get(ISql::SQL_AGGREGATE);
	zval_user distinct = bind.get(ISql::SQL_DISTINCT);

	if (aggregate.isArray())
	{
		htab_read agg(aggregate.zarray());
		zstr_mgr function = agg.get(SQSTR.function);

		buf << ' ' << function << '(';

		if (distinct.isTrue())
		{
			buf << " DISTINCT ";
		}
		
		zval_user columns = agg.get(SQSTR.columns);
		if (columns.isArray())
		{

			buf << this->columns(columns.zarray());
		}

		buf << ") AS " << this->quoteName(function);
	}
	else 
	{
		if (distinct.isTrue())
		{
			buf << " DISTINCT ";
		}
		htab_mgr col_list_mgr;
		htab_write col_list(col_list_mgr);

		htab_read tables = jt->getTables();
		htab_walk wk;
		
		auto tc = wk.value();
		
		

		for(wk.start(tables); wk.ok(); wk.next())
		{
			//zstr_user tkey(wk.key().zstr());
			//zend_printf("Join order = %s\n", tkey.data());
			IColumns* icol = zval_toc<IColumns>(tc);
			this->columnsTC(icol, col_list);
		}
		zval_mgr columns = implode(SQSTR.comma_char, col_list);

		buf << ' ' << zval_user(columns).zstr();
	}
	buf << this->fromJT(bind,jt);

	return buf.zstr();
}




zstr_mgr 
ISql::limit(ParamList* plist, htab_read ltab)
{
	zval_user limit_val = ltab.get(SQSTR.limit);
	zval_user offset_val = ltab.get(SQSTR.offset);

	zstr_buffer buf;

	if (!limit_val.isNull())
	{
		buf << " LIMIT " << plist->paramLiteral(limit_val);
	}
	if (!offset_val.isNull())
	{
		buf << " OFFSET " << plist->paramLiteral(offset_val);
	}
	return buf.zstr();

}

zobj_mgr
ISql::select(Bindings& bind)
{
	JoinTables* from = bind.getJoinTables();

	zstr_buffer buf;

	zstr_mgr what = this->select_jt(bind, from);

	buf << "SELECT" << what;

	// TODO: still need this, check for old-style join specification? 
	/*
	zval_own join = bind.get(ISql::SQL_JOIN);
	if (!join.isNull())
	{
		buf << this->join(join);
	}
	*/
	zval_user where = bind.get(SQL_WHERE);
	if (where.isArray())
	{
		buf << " WHERE" << this->where(bind, where.zarray());
	}

	zval_user order = bind.get(SQL_ORDER);
	if (order.isArray())
	{
		buf << " ORDER BY" << this->orderBy(order.zarray());
	}

	zval_user limit = bind.get(SQL_LIMIT);

	zobj_mgr   pobj = bind.getParamList();

	ParamList* plist = zobj_toc<ParamList>(pobj);

	if (limit.isArray())
	{
		buf << this->limit(plist, limit.zarray());
	}

	zstr_mgr sql = buf.zstr();

	plist->setSql(sql);
	plist->useOwnValues();
	return pobj;
}

zobj_mgr
ISql::update(Bindings& bind)
{
	zstr_buffer buf;

	JoinTables* joins = bind.getJoinTables();
	htab_read   tables = joins->getTables();

	htab_walk wk;

	if (!wk.start(tables))
	{
		throw std::runtime_error("Update table not set");
	}

	auto current = wk.value();
	IColumns* tcol = zval_toc<IColumns>(current);

	buf << "UPDATE " << this->quoteName(tcol->getName()) << " SET";

	zval_user upset = bind.get(SQL_UPDATE);
	zobj_mgr result = bind.getParamList();

	ParamList* plist = zobj_toc<ParamList>(result);

	if (upset.isArray())
	{
		int ix = 0;
		for (wk.start(upset.zarray()); wk.ok(); wk.next(), ix++)
		{
			htab_read pair = current.zarray();
			zstr_user col_name = pair.get(SQSTR.column);
			zval_user col_value = pair.get(SQSTR.value);

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
		zobj_mgr prime = joins->getPrime();
		IColumns* icol = zobj_toc<IColumns>(prime);

		htab_read uset = icol->getColNames();
		zval_mgr bfalse;
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

	zval_user where_val = bind.get(SQL_WHERE);
	if (where_val.isArray())
	{
		buf << " WHERE" << this->where(bind, where_val.zarray());
	}

	zstr_mgr sql = buf.zstr();
	plist->setSql(sql);
	plist->useOwnValues();
	return result;
}

zstr_mgr 
ISql::valuesDefault()
{
	return SQSTR.default_values;
}

zstr_mgr // static 
ISql::tableClass(zstr_user s)
{
	const char* c = s.data();
	size_t slen = s.size();

	zstr_buffer buf;

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

zstr_mgr
ISql::entityClass(zstr_user s)
{
	return ISql::tableClass(s);
}

zstr_mgr
ISql::where(Bindings &bind, htab_read wtab)
{
	zstr_buffer buf;

	// TODO: jtables set wether needed or not!
	JoinTables* jtab = bind.getJoinTables();

	htab_read jtables(jtab->getTables());

	zobj_mgr paramList = bind.getParamList();
	ParamList* params = zobj_toc<ParamList> (paramList);

	zstr_mgr bop; // sql boolean operator eg "AND"
	zval_user wcol; // column name or object, being processed from where entry
	zval_user value; // another temporary zval
	
	zval_user not_value; // boolean true or false for "NOT"

	zval_user harray; // a data array
	zobj_mgr part; // part, some kind of SqlPartId.

	SqlPartId* sqlpart;

	int partid; // vaguely obsolete and trad. way of identifying the part.
	zstr_mgr col_name; // column name as string

	htab_walk wk;
	auto wix = wk.key();
	auto where_zval = wk.value();

	for(wk.start(wtab); wk.ok(); wk.next())
	{
		htab_read where_tab(where_zval.zarray());

		if (wix.zlong() > 0)
		{
			bop = where_tab[SQSTR.boolean];
			buf << ' ' << bop;
		}
		wcol = where_tab[SQSTR.column];
		if (wcol.isObject())
		{
			sqlpart = getPartObj(wcol);
			partid = sqlpart->getPartId();
			if (partid ==  SqlPartId::TA_PID)
			{
				TableAttr* ta = static_cast<TableAttr*>(sqlpart);
				zstr_buffer temp;
				temp << ta->getTable() << '.' << this->quoteName(ta->getAttr());
				col_name = std::move(temp);
			}
			else if (partid == SqlPartId::JE_PID)
			{
				JoinExpr* je = static_cast<JoinExpr*>(sqlpart);
				buf << je->emit(0, &bind, zend_empty_string, zend_empty_string);
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
				sqlpart = getPartObj(value);
				partid = sqlpart->getPartId();
				// 4 kinds at this level
				if (partid == SqlPartId::TA_PID)
				{
					TableAttr* ta = static_cast<TableAttr*>(sqlpart);
					zstr_user tname = ta->getTable();
					zobj_user icol_obj = jtables[tname]; // IColumns always by unique name??
					
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
			buf << " (" << this->where(bind, harray.zarray()) << ')';
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
				throw std::runtime_error("Between data needs 2 values");
			}
			htab_read duo(harray.zarray());
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
				std::runtime_error("IN needs values array");
			}
			buf << " ("  << params->addParamList(value) << ')';
		}
		else if (wtype =="raw")
		{
			zstr_user key = wcol.zstr();

			value = where_tab[key];
			sqlpart = getPartObj(value);
			partid = sqlpart->getPartId();
			if (partid != SqlPartId::EXPR_PID)
			{
				std::runtime_error("'raw' needs Expr object");
			}
			Expr* exp = static_cast<Expr*>(sqlpart);
			buf << exp->toString();
			value = where_tab[SQSTR.values_key]; // if associated parameters
			if (value.isArray()) {
				htab_read vlist(value);

				htab_write plist(params->getParams());

				plist.merge(vlist);
				params->setParams(plist);
			}
		}
	}
	return buf.zstr();
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Bindings::debug_info(htab_write di)
{
	di.set(SQSTR.data_key, data_);
	di.set(SQSTR.param_list, paramList_);
	di.set(SQSTR.isql, sql_);
	di.set(SQSTR.connect, db_);
}

void Bindings::add(int key, zval_user value)
{
	switch(key)
	{
	case ISql::SQL_FROM:
	case ISql::SQL_JOIN:

		if (value.isString())
		{
			zstr_user tname = value.zstr();
			zstr_mgr talias;

			zstr_mgr  lcase = tname.to_lower();
			int pos = lcase.find(std::string_view(" as "),0);
			if (pos >= 0)
			{
				talias = tname.substr(pos+4);
				tname = tname.substr(0, pos);
			}

			zobj_mgr tc_obj = TColumns::omg.new_zobj();
			TColumns* tc = zobj_toc<TColumns>(tc_obj);

			zval_mgr colnames(SQSTR.asterisk);

			tc->construct(tname, talias, colnames);

			JoinTables* joint = getJoinTables();
			joint->setPrime(tc_obj);
		}
		return;
	default:
		break;
	}
	addArray(key, value);
}

zobj_mgr
Bindings::primeJoin(zval_user tcol)
{
	zobj_mgr jobj = getJoins();
	JoinTables* jt = zobj_toc<JoinTables>(jobj);
	jt->setPrime(tcol.zobject());
	return jobj;
}

void 
Bindings::construct(zval_user sql, zval_user connect)
{
	sql_ = sql.zobject();
	db_ = connect.zobject();
}

JoinTables* 
Bindings::getJoinTables()
{
	zobj_mgr jobj = getJoins();
	return zobj_toc<JoinTables>(jobj);
}

zobj_mgr
Bindings::getJoins()
{
	zval_user from = get(ISql::SQL_FROM);
	if (from.isObject())
	{
		return zobj_mgr(from.zobject());
	}

	zobj_mgr result = JoinTables::omg.new_zobj();
	
	htab_write(data_).set((zend_long) ISql::SQL_FROM, result);
	return result;
}

bool
Bindings::getArray(int key, htab_mgr& value)
{
	zval_user result;
	if (data_.try_fetch(key, result))
	{
		if (result.isArray())
		{
			htab_mgr array(result.zarray());
			if (array.size())
			{
				value = array;
				return true;
			}
		}
	}
	return false;
}

void 
Bindings::addArray(int key, zval_user value)
{
	zval_user listown = data_.get(key);

	if (listown.isNull())
	{
		htab_mgr   list_mgr;
		htab_write list(list_mgr);

		list.push_back(value);

		htab_write(data_).set((zend_long)key, list_mgr);
	}
	else {
		htab_write vlist(listown);
		vlist.push_back(value);
	}
}

bool 
Bindings::addJoinData(htab_read data)
{
	zstr_mgr atype = data.get(SQSTR.typekey);
	if (atype.isNull())
	{
		atype = SQSTR.basic;
	}
	// translated from  some older PHP script of a join.
	// TODO: is this still needed?
	if (atype.vstr() == SQSTR.nested.vstr())
	{
		zobj_mgr prime;

		JoinTables* joins = getJoinTables();
		htab_read jia = joins->getData();

		size_t jcount = jia.size();
		if (jcount==0)
		{
			prime = joins->getPrime();
		}
		else {
			prime = jia[jcount-1]; // TODO: why not first?
		}
		zstr_user tname = data.get(SQSTR.table);

		zobj_mgr tcol_obj = TColumns::omg.new_zobj();
		TColumns* tcol = zobj_toc<TColumns>(tcol_obj);

		// TODO: name is own alias? - because alias used first.
		zval_mgr colnames(SQSTR.asterisk);
		tcol->construct(tname, tname, colnames); 

		zstr_user   jtype = data.get(SQSTR.jointype);
		int jenum = JoinInfo::getJoinType(jtype);

		zobj_mgr jiobj = JoinInfo::omg.new_zobj();
		JoinInfo* ji = zobj_toc<JoinInfo>(jiobj);

		zval_mgr temp(tcol_obj);
		ji->construct(temp, prime, jenum);

		joins->addJoin(jiobj);

		htab_read nested = data.get(SQSTR.nested);
		if (nested.size())
		{
			htab_walk wk;
			zval_mgr lhs;
			zval_mgr rhs;

			auto jc = wk.value();
			for(wk.start(nested); wk.ok(); wk.next())
			{
				// must be array
				htab_read jah = jc.zarray();

				if (!jah.size())
				{
					throw std::runtime_error("Nested join has no data");
				}

				zstr_mgr c1str = jah.get(SQSTR.condition1);
				zval_mgr lhs = TableAttr::splitDot(c1str);

				zstr_mgr c2str = jah.get(SQSTR.condition2);
				zval_mgr rhs = TableAttr::splitDot(c2str);

				zstr_mgr opstr = jah.get(SQSTR.operator_key);
				int op = JoinExpr::toOperator(opstr);

				zstr_mgr bstr = jah.get(SQSTR.boolean);
				int logic = JoinExpr::toLogic(bstr);	

				ji->add(lhs, rhs, op, logic);			
			}
			return true;
		}
	}
	return false;
}

zval_user 
Bindings::get(int key)
{
	return data_.get(key);
}


bool 
Bindings::aliasSelect()
{
	JoinTables* jt = getJoinTables();
	htab_read tables = jt->getData();
	if (tables.size())
	{
		htab_mgr   cols_mgr;

		htab_write all_cols(cols_mgr);

		htab_read all = jt->getTables();
		htab_walk wk;

		auto tcol = wk.value();
		for(wk.start(all); wk.ok(); wk.next())
		{

			htab_mgr aliased = this->columnAlias(tcol);
			all_cols.merge(aliased);
		}
		if (all_cols.size())
		{
			zval_mgr temp(all_cols);
			this->set(ISql::SQL_RENAME, temp);
			return true;
		}
	}
	return false;
}

zstr_mgr 
concat2(zstr_user s1, zstr_user s2)
{
	zstr_buffer nbuf;
	nbuf << s1 << s2;
	return nbuf.zstr();
}

htab_mgr 
Bindings::columnAlias(zobj_user tcolobj)
{
	zstr_buffer buf;
	TColumns* tcol = zobj_toc<TColumns>(tcolobj);

	buf << '_' << tcol->getAlias() << '_';
	zstr_mgr sfx = buf.zstr();

	htab_mgr aliased_mgr;
	htab_write aliased(aliased_mgr);

	if (tcol->has(SQSTR.asterisk))
	{
		tcol->unsetCol(SQSTR.asterisk);

		zval_mgr tname(tcol->getName());

		zobj_mgr model = db_.call(SQSTR.getTableModel, tname);

		htab_read columns = model.call(SQSTR.getColDefs);

		htab_walk wk;
		auto cname = wk.key();
		for(wk.start(columns); wk.ok(); wk.next())
		{
			zstr_user colname(cname.zstr());
			zstr_mgr rename = concat2(colname, sfx);
			aliased.set(rename, colname);
			tcol->setColAlias(colname, rename);
		}
	}
	else {
		htab_read columns = tcol->getColNames();
		htab_walk wk;
		auto cname = wk.key();	
		auto alias = wk.value();
		for(wk.start(columns); wk.ok(); wk.next())
		{
			if (!alias.isString())
			{
				zstr_user colname(cname.zstr());
				zstr_mgr rename = concat2(colname, sfx);
				aliased.set(rename, colname);
				tcol->setColAlias(colname, rename);
			}
		}
	}
	return aliased_mgr;
}


void Bindings::set(int key, zval_user value)
{
	htab_write(data_).set(key, value);
}

void Bindings::unset(int key)
{
	htab_write(data_).unset(key);
}

void Bindings::wipe(int key)
{
	htab_write hw(data_);

	if (key == 0)
	{
		hw.clear();
	}
	else {
		hw.unset(key);
	}
}

zstr_mgr alias_str_key(zstr_user malias)
{
	zstr_buffer buf;

	buf << '_' << malias << '_';

	return buf.zstr();
}

void 
Bindings::where(zval_user column, zstr_user opstr, zval_user value, zstr_user blogic)
{

}

void
Bindings::whereKeyValue(zval_user key, zval_user value)
{
	if (key.isString() && !value.isArray())
	{
		where(key, SQSTR.cmp_equal, value, SQSTR.and_str);
	}
	if (key.isArray() && value.isArray())
	{
		htab_read keys(key.zarray());
		htab_read values(value.zarray());

		if (keys.size() == values.size())
		{
			htab_walk wk;
			auto k = wk.key();
			auto v = wk.value();

			zval_user test = values.get(int(0));
			if (test.isNull())
			{
				for(wk.start(values); wk.ok(); wk.next())
				{
					where(k, SQSTR.cmp_equal, v, SQSTR.and_str);
				}
			}
			else {
				int ix = 0;
				for(wk.start(keys); wk.ok(); wk.next())
				{
					ix = k.zlong();
					test = values.get(ix);
					where(v, SQSTR.cmp_equal, test, SQSTR.and_str);
				}
			}
		}
	}
	zend_throw_error(zend_ce_error,"whereKeyValue parameters do not match");
}

zval_mgr 
Bindings::select(zobj_user prop)
{
	zobj_mgr from = getJoins();

	zval_mgr columns_mgr = prop.property(SQSTR.columns);
	zval_user columns(columns_mgr);

	if (columns.ok())
	{
		JoinTables* jt = zobj_toc<JoinTables>(from);
		zobj_mgr prime = jt->getPrime();
		IColumns* pc = zobj_toc<IColumns>(prime);
		pc->clear();
		pc->add(columns);
	}

	if (from.ok())
	{
		aliasSelect();
	}

	ISql* sql = zobj_toc<ISql>(sql_);

	zobj_mgr plist = sql->select(*this);

	zval_mgr  row_fetch = prop.property(SQSTR.fetch_key);
	zval_mgr  old_fetch = db_.call(SQSTR.setfetch, row_fetch);

	ParamList* pobj = zobj_toc<ParamList>(plist);

	zval_mgr rows = RunSql::op(db_, pobj->getSql(), pobj->getValues(), true);


	if (zval_user(old_fetch).zlong() != zval_user(row_fetch).zlong())
	{
		db_.call(SQSTR.setfetch, old_fetch);
	}

	zstr_user mclass;

	zval_mgr model_mgr = prop.property(SQSTR.model);
	zobj_user model(model_mgr);

	if (!model.ok())
	{
		model_mgr = prop.property(SQSTR.modelclass);
		if (zval_user(model_mgr).isNull())
		{
			return rows;
		}
	}

	htab_read hr(zval_user(rows).zarray());
	if (hr.ok())
	{
		size_t rct = hr.size();
		if (rct == 1)
		{
			Model* m = zobj_toc<Model>(model);
			return (zend_object*) m->newRow(hr.get((int)0), true);
		}
		else if (rct == 0)
		{
			//empty array
			model_mgr.set_null();
			return model_mgr;
		}
	}
	//TODO: else what?
	// multiple rows case
	mclass = model.className();

	
	zval_mgr  eager_load_mgr = prop.property(SQSTR.eager_load);
	htab_read eager_load(eager_load_mgr);

	zval_user rename = get(ISql::SQL_RENAME);

	if (rename.isArray())
	{
		htab_mgr objset_mgr;
		htab_write objset(objset_mgr);

		JoinTables* fromjt = getJoinTables();

		zstr_mgr table_name = fromjt->getModel();

		htab_read tables = fromjt->getTables();

		zobj_mgr icols = fromjt->getTable(table_name);

		table_name = zobj_toc<IColumns>(icols)->getAlias();

		zstr_mgr mb_id = alias_str_key(table_name);

		htab_mgr alias_list_mgr;
		htab_write alias_list(alias_list_mgr);

		htab_walk w1;

		auto alias_key = w1.key();
		for(w1.start(tables); w1.ok(); w1.next())
		{
			if (zs_cmp(table_name, alias_key.zstr()) != 0)
			{
				zstr_mgr a_key = alias_str_key(alias_key);
				alias_list.push_back(a_key);
			}
		}

		auto r_row = w1.value();
		htab_read relist(rename.zarray());

		for(w1.start(hr); w1.ok(); w1.next())
		{
			zobj_mgr obj = JoinTables::rowSplit(r_row, relist);
			zval_mgr recset = obj.property(mb_id);
			htab_walk w2;
			auto ai_value = w2.value();
			// TODO: check recset ??
			htab_write rec(recset);

			for(w2.start(alias_list); w2.ok(); w2.next())
			{
				zstr_mgr key = ai_value.zstr();
				rec.set(key, obj.property(key));
			}
			objset.push_back(rec);
		}
		unset(ISql::SQL_RENAME);
		return Model::createFromResult(mclass, objset_mgr);
	}
	return Model::createFromResult(mclass, hr);
}


}; // namespace wcc

#ifndef SQL_ARGINFO_H
#define SQL_ARGINFO_H
extern "C" {
	#include "sqlipart_arginfo.h"
};
#endif

//__construct(IColumns $LTable, ?IColumns $RTable, int $jtype = JoinInfo::J_INNER);
ZEND_METHOD(Wcd_Sql_JoinInfo, __construct)
{
	zval* left_tab;
	zval* right_tab;
	zend_long jtype = JoinInfo::J_INNER;

	ZEND_PARSE_PARAMETERS_START(2,3)
	Z_PARAM_OBJECT_OF_CLASS(left_tab, zclass_sql_icolumns)
	Z_PARAM_OBJECT_OF_CLASS_OR_NULL(right_tab, zclass_sql_icolumns)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(jtype)
	ZEND_PARSE_PARAMETERS_END();

	JoinInfo* cobj = zval_toc<JoinInfo>(ZEND_THIS);
	cobj->construct(left_tab, right_tab, jtype);
}

/*  public function add(
            mixed $lattr,   
            mixed $rattr = null, 
            int $op = JoinExpr::OP_EQ, 
            int $logic =  JoinExpr::B_NULL) : void {} */

ZEND_METHOD(Wcd_Sql_JoinInfo, add)
{
	zval* left_attr;
	zval* right_attr = nullptr;
	zend_long op = JoinExpr::OP_EQ;
	zend_long logic = JoinExpr::B_NULL;

	ZEND_PARSE_PARAMETERS_START(1,4)
	Z_PARAM_ZVAL(left_attr)
	Z_PARAM_OPTIONAL
	Z_PARAM_ZVAL(right_attr)
	Z_PARAM_LONG(op)
	Z_PARAM_LONG(logic)
	ZEND_PARSE_PARAMETERS_END();

	JoinInfo* cobj = zval_toc<JoinInfo>(ZEND_THIS);
	cobj->add(left_attr, right_attr, op, logic);
}

/* public function addExpr(JoinExpr $expr) : void {} */
ZEND_METHOD(Wcd_Sql_JoinInfo, addExpr)
{
	zval* expr;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(expr, zclass_join_expr)
	ZEND_PARSE_PARAMETERS_END();

	JoinInfo* cobj = zval_toc<JoinInfo>(ZEND_THIS);
	cobj->addExpr(expr);
}

/* public function getConditions() : array {} */
ZEND_METHOD(Wcd_Sql_JoinInfo, getConditions)
{
	ZEND_PARSE_PARAMETERS_NONE();
	JoinInfo* cobj = zval_toc<JoinInfo>(ZEND_THIS);

	htab_read htab = cobj->getConditions();
	htab.return_zv(return_value);
}

/* static public function toJoinType(string $join) : int {} */
ZEND_METHOD(Wcd_Sql_JoinInfo, toJoinType)
{
	zend_string* join;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(join)
	ZEND_PARSE_PARAMETERS_END();

	RETURN_LONG(JoinInfo::getJoinType(join));

}

/* public function addJoin(JoinInfo $ji) : JoinInfo {} */
ZEND_METHOD(Wcd_Sql_JoinTables, addJoin)
{
	zval* jinfo;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(jinfo, zclass_join_info);
	ZEND_PARSE_PARAMETERS_END();
	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);

	zobj_mgr obj = cobj->addJoin(jinfo);

	obj.move_zv(return_value);
}

/* public function addResult(TableAttr $attr) : void {} */
ZEND_METHOD(Wcd_Sql_JoinTables, addResult)
{
	zval* tattr;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(tattr, zclass_table_attr);
	ZEND_PARSE_PARAMETERS_END();
	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);

	cobj->addResult(tattr);
}

/* public function addTable(IColumns $tc) : void {} */
ZEND_METHOD(Wcd_Sql_JoinTables, addTable)
{
	zval* tc;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(tc, zclass_sql_icolumns);
	ZEND_PARSE_PARAMETERS_END();

	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	cobj->addTable(tc);
}

/* public function addWhere(mixed $lattr, mixed $rattr=null, 
        int $op=JoinExpr::OP_EQ, 
        int $logic=JoinExpr::B_NULL) : void {} */

ZEND_METHOD(Wcd_Sql_JoinTables, addWhere)
{
	zval* left_attr;
	zval* right_attr = nullptr;
	zend_long op = JoinExpr::OP_EQ;
	zend_long logic = JoinExpr::B_NULL;

	ZEND_PARSE_PARAMETERS_START(1,4)
	Z_PARAM_OBJECT(left_attr)
	Z_PARAM_OPTIONAL
	Z_PARAM_OBJECT(right_attr)
	Z_PARAM_LONG(op)
	Z_PARAM_LONG(logic)
	ZEND_PARSE_PARAMETERS_END();

	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	cobj->addWhere(left_attr, right_attr, op, logic);
}

/* public function getData() : array {} */
ZEND_METHOD(Wcd_Sql_JoinTables, getData)
{
	ZEND_PARSE_PARAMETERS_NONE();
	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	const htab_mgr& htab = cobj->getData();
	htab.return_zv(return_value);
}

/* public function getModel() : ?string {} */
ZEND_METHOD(Wcd_Sql_JoinTables, getModel)
{
	ZEND_PARSE_PARAMETERS_NONE();
	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	zstr_user name = cobj->getModel();
	name.return_zv(return_value);
}

/* public function getOrder() : array {} */
ZEND_METHOD(Wcd_Sql_JoinTables, getOrder)
{
	ZEND_PARSE_PARAMETERS_NONE();
	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	const htab_mgr& htab = cobj->getOrder();
	htab.return_zv(return_value);
}

/*  public function getPivot(): ?IColumns {} */
ZEND_METHOD(Wcd_Sql_JoinTables, getPivot)
{
	ZEND_PARSE_PARAMETERS_NONE();
	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
    zobj_mgr obj = cobj->getPivot();
	obj.move_zv(return_value);
}

/* public function getPrime(): ?IColumns  {} */
ZEND_METHOD(Wcd_Sql_JoinTables, getPrime)
{
	ZEND_PARSE_PARAMETERS_NONE();
	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	zobj_mgr obj = cobj->getPrime();
	obj.move_zv(return_value);
}

/* public function getTable(string $name) : ?IColumns {} */
ZEND_METHOD(Wcd_Sql_JoinTables, getTable)
{
	zend_string* name;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	zobj_mgr obj = cobj->getTable(name);
	obj.move_zv(return_value);
}

/* public function getTableAlias(string $name) : ?IColumns {} */
ZEND_METHOD(Wcd_Sql_JoinTables, getTableAlias)
{
	zend_string* name;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	zobj_mgr obj = cobj->getTableAlias(name);
	obj.move_zv(return_value);
}

/* public function getTables(): array {} */
ZEND_METHOD(Wcd_Sql_JoinTables, getTables)
{
	ZEND_PARSE_PARAMETERS_NONE();
	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	const htab_mgr& htab = cobj->getTables();
	htab.return_zv(return_value);
}

/* public function order(string $name, bool $ascend = true) : void {} */
ZEND_METHOD(Wcd_Sql_JoinTables, order)
{
	zend_string* name;
	bool         ascend = true;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_STR(name)
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(ascend)
	ZEND_PARSE_PARAMETERS_END();

	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	cobj->order(name, ascend);
}

/* public static function rowSplit(array $row, array $rename) : object {} */
ZEND_METHOD(Wcd_Sql_JoinTables, rowSplit)
{
	zval* row;
	zval* rename;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_ARRAY(row)
	Z_PARAM_ARRAY(rename)
	ZEND_PARSE_PARAMETERS_END();

	zobj_mgr obj = JoinTables::rowSplit(row, rename);
	obj.move_zv(return_value);
}

/* public function setModel(string $name) : void {} */
ZEND_METHOD(Wcd_Sql_JoinTables, setModel)
{
	zend_string* name;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	cobj->setModel(name);

}

/* public function setPrime(IColumns $tc) : void {} */
ZEND_METHOD(Wcd_Sql_JoinTables, setPrime)
{
	zval* tcol;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(tcol, zclass_sql_icolumns)
	ZEND_PARSE_PARAMETERS_END();


	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	cobj->setPrime(zval_user(tcol).zobject());
}

/* public function delete(Bindings $bind) : ParamList {} */
ZEND_METHOD(Wcd_Sql_ISql, delete)
{
	zval* bind;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(bind, zclass_bindings)
	ZEND_PARSE_PARAMETERS_END();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);
	Bindings& refbind = *zval_toc<Bindings>(bind);

	zobj_mgr plist = cobj->deleteSql(refbind);
	plist.move_zv(return_value);
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

	zstr_mgr result = cobj->emit(part, pbind, lalias, ralias);
	result.move_zv(return_value);

}

/* public static function tableClass(string $uname) : string {} */
ZEND_METHOD(Wcd_Sql_ISql, tableClass)
{
	zend_string* table;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(table)
	ZEND_PARSE_PARAMETERS_END();

	zstr_mgr result = ISql::tableClass(table);
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
	zstr_mgr result = cobj->entityClass(table);
	result.move_zv(return_value);
}

/* public function getTruncateSql(): string {} */
ZEND_METHOD(Wcd_Sql_ISql, getTruncateSql)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);
	zstr_mgr result = cobj->getTruncateSql();
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

	zobj_mgr plist = cobj->insert(*refbind);
	plist.move_zv(return_value);
}

/* public function quoteName(string $name): string {} */
ZEND_METHOD(Wcd_Sql_ISql, quoteName)
{
	zend_string* name;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);

	zstr_mgr qname = cobj->quoteName(name);
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

	zobj_mgr plist = cobj->select(refbind);
	plist.move_zv(return_value);
}

/* public function seqLastValue(string $seq_name): string {} */
ZEND_METHOD(Wcd_Sql_ISql, seqLastValue)
{
	zend_string* name;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);

	zstr_mgr qname = cobj->seqLastValue(name);
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

	zstr_mgr sql = cobj->setSeqValue(value, data);
	sql.move_zv(return_value);
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

	zstr_mgr sql = cobj->truncate(refbind);
	sql.move_zv(return_value);
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

	zobj_mgr plist = cobj->update(refbind);
	plist.move_zv(return_value);
}

/* public function valuesDefault(): string {} */
ZEND_METHOD(Wcd_Sql_ISql, valuesDefault)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);
	zstr_mgr result = cobj->valuesDefault();
	result.move_zv(return_value);
}

/* public function __construct(\Wcd\IDriver $gen); */
ZEND_METHOD(Wcd_Sql_ParamList, __construct)
{
	zval* driver;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT(driver)
	ZEND_PARSE_PARAMETERS_END();
	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
	cobj->construct(driver);
}

/* public function addParam(mixed $value) : void {} */
ZEND_METHOD(Wcd_Sql_ParamList, addParam)
{
	zval* value;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();
	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
	cobj->addParam(value);
}

/* public function addParamList(array $values): string {} */
ZEND_METHOD(Wcd_Sql_ParamList, addParamList)
{
	zval* value;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(value)
	ZEND_PARSE_PARAMETERS_END();
	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
	zstr_mgr result = cobj->addParamList(value);
	result.move_zv(return_value);
}

/* public function getParams() : array {} */
ZEND_METHOD(Wcd_Sql_ParamList, getParams)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
	const htab_mgr& htab = cobj->getParams();
	htab.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_ParamList, getReturns)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
	const htab_mgr& htab = cobj->getReturns();
	htab.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_ParamList, getSql)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
	zstr_mgr sql = cobj->getSql();
	sql.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_ParamList, getValues)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
	const htab_mgr& htab = cobj->getValues();
	htab.return_zv(return_value);
}

/*public function makeList(int $start, int $count): string {}*/
ZEND_METHOD(Wcd_Sql_ParamList, makeList)
{
	zend_long start;
	zend_long count;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_LONG(start)
	Z_PARAM_LONG(count)
	ZEND_PARSE_PARAMETERS_END();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);

	zstr_mgr result = cobj->makeList(start, count);
	result.move_zv(return_value);
}

/* public function paramLiteral(mixed $value) : string {} */
ZEND_METHOD(Wcd_Sql_ParamList, paramLiteral)
{
	zval* value;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);

	zstr_mgr result = cobj->paramLiteral(value);
	result.move_zv(return_value);
}

/* public function setParams(array $replace) : void {} */
ZEND_METHOD(Wcd_Sql_ParamList, setParams)
{
	zval* value;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(value)
	ZEND_PARSE_PARAMETERS_END();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);

	cobj->setParams(value);
}

/* public function setReturns(array $replace) : void {}*/
ZEND_METHOD(Wcd_Sql_ParamList, setReturns)
{
	zval* value;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(value)
	ZEND_PARSE_PARAMETERS_END();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);

	cobj->setReturns(value);
}

ZEND_METHOD(Wcd_Sql_ParamList, setSql)
{
	zend_string* sql;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(sql)
	ZEND_PARSE_PARAMETERS_END();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);

	cobj->setSql(sql);
}

ZEND_METHOD(Wcd_Sql_ParamList, setValues)
{
	zval* values;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(values)
	ZEND_PARSE_PARAMETERS_END();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);

	cobj->setValues(values);
}

/* public function useOwnValues() : void {} */
ZEND_METHOD(Wcd_Sql_ParamList, useOwnValues)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
	cobj->useOwnValues();
}

/* public function wipe() : void {} */
ZEND_METHOD(Wcd_Sql_ParamList, wipe)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
	cobj->wipe();
}

/* public function __construct(ISql $gen, IConnect $connect); */
ZEND_METHOD(Wcd_Sql_Bindings, __construct)
{
	zval* gen;
	zval* connect;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_OBJECT(gen)
	Z_PARAM_OBJECT(connect)
	ZEND_PARSE_PARAMETERS_END();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	cobj->construct(gen,connect);
}

/* public function add(int $key, mixed $value) : void {} */
ZEND_METHOD(Wcd_Sql_Bindings, add)
{
	zend_long key;
	zval*     value;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_LONG(key)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	cobj->add(key,value);
}

/* public function addJoinData(array $data) : void {} */
ZEND_METHOD(Wcd_Sql_Bindings, addJoinData)
{
	zval*     data;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	RETURN_BOOL(cobj->addJoinData(data));
}

/* public function aliasSelect(): bool {} */
ZEND_METHOD(Wcd_Sql_Bindings, aliasSelect)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	RETURN_BOOL(cobj->aliasSelect());
}

/* public function get(int $key) : mixed {} */
ZEND_METHOD(Wcd_Sql_Bindings, get)
{
	zend_long key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(key)
	ZEND_PARSE_PARAMETERS_END();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	zval_user ret = cobj->get(key);
	ret.return_zv(return_value);
}

/* public function getArray(int $key) : array|null {} */
ZEND_METHOD(Wcd_Sql_Bindings, getArray)
{
	zend_long key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(key)
	ZEND_PARSE_PARAMETERS_END();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	htab_mgr result;
	if (cobj->getArray(key, result))
	{
		result.move_zv(return_value);
	}
	else {
		RETVAL_NULL();
	}
}

ZEND_METHOD(Wcd_Sql_Bindings, getData)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	const htab_mgr& data = cobj->getData();
	data.return_zv(return_value);
}

/* public function getJoins(): JoinTables {} */
ZEND_METHOD(Wcd_Sql_Bindings, getJoins)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	zobj_mgr result = cobj->getJoins();

	result.move_zv(return_value);
}

/* public function getParamList() : ParamList {} */
ZEND_METHOD(Wcd_Sql_Bindings, getParamList)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	const zobj_mgr& result = cobj->getParamList();
	result.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Bindings, iSql)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	const zobj_mgr& result = cobj->isql();

	result.return_zv(return_value);
}

/* public function primeJoin(TColumns $tc): JoinTables {} */
ZEND_METHOD(Wcd_Sql_Bindings, primeJoin)
{
	zval* tcol;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(tcol, zclass_sql_tcolumns)
	ZEND_PARSE_PARAMETERS_END();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	zobj_mgr jt = cobj->primeJoin(tcol);
	jt.move_zv(return_value);
}

/* public function set(int $key, mixed $value) : void {} */
ZEND_METHOD(Wcd_Sql_Bindings, set)
{
	zend_long key;
	zval*     value;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_LONG(key)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();
	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	cobj->set(key, value);

}

/* public function setParamList(ParamList $params) : void {} */
ZEND_METHOD(Wcd_Sql_Bindings, setParamList)
{
	zval* plist;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(plist, zclass_param_list)
	ZEND_PARSE_PARAMETERS_END();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	cobj->setParamList(plist);
}

/* public function unset(int $key) : void {} */
ZEND_METHOD(Wcd_Sql_Bindings, unset)
{
	zend_long key;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(key)
	ZEND_PARSE_PARAMETERS_END();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	cobj->unset(key);
}

/* public function wipe(int $ix = 0) : void {}*/
ZEND_METHOD(Wcd_Sql_Bindings, wipe)
{
	zend_long key = 0;
	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(key)
	ZEND_PARSE_PARAMETERS_END();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	cobj->wipe(key);
}

ZEND_METHOD(Wcd_Sql_Bindings, select)
{
	zval* obj;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT(obj)
	ZEND_PARSE_PARAMETERS_END();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	zval_mgr result = cobj->select(obj);

	result.move_zv(return_value);
}


#ifndef DB_ARGINFO_H
#define DB_ARGINFO_H
extern "C" {
     #include "stub/db_arginfo.h"
}
#endif

bool init_isql_module()
{
	// need the IfSql interface

	zintf_ce_Sql_IfSql = register_class_Wcd_Sql_IfSql();

	zclass_isql = register_class_Wcd_Sql_ISql(zintf_ce_Sql_IfSql);
	ISql::omg.classEntry(zclass_isql);

	zclass_bindings = register_class_Wcd_Sql_Bindings();
	Bindings::omg.classEntry(zclass_bindings);

	zclass_param_list = register_class_Wcd_Sql_ParamList();
	ParamList::omg.classEntry(zclass_param_list);

	zclass_join_tables = register_class_Wcd_Sql_JoinTables();
	JoinTables::omg.classEntry(zclass_join_tables);

	zclass_join_info = register_class_Wcd_Sql_JoinInfo();
	JoinInfo::omg.classEntry(zclass_join_info);

	return true;

}
//sql_isql.cpp
#endif