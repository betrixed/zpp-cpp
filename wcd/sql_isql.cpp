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

#ifndef WCD_OPERATION_H
#include "operation.h"
#endif

#ifndef SQL_ARGINFO_H
#define SQL_ARGINFO_H
extern "C" {
	#include "stub/sqlipart_arginfo.h"
};
#endif

#ifndef BINDINGS_ARGINFO_H
#define BINDINGS_ARGINFO_H
extern "C" {
	#include "stub/bindings_arginfo.h"
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

void ParamList::debug_info(htab_rw di)
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
ParamList::construct(obj_ptr driver)
{
	driver_ = driver;
}

str_rc 
ParamList::paramStr(int ct)
{
	return driver_.call(SQSTR.param, val_rc(ct));
}

str_rc 
ParamList::addParam(val_ptr value)
{
	htab_rw hw(params_);
	hw.push_back(value);

	return paramStr((int) params_.size());
}


void 
ParamList::useOwnValues()
{
	val_params_ = params_;
}

str_rc 
ParamList::paramLiteral(val_ptr value)
{
	val_ptr temp;

	if (value.isObject())
	{
		obj_ptr obj(value.zobject());

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
				return val_ptr(temp).to_zstr();
			}
		}
	}
	else {
		return this->addParam(value);
	}
	throw std::runtime_error("Unhandled paramLiteral argument");
}

