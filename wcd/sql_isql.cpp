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


base_obj_mgr<Bindings>  	Bindings::omg;
base_obj_mgr<JoinTables> 	JoinTables::omg;
base_obj_mgr<JoinInfo> 		JoinInfo::omg;


zend_class_entry* zclass_join_info;
zend_class_entry* zclass_join_tables;
zend_class_entry* zclass_bindings;




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

	auto zce_iparams = IParams::register_class();
	NamedParams::register_class(zce_iparams);

	zclass_join_tables = register_class_Wcd_Sql_JoinTables();
	JoinTables::omg.classEntry(zclass_join_tables);

	zclass_join_info = register_class_Wcd_Sql_JoinInfo();
	JoinInfo::omg.classEntry(zclass_join_info);

	return true;

}
//sql_isql.cpp
#endif