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

#ifndef WCD_SQL_POSTGRES_H
#include "postgres.h"
#endif

#ifndef WCC_DEBUGLOG_H
#include "debuglog.h"
#endif

namespace wcd {

using namespace zpp;

base_obj_mgr<ParamList> 	ParamList::omg;
base_obj_mgr<Bindings>  	Bindings::omg;
base_obj_mgr<JoinTables> 	JoinTables::omg;
base_obj_mgr<JoinInfo> 		JoinInfo::omg;


zend_class_entry* zclass_join_info;
zend_class_entry* zclass_join_tables;
zend_class_entry* zclass_bindings;
zend_class_entry* zclass_param_list;



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

	di.set(SQSTR.db_ref, dbref_);
	di.set(SQSTR.driver, driver_);
}

void 
ParamList::construct(weak_ref& driver)
{
	dbref_ = driver;
}

str_rc 
ParamList::paramStr(int ct)
{
	if (!driver_.ok())
	{
		driver_ = dbref_.get();
	}
	return driver_.call(SQSTR.param, val_rc(ct));
}

str_rc 
ParamList::addParam(val_ptr value)
{
	str_rc result;
	htab_rw hw(params_);

/*
	str_rc result = paramStr((int) params_.size()+1);

	if ((result.size() == 1) && result.starts_with('?'))
	{
		hw.push_back(value);
	}
	else {
		hw.set(result, value);
	}
*/
	unsigned nextid = hw.size() + 1;
	result = paramStr(nextid);
	hw.set(value, result);
	DebugLog* log = DebugLog::cpp_global();
	if (log)
	{
		log->dump("addParam", params_);
	}
	return result;
}

str_rc 
ParamList::addParamEquals(val_ptr value)
{
	str_rc result;
	htab_rw hw(params_);

	unsigned nextid = params_.size()+1;
	result = paramStr(nextid);
	hw.set(result, value);
	DebugLog* log = DebugLog::cpp_global();
	if (log)
	{
		log->dump("addParamEquals", params_);
	}
	return result;
}

void 
ParamList::setSql(str_ptr s)
{
	sql_ = s;
}

str_ptr 
ParamList::getSql()
{
	return sql_;
}

void 
ParamList::useOwnValues()
{
	val_params_ = params_;
}

str_return
ParamList::paramLiteral(val_ptr value)
{
	str_return result;

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
				result = this->addParamEquals(temp);
				break;
			case SqlPartId::LIT_PID:
				temp = static_cast<Literal*>(part)->getValue();
				result = val_ptr(temp).to_zstr();
				break;
			}
		}

	}
	else {
		result = this->addParamEquals(value);
	}
	if (!result.value_.size()) {
		result.error() << "Unhandled paramLiteral argument";
	}
	return result;
}

str_rc 
ParamList::addParamList(htab_ptr values)
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
	driver_.init();
	
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

