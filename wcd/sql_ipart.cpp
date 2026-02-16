#ifndef SQL_IPART_CPP
#define SQL_IPART_CPP


#ifndef SQL_ARGINFO_H
#define SQL_ARGINFO_H
extern "C" {
	#include "stub/sqlipart_arginfo.h"
};
#endif

#ifndef SQL_IPART_H
#include "sql_ipart.h"
#endif

#ifndef SQL_ISQL_H
#include "sql_isql.h"
#endif

namespace wcd {

using StrView = std::string_view;

using namespace zpp;

base_obj_mgr<Literal> Literal::omg;
base_obj_mgr<Expr> Expr::omg;
base_obj_mgr<JoinExpr> JoinExpr::omg;
base_obj_mgr<IColumns> IColumns::omg;
base_obj_mgr<TableAttr> TableAttr::omg;
base_obj_mgr<TColumns> TColumns::omg;
base_obj_mgr<Param> Param::omg;

sql_strtab SQSTR;

zend_class_entry* zclass_sql_ifipart;
zend_class_entry* zclass_sql_icolumns;
zend_class_entry* zclass_sql_tcolumns;
zend_class_entry* zclass_table_attr;
zend_class_entry* zclass_join_expr;


void sql_strtab::init() 
{

	str_space = " ";
	partid = "part_id";
	value = "value";
	values_key = "values";
	
	dot_char = ".";
	comma_char = ",";
	single_quote = "'";
	single_q2 = "''";

	mysql_empty = "``";
	default_values = "DEFAULT VALUES";
	delete_from = "DELETE FROM";

	emit = "emit";


	lhs_val =  "lhs_val";
	rhs_val = "rhs_val";
	val_op = "val_op";
	next_op = "next_op";
	
	quotename = "quotename";
	alias = "alias";
	columns = "columns";
	namekey = "name";
	valuekey = "value";

	colexpr = "colexpr";
	owner = "owner";
	table = "table";
	attr = "attr";

	isql = "isql";
	param = "param";

	ok = "ok";
	asc = "asc";
	desc = "desc";

	limit = "limit";
	offset = "offset";
	
	column = "column";
	boolean = "boolean";

	operator_key = "operator";
	nested = "nested";
	function = "function";
	seq_key = "seq";

	condition1 = "condition1";
	condition2 = "condition2";

	not_key = "not";
	asterisk = "*";
	and_str = "AND";
	count_str = "COUNT";

	basic = "basic";
	jointype = "joinType";
	typekey = "type";
	data_key = "data";

	is_null = "IS NULL";
	is_not_null = "IS NOT NULL";
	op_like = "LIKE";
	op_and = "AND";
	op_or = "OR";
	cmp_equal = "=";


	getTableModel = "getTableModel";
	getColDefs = "getColDefs";

	param_list = "param_list";
	connect = "connect";
	idriver = "idriver";
	getfetch = "getfetch";
	setfetch = "setfetch";
	fetch_key = "fetch";

	params = "params";
	driver = "driver";
	sql = "sql";
	returns_str = "returns";
	return_str = "return";
	bind_key = "bind";
	id_key = "identity";
	default_key = "default";
	auto_inc = "auto_inc";

	results  = "results";
	by_alias  = "by_alias";
	model  = "model";
	modelclass = "modelclass";
	join_tables = "jointables";
	joins  = "joins";
	prime  = "prime";
	orderby  = "orderby";
	where  = "where";

	get_sql_params = "getsqlparams";
	valuesdefault = "valuesdefault";
	eager_load = "eager_load";
	statement = "stmt";
	db_ref = "db_ref";
	queryString = "queryString";
	icolumns = "icols";
	auto_alias = "auto_alias";

	nulls = "nulls";
	last = "last";
	first = "first";
	nulls_last = "nulls_last";
	
	opstr = {
		{"="}, {"<>"}, {">"}, {"<"}, {">="}, {"<="}, {op_like},
        {op_and}, {op_or}, {is_null}, {is_not_null}, {str_space}
	};

	boolstr = {
		 {str_space}, {op_and}, {op_or}
	};

	joinstr = {
		{"INNER"}, {"LEFT"}, {"RIGHT"}, {"FULL"}
	};
}

void 
Literal::construct(val_ptr val)
{
	obj_ptr self(vobj());
	self.property(SQSTR.value, val);
}

val_ptr
Literal::getValue() const
{
	obj_ptr self(vobj());
	return self.property_ptr(SQSTR.value);
}

str_rc 
Literal::toString() const
{
	const char k_sqt = '\'';

	val_ptr temp = getValue();
	if (temp.isString())
	{
		str_buf buf;
		buf << k_sqt << str_replace(StrView("'"), StrView("''"), temp) << k_sqt;
		return buf.zstr();
	}
	else {
		return temp.to_zstr();
	}
}

void Literal::debug_info(htab_rw di)
{
	base_d::debug_info(di);
	di.set(SQSTR.partid, LIT_PID);
	//di.set(SQSTR.value, value_);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Expr::debug_info(htab_rw di)
{
	di.set(SQSTR.partid, EXPR_PID);
	di.set(SQSTR.value,  expr_);
}

void Expr::construct(str_ptr val)
{
	expr_ = val;
	//showstr("construct expr", expr_);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


void 
JoinExpr::debug_info(htab_rw di)
{
	di.set(SQSTR.partid, JE_PID);
	di.set(SQSTR.lhs_val, lattr_);
	di.set(SQSTR.rhs_val, rattr_);
	di.set(SQSTR.val_op, op_);
	di.set(SQSTR.next_op, nextop_);
}

str_ptr //static
JoinExpr::opStr(int op)
{
	return SQSTR.opstr[op];
}

str_ptr //static
JoinExpr::boolStr(int nextop)
{
	return SQSTR.boolstr[nextop];
}

void 
JoinExpr::construct(val_ptr leftval, val_ptr rightval, int op, int nextop)
{
	lattr_ = leftval;
	rattr_ = rightval;
	op_ = op;
	nextop_ = nextop;
}

void
JoinExpr::destruct()
{
	lattr_.set_null();
	rattr_.set_null();
}

str_return 
JoinExpr::emit(int ix, Bindings* bind, 
	str_ptr Lalias, str_ptr Ralias)
{

	str_return result;

	str_buf sqlbuf;

	str_return	temp;

	const char blank = ' ';

	ISql* isql = zobj_toc<ISql>(bind->isql());

	// if not the first
	if ((ix > 0) && (nextop_ != B_NULL))
	{
		sqlbuf << blank << SQSTR.boolstr[nextop_];
	}

	val_ptr L_attr(lattr_);
	val_ptr R_attr(rattr_);

	if (!L_attr.isNull())
	{
		sqlbuf << blank;
		if (L_attr.isObject())
		{
			temp = isql->emit(L_attr, bind, Lalias, Ralias); 
		}
		else if (L_attr.isString())
		{
			temp = isql->quoteName(L_attr.zstr());
			sqlbuf << Lalias << '.';
		}
		else {
			temp = L_attr.to_zstr();
		}
		
		if (temp.has_errors())
		{
			result = std::move(temp);
			goto RET_ALL;
		}
		sqlbuf << temp.value_;
	}

	if (op_ < OP_NOP)
	{
		sqlbuf << blank << SQSTR.opstr[op_];
	}

	if (!R_attr.isNull())
	{	
		sqlbuf << blank;
		if (R_attr.isObject()) 
		{
			temp = isql->emit(R_attr,  bind, Lalias, Ralias); 
		}
		else if (R_attr.isString())
		{
			temp = isql->quoteName(R_attr.zstr());
			sqlbuf << Ralias << '.';
		}
		else {
			temp = R_attr.to_zstr();
		}
		if (temp.has_errors())
		{
			result = std::move(temp);
		}
		sqlbuf << temp.value_;

	}
RET_ALL:
	result.value_ = sqlbuf.zstr();
	return result;
}

int_return
JoinExpr::toLogic(str_ptr s)
{
	size_t opsize = s.size();
	int_return result;

	result = B_ERROR;

	if (opsize)
	{
		const char* c = s.data();
		int c1 = toupper(c[0]);
		switch(c1)
		{
		case ' ':
			result = B_NULL;
			break;
		case 'A':
			result = B_AND;
			break;
		case 'O':
			result = B_OR;
			break;
		default:
			result = B_ERROR;
			result.error() << " - invalid toLogic arg: " << s;
			break;
		}
	}
	else {
		result.error() << " empty string " << s;
	}

	return result;
}

int_return
JoinExpr::toOperator(str_ptr s)
{
	size_t opsize = s.size();

	int_return result;

	if (!opsize)
	{
		result.error() << "empty string";
	}
	else {
		const char* c = s.data();

		int c1 = toupper(c[0]);
		int c2 = 0;
		str_rc upw;
		str_ptr ps(s);

		result = OP_ERROR;

		switch(c1)
		{
		case '=':
			result = OP_EQ;
			break;
		case '<':
			if (opsize==1)
			{
				result = OP_LT;
				break;
			}	
			else if (opsize==2)
			{
				c2 = toupper(c[1]);
				if (c2 == '>')
				{
					result = OP_NEQ;
				}
				else if (c2 == '=')
				{
					result = OP_LE;
				}
			}
			break;
		case '>':
			if (opsize==1)
			{
				result = OP_GT;
			}
			else if (opsize==2)
			{
				c2 = toupper(c[1]);
				if (c2 == '=')
				{
					result = OP_GE;
				}
			}
			break;
		case 'L':
			upw = ps;
			upw.uppercase();
			if (upw.vstr() == SQSTR.op_like.vstr()) {
				result = OP_LIKE;
			}
			break;
		case 'A':
			upw = ps;
			upw.uppercase();
			if (upw.vstr() == SQSTR.op_and.vstr()) {
				result = OP_AND;
			}
			break;
		case 'O':
			upw = ps;
			upw.uppercase();
			if (upw.vstr() == SQSTR.op_or.vstr())
			{
				result =  OP_OR;
			}
			break;
		case 'I':
			upw = ps;
			upw.uppercase();
			if (upw.vstr() == SQSTR.is_null.vstr())
			{
				result =  OP_ISNULL;
			}
			if (upw.vstr() == SQSTR.is_not_null.vstr()) {
				result =  OP_NOTNULL;		
			}
			break;
		default:
			result = OP_ERROR;
			result.error() << "Invalid toOperator arg: " << s;
			break;
		}
	}
	return result;
	
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void
IColumns::debug_info(htab_rw di)
{
	di.set(SQSTR.partid, ICOL_PID);
	di.set(SQSTR.alias, alias_);
	di.set(SQSTR.columns, colnames_);
	di.set(SQSTR.colexpr, expr_);
	di.set(SQSTR.owner, owner_);
}	

void 
IColumns::construct(obj_ptr owner)
{
	setOwner(owner);
}

obj_rc 
IColumns::getOwner()
{
	obj_rc ref;

	if (owner_.ok())
	{
		ref = weakref_get(owner_);
	}
	return ref;
}

void IColumns::setOwner(obj_ptr obj)
{
	//showobj("Set Owner", obj);

	if (obj.ok())
	{
		owner_ = weakref_create(obj);
	}
	else {
		owner_.init();
	}
}

void
IColumns::destruct()
{
	//showobj("IColumns owner", owner_);
	owner_.init();
}

void IColumns::clear()
{
	colnames_.reset();
	expr_.reset();
	alias_.init();
}

void IColumns::addExpr(str_ptr alias, str_ptr expr)
{
	htab_rw exp_w(expr_);
	exp_w.set(alias, expr);
}
		
void 
IColumns::add(htab_ptr columns)		
{
	if (columns.ok())
	{
		htab_walk wk;

		val_rc vFalse;

		vFalse.set_bool(false);

		auto ix = wk.key();
		auto name = wk.value();
		htab_rw names(colnames_);

		for(wk.start(columns); wk.ok(); wk.next())
		{
			if (ix.isLong()) 
			{
				names.set(name.zstr(), vFalse);
			}
			else {
				names.set(ix.zstr(), name);
			}
		}
	}
}

void 
IColumns::setColAlias(str_ptr name, str_ptr alias)
{
	val_rc  val;

	if (alias.isNull())
	{
		val.set_bool(false);
	}
	else {
		val = alias;
	}
	htab_rw hw(colnames_);
	hw.set(name, val);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

obj_rc
TableAttr::makeTA(str_ptr t, str_ptr a)
{
	obj_rc result = TableAttr::omg.new_zobj();
	TableAttr* ta = zobj_toc<TableAttr>(result);
	ta->construct(t, a);
	return result;
}

val_rc
TableAttr::splitDot(str_ptr s)
{
	int pos = s.find('.');
	val_rc result;

	if (pos > 0)
	{
		str_rc  tname = s.substr(0,pos);
		str_rc  tattr = s.substr(pos+1);
		result = makeTA(tname, tattr);
	}
	else {
		result = s;

	}
	//showmem("SplitDot result", result);
	return result;

}

void 
TableAttr::construct(str_ptr t, str_ptr a)
{
	table_ = t;
	attr_ = a;
	//showstr("table_", table_);
	//showstr("attr_", attr_);
}

void TableAttr::debug_info(htab_rw di)
{
	di.set(SQSTR.partid, TA_PID);
	di.set(SQSTR.table, table_);
	di.set(SQSTR.attr, attr_);

}

str_rc 
TableAttr::toString() const
{
	str_buf buf;

	buf << table_ << '.' << attr_;
	return buf.zstr();
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void // virtual
TColumns::debug_info(htab_rw di)
{
	IColumns::debug_info(di);

	di.set(SQSTR.partid, TCOL_PID);
	di.set(SQSTR.namekey, name_);
	di.set(SQSTR.attr, attr_map_);
}	

void 
TColumns::construct(str_ptr tname,  str_ptr talias,  val_ptr tcol)
{
	val_rc null_owner;

	this->IColumns::construct(null_owner);

	this->name_ = tname;

	if (!talias.isNull())
	{
		this->alias_ = talias;
	}

	if (tcol.isArray())
	{
		this->add(tcol);
	}
	else if (tcol.isString())
	{
		str_ptr falseval;

		this->setColAlias(tcol.zstr(), falseval);
	}
}

void
TColumns::destruct()
{
	//showstr("~TColumns ", name_);
	IColumns::destruct();
}

val_rc
TColumns::attr(str_ptr name)
{
	val_rc result;
	val_ptr test;

	if (attr_map_.try_fetch(name, test))
	{
		result = test;
		return result;
	}

	str_ptr table(this->alias_);
	if (table.isNull())
	{
		table = this->name_;
	}

	obj_rc ta = TableAttr::makeTA(table, name);

	result = ta;
	htab_rw hw(attr_map_);

	hw.set(name, result);

	return result;
}

void 
TColumns::setName(str_ptr name)
{
	this->name_ = name;
}

val_rc //static
TColumns::tableCol(str_ptr expr)
{
	

	obj_rc  zobj = omg.new_zobj();
	TColumns* cobj = zobj_toc<TColumns>(zobj);

	str_ptr nullstr;
	val_ptr tcol;

	val_rc  pair_z = explode(SQSTR.dot_char, expr);
	val_ptr pair(pair_z);

	if (pair.isArray())
	{
		htab_ptr hpair(pair.zarray());
		str_ptr name = hpair.get(int(0));
		
		tcol = hpair.get(int(1));

		cobj->construct(name, nullstr, tcol);
	}
	else {
		cobj->construct(expr, nullstr, tcol);
	}
	return val_rc(zobj);
	//zval_own explode(str_ptr sep,  str_ptr split, long limit = 0);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void 
Param::debug_info(htab_rw di)
{
	base_d::debug_info(di);
	//di.set(SQSTR.valuekey, value_);
}

void 
Param::construct(val_ptr zp)
{
	// first make value_ point to actual property

	obj_ptr self(vobj());
	self.property(SQSTR.value, zp);
}

val_ptr
Param::getValue() const
{
	obj_ptr self(vobj());
	return self.property_ptr(SQSTR.value); // for reads only
}

}; //namespace wcd

using namespace wcd;
using namespace zpp;

ZEND_METHOD(Wcd_Sql_Literal, __construct)
{
	zval* value;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	Literal* cobj = zval_toc<Literal> (ZEND_THIS);
	cobj->construct(value);

}

ZEND_METHOD(Wcd_Sql_Literal, getValue)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Literal* cobj = zval_toc<Literal> (ZEND_THIS);
	val_ptr value = cobj->getValue();
	value.copy_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Literal, getPartId)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG(SqlPartId::LIT_PID);
}

ZEND_METHOD(Wcd_Sql_Literal, __toString)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Literal* cobj = zval_toc<Literal> (ZEND_THIS);
	str_rc result = cobj->toString();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Expr, __construct)
{
	zend_string* value;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(value)
	ZEND_PARSE_PARAMETERS_END();

	Expr* cobj = zval_toc<Expr> (ZEND_THIS);
	cobj->construct(value);

}

ZEND_METHOD(Wcd_Sql_Expr, getPartId)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG(SqlPartId::EXPR_PID);
}


ZEND_METHOD(Wcd_Sql_Expr, __toString)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Expr* cobj = zval_toc<Expr> (ZEND_THIS);
	str_ptr s = cobj->toString();
	s.copy_zv(return_value);	
}

ZEND_METHOD(Wcd_Sql_JoinExpr, __construct)
{
	zval* LAttr;
	zval* RAttr = nullptr;
	zend_long  op = JoinExpr::OP_EQ;
	zend_long  nextop = JoinExpr::B_AND;

	ZEND_PARSE_PARAMETERS_START(1,4)
	Z_PARAM_ZVAL(LAttr)
	Z_PARAM_OPTIONAL
	Z_PARAM_ZVAL(RAttr)
	Z_PARAM_LONG(op)
	Z_PARAM_LONG(nextop)
	ZEND_PARSE_PARAMETERS_END();

	JoinExpr* cobj = zval_toc<JoinExpr> (ZEND_THIS);
	cobj->construct(LAttr, RAttr, op, nextop);
}

ZEND_METHOD(Wcd_Sql_JoinExpr, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();
	JoinExpr* cobj = zval_toc<JoinExpr> (ZEND_THIS);
	cobj->destruct();
}


ZEND_METHOD(Wcd_Sql_JoinExpr, getPartId)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG(SqlPartId::JE_PID);
}