str_rc 
ParamList::addParamList(htab_rd values)
{
	str_buf buf;

	int ix = (int) params_.size();

	htab_walk wk;
	auto item = wk.value();

	int bufct = 0;

	htab_rw pw(params_);

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

str_rc 
ParamList::makeList(int start, int count)
{
	str_buf buf;
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
JoinInfo::construct(obj_ptr ltable, obj_ptr rtable, int jtype)
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
JoinInfo::add(val_ptr lexp, val_ptr rexp, int jtype, int logic)
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
JoinInfo::addExpr(obj_ptr jexpr)
{
	htab_rw(joinExpr_).push_back(jexpr);
}

str_ptr// static
JoinInfo::joinStr(int jid)
{
	return SQSTR.joinstr[jid];
}

str_ptr  
JoinInfo::joinTypeStr() const
{
	return SQSTR.joinstr[joinType_];
}

int //static
JoinInfo::getJoinType(str_ptr s)
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
JoinTables::debug_info(htab_rw di)
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
JoinTables::setPrime(obj_ptr obj)
{
	this->addTable(obj);
	prime_ = obj;
}

obj_rc 
JoinTables::addJoin(obj_ptr jiobj)
{
	obj_rc result(jiobj);

	htab_rw(joins_).push_back(jiobj);

	JoinInfo* ji = zobj_toc<JoinInfo>(jiobj);

	this->addTable(ji->leftTable_);

	return result;
}

void 
JoinTables::addTable(obj_ptr icol)
{
	IColumns* tc = zobj_toc<IColumns>(icol);
	str_ptr name = tc->getAlias();
	if (name.isNull())
	{	
		name = tc->getName();
	}
	if (name.isNull())
	{
		zend_throw_error(zend_ce_error, "addTable with no Alias or Name");
		return;
	}
	htab_rw(byAlias_).set(name, icol);
}

void 
JoinTables::addResult(obj_ptr ta)
{
	htab_rw(results_).push_back(ta);
}

void 
JoinTables::addWhere(val_ptr leftAttr, val_ptr rightAttr, int op, int logic)
{
	obj_rc jobj = JoinExpr::omg.new_zobj();

	JoinExpr* je = zobj_toc<JoinExpr>(jobj);

	je->construct(leftAttr, rightAttr, op, logic);

	htab_rw(where_).push_back(jobj);
}

obj_rc
JoinTables::getPivot()
{
	obj_rc result;

	if (joins_.size())
	{
		obj_ptr first = joins_.get(int(0));

		JoinInfo* ji = zobj_toc<JoinInfo>(first);
		result = ji->leftTable_;
	}
	return result;
}

obj_rc
JoinTables::getTable(str_ptr name)
{
	obj_rc result;

	if (byAlias_.size())
	{
		htab_walk wk;

		auto tcobj = wk.value();

		for(wk.start(byAlias_); wk.next(); wk.ok())
		{
			if (tcobj.isObject())
			{
				obj_ptr test(tcobj.zobject());
				IColumns* ic = zobj_toc<IColumns>(test);
				str_ptr tcname = ic->getName();
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

obj_rc
JoinTables::getTableAlias(str_ptr name)
{

	obj_rc result = byAlias_.get(name);
	return result;
}

void 
JoinTables::order(str_ptr name, bool ascend)
{
	htab_rc pair;
	val_rc bval;

	bval.set_bool(ascend);
	htab_rw hw(pair);

	hw.set(SQSTR.column, name);
	hw.set(SQSTR.ascend, bval);

	htab_rw(orderby_).push_back(pair);
}

obj_rc //static
JoinTables::rowSplit(htab_rd row, htab_rd rename)
{
	htab_rc rec_temp;
	htab_rw rec(rec_temp);

	htab_rc ok_temp;
	htab_rw ok(ok_temp);

	obj_rc obj = class_data::std_object();

	{
		htab_walk wk;
		auto name = wk.key();
		auto val = wk.value();
		for(wk.start(row); wk.ok(); wk.next())
		{
			val_ptr orig;

			str_rc namekey(name.zstr());

			if (rename.try_fetch(namekey, orig))
			{
				str_rc tail = namekey.substr(-3);
				htab_rc tails;


				htab_rw(tails).set(orig.zstr(), val);
				rec.set(tail, tails);
			}
			else {
				ok.set(namekey, val);
			}
		}
	}
	if (ok.size())
	{
		val_rc temp(ok);
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
ISql::columns(htab_rd bd)
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
		htab_rd colNames(tc->getColNames());
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

			cfrag = std::move(fbuf);
			//showstr("cfrag:", cfrag);
			col_list.push_back(cfrag);
		}
	}
	{
		htab_rd expr = tc->getExpr();
		htab_walk wk;
		auto exalias = wk.key();
		auto exfn = wk.value();

		for(wk.start(expr); wk.ok(); wk.next())
		{
			str_buf fbuf;
			fbuf << exfn.zstr() << " as " << this->quoteName(exalias.zstr());
			cfrag = fbuf.zstr();
			//showstr("expr:", cfrag);
			col_list.push_back(cfrag);
		}
	}
}

// 
str_rc
ISql::orderBy(htab_rd obind)
{
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
			htab_rd order(order_tab.zarray());
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
			val_ptr descend = order.get(SQSTR.descend);
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

obj_rc 
ISql::deleteSql(Bindings& bind)
{
	str_buf buf;

	buf << "DELETE FROM";

	htab_rd tables = this->getTables(bind);

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
		htab_rd wbind( wh.zarray());

		if (wbind.size())
		{
			str_rc whstr(this->where(bind, wbind));
			//showstr("WHERE ", whstr);
			buf << " WHERE" << whstr;
		}
	}

	val_ptr order( bind.get(SQL_ORDER));

	if (order.isArray())
	{
		buf << " ORDER BY" << this->orderBy(order);
	}

	obj_rc paramList = bind.getParamList();
	ParamList* plist = zobj_toc<ParamList> (paramList);
	//htab_rd params = plist->getParams();

	str_rc sql = buf.zstr();
	plist->setSql(sql);
	plist->useOwnValues();

	return paramList;
}

SqlPartId* getPartObj(val_ptr ret)
{
	if (ret.isObject())
	{
		obj_ptr test(ret.zobject());
		if (test.instanceof(zclass_sql_ifipart))
		{
			return zobj_toc<SqlPartId>(test);
		}
	}
	throw std::runtime_error("SqlPartId object expected");
}

//* called from a JoinExpr
str_rc
ISql::emit(val_ptr sp, Bindings* bind, str_ptr lalias, str_ptr ralias)
{
 	SqlPartId* part = getPartObj(sp);
 	int partid = part->getPartId();

 	str_buf buf;
 	str_rc str;

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
 			buf << " (" << je->emit(0, bind, lalias, ralias) << " )";
 		}
 		break;
 	case SqlPartId::PARAM_PID:
 		{
 			Param* p = static_cast<Param*>(part);
 			val_ptr pvalue = p->getValue();
			obj_rc paramList = bind->getParamList();
			ParamList* list = zobj_toc<ParamList> (paramList);
 			buf << list->addParam(pvalue);
 		}
 		break;
 	default:
 		throw std::runtime_error("Unmatched partid in emit");
 	}
 	
 	return buf.zstr();		
}

str_rc 
ISql::getTruncateSql()
{
	return SQSTR.delete_from;
}

htab_rd
ISql::getTables(Bindings& bind)
{
	JoinTables* jt = bind.getJoinTables();
	return jt->getTables();
}

str_rc 
ISql::truncate(Bindings& bind)
{
	str_buf buf;

	buf << this->getTruncateSql();

	JoinTables* jt = bind.getJoinTables();

	if (jt) {
		obj_rc pobj = jt->getPrime();
		if (pobj.ok())
		{
			IColumns* icol = zobj_toc<IColumns>(pobj);
			buf << ' ' << this->quoteName(icol->getName());
			return buf.zstr();
		}
	}
	throw std::runtime_error("Truncate table not set");
}

str_rc
ISql::insert_col_params(Bindings& bind, htab_rd rowbind)
{
	htab_walk wk;

	str_buf ptext;
	str_buf ctext;

	str_rc place;

	obj_rc paramList = bind.getParamList();

	ParamList* params = zobj_toc<ParamList> (paramList);

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
			htab_rd vdata(val.zarray());
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

	return ctext.zstr();
}

str_rc
ISql::setSeqValue(int value, htab_rd data)
{
	str_rc seq_name = data.get(SQSTR.seq_key);

	if (!seq_name.isNull())
	{
		str_buf buf;

		buf << "select setval('" << seq_name << "'," << value << ")";
		return buf.zstr();
	}
	throw std::runtime_error("Missing name for setSeqValue");
}

str_rc
ISql::seqLastValue(str_ptr seq)
{
	str_buf buf;

	buf << "select LASTVAL(" << seq << ")";

	return buf.zstr();
}

IColumns* getIColumns(val_ptr zv)
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

void extract_params(htab_rd rowbind, htab_rd plist, htab_rw params);

void extract_params(htab_rd rowbind, htab_rd plist, htab_rw params)
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

obj_rc 
ISql::insert(Bindings& bind)
{
	obj_rc result;

	str_buf buf;

	buf << "INSERT INTO";

	htab_rd jtables = this->getTables(bind);

	if (!jtables.size()) {
		zend_throw_error(zend_ce_error, "No tables set");
		return result;
	}

	htab_walk pos;
	pos.start(jtables);
	
	IColumns* icol = getIColumns(pos.value());

	buf << ' ' << this->quoteName(icol->getName());

	htab_rc sql_insert;
	// C++ pass by reference, return if set
	if (!bind.getArray(ISql::SQL_INSERT, sql_insert))
	{
		zend_throw_error(zend_ce_error, "Insert table not set");
		result = buf.zstr();
		return result;
	}
	//zend_printf("insert: %s ", buf.data());
	//showdata("insert bind", sql_insert);

	htab_walk  insert_wk;
	htab_rd  rowbind;


	int pcount = 0;
	if (insert_wk.start(sql_insert))
	{
		rowbind = insert_wk.value().zarray();
		
		size_t rsize = rowbind.size();

		if (rsize)
		{
			pcount += rsize;
			buf << this->insert_col_params(bind, rowbind);
			//zend_printf("insert 2: %s\n ", buf.data());
		}
	}
	if (pcount == 0)
    {
    	//zend_printf("\nValues Default\n");
    	obj_rc self(this->vobj());
    	val_rc dtext = self.call(SQSTR.valuesdefault);
		buf << ' ' << val_ptr(dtext).zstr() << ' ';
		//zend_printf("insert 3: %s\n ", buf.data());
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
	
	obj_rc   pobj = bind.getParamList();

	//showobj("pobj", pobj);

	ParamList* plist = zobj_toc<ParamList>(pobj);

	htab_rd params = plist->getParams();

	htab_rc   ret_params_mgr;
	htab_rw ret_params(ret_params_mgr);

	if (params.size())
	{
		extract_params(rowbind, params, ret_params);
		//ret_params.show_data("ret_params");

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
	//showstr("sql", sql);

	plist->setSql(sql);

	if (ret_params.size())
	{
		//showarray("ret_params_mgr", ret_params_mgr);
		plist->setValues(ret_params_mgr);
	}
	if (rettab.size())
	{
		//showarray("rettab", rettab);
		plist->setReturns(rettab);
	}
	//showobj("return pobj", pobj);
	return pobj;
}

str_rc 
ISql::fromJT(Bindings& bind, JoinTables* jt)
{
	str_buf buf;
	buf << " FROM";
	//zend_printf("buflen %ld, %ld\n", buf.size(), buf.len());

	htab_rd joins(jt->getData());

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
			obj_rc  owner(tcol->getOwner());
			// Owner is an "Operation" , usually a Select
			//showobj("subq owner",subq);
			obj_rc plist_mgr(owner.call(SQSTR.get_sql_params));
		    ParamList* plist = zobj_toc<ParamList>(plist_mgr);
		    str_rc   sub_sql = plist->getSql();

			//val_rc subq_sql = subq.call(SQSTR.getsql);
			buf << " (" << sub_sql << ")";
		}
		buf << ' ' << l_alias;

		htab_rd  expr = ji->getConditions();
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
				name = jex->emit(jix, &bind, l_alias, r_alias );
				buf << name;
			}
		}
	}

	return buf.zstr();


}

str_rc 
ISql::select_jt(Bindings& bind, JoinTables* jt)
{
	str_buf buf;

	val_ptr aggregate = bind.get(ISql::SQL_AGGREGATE);
	val_ptr distinct = bind.get(ISql::SQL_DISTINCT);

	if (aggregate.isArray())
	{
		htab_rd agg(aggregate.zarray());
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

		buf << ") AS " << this->quoteName(function);

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

		htab_rd tables = jt->getTables();
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
	buf << this->fromJT(bind,jt);

	return buf.zstr();
}




str_rc 
ISql::limit(ParamList* plist, htab_rd ltab)
{
	val_ptr limit_val = ltab.get(SQSTR.limit);
	val_ptr offset_val = ltab.get(SQSTR.offset);

	str_buf buf;

	if (!limit_val.isNull())
	{
		buf << " LIMIT " << plist->paramLiteral(limit_val);
	}
	if (!offset_val.isNull())
	{
		buf << " OFFSET " << plist->paramLiteral(offset_val);
	}

	str_rc result = buf.zstr();

	//showstr("limit", result);
	return result;

}

obj_rc
ISql::select(Bindings& bind)
{
	JoinTables* from = bind.getJoinTables();

	str_buf buf;

	str_rc what = this->select_jt(bind, from);

	buf << "SELECT" << what;

	// TODO: still need this, check for old-style join specification? 
	/*
	zval_own join = bind.get(ISql::SQL_JOIN);
	if (!join.isNull())
	{
		buf << this->join(join);
	}
	*/

	obj_rc   pobj = bind.getParamList();

	ParamList* plist = zobj_toc<ParamList>(pobj);

	val_ptr where = bind.get(SQL_WHERE);
	if (where.isArray())
	{
		buf << " WHERE" << this->where(bind, where.zarray());
	}

	val_ptr order = bind.get(SQL_ORDER);
	if (order.isArray())
	{
		buf << " ORDER BY" << this->orderBy(order.zarray());
	}

	val_ptr limit = bind.get(SQL_LIMIT);

	if (limit.isArray())
	{
		buf << this->limit(plist, limit.zarray());
	}

	str_rc sql = buf.zstr();


	plist->setSql(sql);
	plist->useOwnValues();
	return pobj;
}

obj_rc
ISql::update(Bindings& bind)
{
	str_buf buf;

	JoinTables* joins = bind.getJoinTables();
	htab_rd   tables = joins->getTables();

	htab_walk wk;

	if (!wk.start(tables))
	{
		throw std::runtime_error("Update table not set");
	}

	auto current = wk.value();
	IColumns* tcol = zval_toc<IColumns>(current);

	buf << "UPDATE " << this->quoteName(tcol->getName()) << " SET";

	val_ptr upset = bind.get(SQL_UPDATE);
	obj_rc result = bind.getParamList();

	ParamList* plist = zobj_toc<ParamList>(result);

	if (upset.isArray())
	{
		int ix = 0;
		for (wk.start(upset.zarray()); wk.ok(); wk.next(), ix++)
		{
			htab_rd pair = current.zarray();
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

		htab_rd uset = icol->getColNames();
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
	if (where_val.isArray())
	{
		buf << " WHERE" << this->where(bind, where_val.zarray());
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

str_rc
ISql::where(Bindings &bind, htab_rd wtab)
{
	str_buf buf;

	// TODO: jtables set wether needed or not!
	JoinTables* jtab = bind.getJoinTables();

	htab_rd jtables(jtab->getTables());

	obj_rc paramList = bind.getParamList();
	ParamList* params = zobj_toc<ParamList> (paramList);

	str_rc bop; // sql boolean operator eg "AND"
	val_ptr wcol; // column name or object, being processed from where entry
	val_ptr value; // another temporary zval
	
	val_ptr not_value; // boolean true or false for "NOT"

	val_ptr harray; // a data array
	obj_rc part; // part, some kind of SqlPartId.

	SqlPartId* sqlpart;

	int partid; // vaguely obsolete and trad. way of identifying the part.
	str_rc col_name; // column name as string

	htab_walk wk;
	auto wix = wk.key();
	auto where_zval = wk.value();

	for(wk.start(wtab); wk.ok(); wk.next())
	{
		//showmem("where_zval", where_zval);
		htab_rd where_tab(where_zval.zarray());

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
				str_buf temp;
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
			htab_rd duo(harray.zarray());
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
			str_ptr key = wcol.zstr();

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
				htab_rd vlist(value);

				htab_rw plist(params->getParams());

				plist.merge(vlist);
				params->setParams(plist);
			}
		}
	}
	return buf.zstr();
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@



}; // namespace wcd


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

	htab_rd htab = cobj->getConditions();
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

	obj_rc obj = cobj->addJoin(jinfo);

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
	const htab_rc& htab = cobj->getData();
	htab.return_zv(return_value);
}

/* public function getModel() : ?string {} */
ZEND_METHOD(Wcd_Sql_JoinTables, getModel)
{
	ZEND_PARSE_PARAMETERS_NONE();
	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	str_ptr name = cobj->getModel();
	name.return_zv(return_value);
}

/* public function getOrder() : array {} */
ZEND_METHOD(Wcd_Sql_JoinTables, getOrder)
{
	ZEND_PARSE_PARAMETERS_NONE();
	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	const htab_rc& htab = cobj->getOrder();
	htab.return_zv(return_value);
}

/*  public function getPivot(): ?IColumns {} */
ZEND_METHOD(Wcd_Sql_JoinTables, getPivot)
{
	ZEND_PARSE_PARAMETERS_NONE();
	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
    obj_rc obj = cobj->getPivot();
	obj.move_zv(return_value);
}

/* public function getPrime(): ?IColumns  {} */
ZEND_METHOD(Wcd_Sql_JoinTables, getPrime)
{
	ZEND_PARSE_PARAMETERS_NONE();
	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	obj_rc obj = cobj->getPrime();
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
	obj_rc obj = cobj->getTable(name);
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
	obj_rc obj = cobj->getTableAlias(name);
	obj.move_zv(return_value);
}

/* public function getTables(): array {} */
ZEND_METHOD(Wcd_Sql_JoinTables, getTables)
{
	ZEND_PARSE_PARAMETERS_NONE();
	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	const htab_rc& htab = cobj->getTables();
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

	obj_rc obj = JoinTables::rowSplit(row, rename);
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
	cobj->setPrime(val_ptr(tcol).zobject());
}

/* public function delete(Bindings $bind) : ParamList {} */
ZEND_METHOD(Wcd_Sql_ISql, deleteSql)
{
	zval* bind;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(bind, zclass_bindings)
	ZEND_PARSE_PARAMETERS_END();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);
	Bindings& refbind = *zval_toc<Bindings>(bind);

	obj_rc plist = cobj->deleteSql(refbind);
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

	str_rc result = cobj->emit(part, pbind, lalias, ralias);
	result.move_zv(return_value);

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

	obj_rc plist = cobj->insert(*refbind);
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

	obj_rc plist = cobj->select(refbind);
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

	str_rc sql = cobj->setSeqValue(value, data);
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

	str_rc sql = cobj->truncate(refbind);
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

	obj_rc plist = cobj->update(refbind);
	plist.move_zv(return_value);
}

/* public function valuesDefault(): string {} */
ZEND_METHOD(Wcd_Sql_ISql, valuesDefault)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ISql* cobj = zval_toc<ISql>(ZEND_THIS);
	str_rc result = cobj->valuesDefault();
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
	str_rc result = cobj->addParamList(value);
	result.move_zv(return_value);
}

/* public function getParams() : array {} */
ZEND_METHOD(Wcd_Sql_ParamList, getParams)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
	htab_rd htab = cobj->getParams();
	htab.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_ParamList, getReturns)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
	htab_rd htab = cobj->getReturns();
	htab.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_ParamList, getSql)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
	str_ptr sql = cobj->getSql();
	sql.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_ParamList, getValues)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
	htab_rd htab = cobj->getValues();
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

	str_rc result = cobj->makeList(start, count);
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

	str_rc result = cobj->paramLiteral(value);
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