void
JoinInfo::destruct()
{
	//showobj("Left table - ", leftTable_);
	//showobj("Right table - ", rightTable_);
	leftTable_.init();
	rightTable_.init();
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

int_return //static
JoinInfo::getJoinType(str_ptr s)
{
	int_return result;


	if (s.size() == 0)
	{
		result = J_ERROR;
		result.error() << "Empty join string";
	}
	else {
		const char* c = s.data();
		int c1 = toupper(c[0]);
		switch(c1)
		{
		case 'I':
			result = J_INNER;
			break;
		case 'L':
			result = J_LEFT;
			break;
		case 'R':
			result = J_RIGHT;
			break;
		case 'F':
			result = J_FULL;
			break;
		default:
			result = J_ERROR;
			break;
		}
	}
	if (result == J_ERROR)
	{
		result.error() << "Unknown Join specifier: " << s;
	}
	return result;

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
JoinTables::destruct()
{
	prime_.init();
	//showarray("joins_", joins_);
	htab_rw hw(joins_);

	hw.clear();
	byAlias_.init();
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

error_return 
JoinTables::addTable(obj_ptr icol)
{
	error_return result;

	IColumns* tc = zobj_toc<IColumns>(icol);
	str_ptr name = tc->getAlias();
	if (name.isNull())
	{	
		name = tc->getName();
	}
	if (name.isNull())
	{
		result.error() << "addTable with no Alias or Name";
		return result;
	}
	htab_rw(byAlias_).set(name, icol);
	return result;
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
JoinTables::order(str_ptr name, bool descend)
{
	htab_rc pair;
	val_rc desc;

	desc.set_bool(descend);
	htab_rw hw(pair);

	hw.set(SQSTR.column, name);
	hw.set(SQSTR.desc, desc);

	htab_rw(orderby_).push_back(pair);
}

obj_rc //static
JoinTables::rowSplit(htab_ptr row, htab_ptr rename)
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

//                     @@@@@@@@@@@@@@@@@@@@@@@@



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

ZEND_METHOD(Wcd_Sql_JoinInfo, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();
	JoinInfo* cobj = zval_toc<JoinInfo>(ZEND_THIS);
	cobj->destruct();
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

	htab_ptr htab = cobj->getConditions();
	htab.copy_zv(return_value);
}

/* static public function toJoinType(string $join) : int {} */
ZEND_METHOD(Wcd_Sql_JoinInfo, toJoinType)
{
	zend_string* join;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(join)
	ZEND_PARSE_PARAMETERS_END();

	int_return result = JoinInfo::getJoinType(join);

	result.throw_errors();
	RETURN_LONG(result.value_);

}

ZEND_METHOD(Wcd_Sql_JoinTables, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();
	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	cobj->destruct();

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
	htab_ptr htab = cobj->getData();
	htab.copy_zv(return_value);
}

/* public function getModel() : ?string {} */
ZEND_METHOD(Wcd_Sql_JoinTables, getModel)
{
	ZEND_PARSE_PARAMETERS_NONE();
	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	str_ptr name = cobj->getModel();
	name.copy_zv(return_value);
}

/* public function getOrder() : array {} */
ZEND_METHOD(Wcd_Sql_JoinTables, getOrder)
{
	ZEND_PARSE_PARAMETERS_NONE();
	JoinTables* cobj = zval_toc<JoinTables>(ZEND_THIS);
	htab_ptr htab = cobj->getOrder();
	htab.copy_zv(return_value);
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
	htab_ptr htab = cobj->getTables();
	htab.copy_zv(return_value);
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


/* public function __construct(\Wcd\IDriver $gen); */
ZEND_METHOD(Wcd_Sql_ParamList, __construct)
{
	weak_ref  wref;

	zarg_rd args(execute_data);

	args.weakref(wref, args.need(0));

	if (!args.throw_errors())
	{
		ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
		cobj->construct(wref);
	}
}

/* public function addParam(mixed $value) : void {} */
ZEND_METHOD(Wcd_Sql_ParamList, addParam)
{
	zarg_rd args(execute_data);
	zval* value = args.need(0);

	if (!args.throw_errors())
	{
		ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
		cobj->addParam(value);
	}
}

/* public function addParam(mixed $value) : void {} */
ZEND_METHOD(Wcd_Sql_ParamList, addParamEquals)
{
	zarg_rd args(execute_data);
	zval* value = args.need(0);

	if (!args.throw_errors())
	{
		ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
		cobj->addParamEquals(value);
	}
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
	htab_ptr htab = cobj->getParams();
	htab.copy_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_ParamList, getReturns)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
	htab_ptr htab = cobj->getReturns();
	htab.copy_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_ParamList, getSql)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
	str_ptr sql = cobj->getSql();
	sql.copy_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_ParamList, getValues)
{
	ZEND_PARSE_PARAMETERS_NONE();

	ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
	htab_ptr htab = cobj->getValues();
	htab.copy_zv(return_value);
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

	str_return result = cobj->paramLiteral(value);
	result.throw_errors();
	result.value_.move_zv(return_value);
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
	str_ptr sql;

	zarg_rd args(execute_data);

	args.zstring(sql, args.need(0));

	if (!args.throw_errors())
	{
		ParamList* cobj = zval_toc<ParamList>(ZEND_THIS);
		cobj->setSql(sql);
	}	
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

	// class register must be done in dependency order

	ISql::register_class();

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