ZEND_METHOD(Wcd_Sql_JoinExpr, emit)
{
	zend_long ix;
	zval* bindings;
	zend_string* lalias;
	zend_string* ralias;

	ZEND_PARSE_PARAMETERS_START(4,4)
	Z_PARAM_LONG(ix)
	Z_PARAM_ZVAL(bindings)
	Z_PARAM_STR(lalias)
	Z_PARAM_STR(ralias)
	ZEND_PARSE_PARAMETERS_END();

	JoinExpr* cobj = zval_toc<JoinExpr> (ZEND_THIS);
	Bindings* bind = zval_toc<Bindings>(bindings);
	
	str_return result = cobj->emit(ix, bind, lalias, ralias);
	result.throw_errors();
	result.value_.move_zv(return_value);

}

ZEND_METHOD(Wcd_Sql_JoinExpr, opstr)
{
	zend_long ix;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(ix)
	ZEND_PARSE_PARAMETERS_END();

	str_ptr s = JoinExpr::opStr(ix);
	s.copy_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_JoinExpr, boolstr)
{
	zend_long ix;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(ix)
	ZEND_PARSE_PARAMETERS_END();

	str_ptr s = JoinExpr::boolStr(ix);
	s.copy_zv(return_value);
}

//@@@@@@@@@@@@@ 

ZEND_METHOD(Wcd_Sql_IColumns, __construct)
{
	zarg_rd args(execute_data);


	obj_rc owner;

	args.obj_null(owner, args.need(0));

	if (!args.throw_errors())
	{
		IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
		cobj->construct(owner);
	}
}

ZEND_METHOD(Wcd_Sql_IColumns, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
	cobj->destruct();
}

ZEND_METHOD(Wcd_Sql_IColumns, getPartId)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG(SqlPartId::ICOL_PID);
}

ZEND_METHOD(Wcd_Sql_IColumns, clear)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
	cobj->clear();
}

ZEND_METHOD(Wcd_Sql_IColumns, getOwner)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
	obj_rc obj = cobj->getOwner();
	obj.move_zv(return_value);
}


ZEND_METHOD(Wcd_Sql_IColumns, getColNames)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
	htab_ptr array = cobj->getColNames();
	array.copy_zv(return_value);

}

ZEND_METHOD(Wcd_Sql_IColumns, setAlias)
{
	zend_string* name;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();
	IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
	cobj->setAlias(name);	
}

ZEND_METHOD(Wcd_Sql_IColumns, getAlias)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
	str_ptr str = cobj->getAlias();
	str.copy_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_IColumns, getName)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
	str_ptr str = cobj->getName();
	str.copy_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_IColumns, setColAlias)
{
	zend_string* name;
	zend_string* alias = nullptr;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_STR(name)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR(alias)
	ZEND_PARSE_PARAMETERS_END();
	IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
	cobj->setColAlias(name,alias);
}

ZEND_METHOD(Wcd_Sql_IColumns, setOwner)
{
	zarg_rd args(execute_data);

	obj_ptr owner;

	args.obj_null(owner, args.need(0));

	if (!args.throw_errors())
	{
		IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
		cobj->setOwner(cobj);
	}
	
}

ZEND_METHOD(Wcd_Sql_IColumns, add)
{
	zarg_rd args(execute_data);

	htab_ptr cols;

	args.zarray(cols, args.need(0));

	if (!args.throw_errors())
	{
		IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
		cobj->add(cols);
	}	
}

ZEND_METHOD(Wcd_Sql_IColumns, unsetCol)
{
	zend_string* name;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();
	IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
	cobj->unsetCol(name);
}

ZEND_METHOD(Wcd_Sql_IColumns, has)
{
	zend_string* name;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();
	IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
	RETURN_BOOL(cobj->has(name));

}

ZEND_METHOD(Wcd_Sql_IColumns, hasColNames)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
	RETURN_BOOL(cobj->hasColNames());
}

ZEND_METHOD(Wcd_Sql_IColumns, addExpr)
{
	zend_string* name;
	zend_string* expr;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(name)
	Z_PARAM_STR(expr)
	ZEND_PARSE_PARAMETERS_END();

	IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
	cobj->addExpr(name,expr);
}

ZEND_METHOD(Wcd_Sql_IColumns, getExpr)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
	htab_ptr array = cobj->getExpr();
	array.copy_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_TableAttr, __construct)
{
	zend_string* tname;
	zend_string* attr;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(tname)
	Z_PARAM_STR(attr)
	ZEND_PARSE_PARAMETERS_END();

	TableAttr* cobj = zval_toc<TableAttr> (ZEND_THIS);

	cobj->construct(tname, attr);
}

ZEND_METHOD(Wcd_Sql_TableAttr, getPartId)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG(SqlPartId::TA_PID);
}

ZEND_METHOD(Wcd_Sql_TableAttr, __toString)
{
	ZEND_PARSE_PARAMETERS_NONE();

	TableAttr* cobj = zval_toc<TableAttr> (ZEND_THIS);

	str_rc s = cobj->toString();
	s.move_zv(return_value);

}

ZEND_METHOD(Wcd_Sql_TableAttr, getTable)
{
	ZEND_PARSE_PARAMETERS_NONE();
	TableAttr* cobj = zval_toc<TableAttr> (ZEND_THIS);
	str_ptr s = cobj->getTable();
	s.copy_zv(return_value);

}

ZEND_METHOD(Wcd_Sql_TableAttr, getAttr)
{
	ZEND_PARSE_PARAMETERS_NONE();
	TableAttr* cobj = zval_toc<TableAttr> (ZEND_THIS);
	str_ptr s = cobj->getAttr();
	s.copy_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_TableAttr, splitDot)
{
	zend_string* attr;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(attr)
	ZEND_PARSE_PARAMETERS_END();

	val_rc result = TableAttr::splitDot(attr);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcd_Sql_TColumns, __construct)
{
	zend_string* tname;
	zend_string* alias = nullptr;
	HashTable* tcol = nullptr;
	zend_string* tstr = nullptr;

	ZEND_PARSE_PARAMETERS_START(1,3)
	Z_PARAM_STR(tname)
	Z_PARAM_OPTIONAL
	Z_PARAM_STR_OR_NULL(alias)
	Z_PARAM_ARRAY_HT_OR_STR_OR_NULL(tcol, tstr)
	ZEND_PARSE_PARAMETERS_END();

	TColumns* cobj = zval_toc<TColumns> (ZEND_THIS);

	val_rc tempval;
	if (tcol)
	{
		tempval = tcol;
	}
	else if (tstr)
	{
		tempval = str_ptr(tstr);
	}

	cobj->construct(tname,alias,tempval);

}

ZEND_METHOD(Wcd_Sql_TColumns, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();
	TColumns* cobj = zval_toc<TColumns> (ZEND_THIS);
	cobj->destruct();
}

ZEND_METHOD(Wcd_Sql_TColumns, attr)
{
	zend_string* value;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(value)
	ZEND_PARSE_PARAMETERS_END();

	TColumns* cobj = zval_toc<TColumns> (ZEND_THIS);
	obj_rc obj = cobj->attr(value);
	obj.move_zv(return_value);

}

ZEND_METHOD(Wcd_Sql_TColumns, getPartId)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG(SqlPartId::TCOL_PID);
}


ZEND_METHOD(Wcd_Sql_TColumns, setName)
{
	zend_string* name;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(name)
	ZEND_PARSE_PARAMETERS_END();
	TColumns* cobj = zval_toc<TColumns> (ZEND_THIS);
	cobj->setName(name);
}

ZEND_METHOD(Wcd_Sql_TColumns, getName)
{
	ZEND_PARSE_PARAMETERS_NONE();
	TColumns* cobj = zval_toc<TColumns> (ZEND_THIS);
	str_ptr s = cobj->getName();
	s.copy_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_TColumns, tableCol)
{
	zend_string* expr;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(expr)
	ZEND_PARSE_PARAMETERS_END();

	val_rc obj = TColumns::tableCol(expr);

	obj.move_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Param, __construct)
{
	zval* value;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	Param* cobj = zval_toc<Param>(ZEND_THIS);

	cobj->construct(value);

}

ZEND_METHOD(Wcd_Sql_Param, getPartId)
{
	ZEND_PARSE_PARAMETERS_NONE();
	RETURN_LONG(SqlPartId::PARAM_PID);
}

ZEND_METHOD(Wcd_Sql_Param, getValue)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Param* cobj = zval_toc<Param>(ZEND_THIS);
	val_rc ret = cobj->getValue();

	ret.move_zv(return_value);
}



PHP_MINIT_FUNCTION(SqlIPart_reg)
{
	
	zclass_sql_ifipart = register_class_Wcd_Sql_IfSqlPart();

	zend_class_entry* ce;

	ce = register_class_Wcd_Sql_Literal(zclass_sql_ifipart);
	Literal::omg.classEntry(ce);

	ce = register_class_Wcd_Sql_Expr(zclass_sql_ifipart);
	Expr::omg.classEntry(ce);

	zclass_join_expr = register_class_Wcd_Sql_JoinExpr(zclass_sql_ifipart);
	JoinExpr::omg.classEntry(zclass_join_expr);

	zclass_table_attr = register_class_Wcd_Sql_TableAttr(zclass_sql_ifipart);
	TableAttr::omg.classEntry(zclass_table_attr);

	zclass_sql_icolumns = register_class_Wcd_Sql_IColumns(zclass_sql_ifipart);
	IColumns::omg.classEntry(zclass_sql_icolumns);

	zclass_sql_tcolumns = register_class_Wcd_Sql_TColumns(zclass_sql_icolumns, zclass_sql_ifipart);
	TColumns::omg.classEntry(zclass_sql_tcolumns);

	ce = register_class_Wcd_Sql_Param(zclass_sql_ifipart);
	Param::omg.classEntry(ce);

	RunSql::omg.classEntry(register_class_Wcd_Sql_RunSql());
	
	STATE_INIT_ADD(SQSTR)
	
	if (init_isql_module())
	{
		return SUCCESS;
	}
	else
	{
		return FAILURE;
	}
}


#endif
