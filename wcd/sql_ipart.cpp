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
	ascend = "ascend";
	descend = "descend";

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

	getsql = "getsql";
	valuesdefault = "valuesdefault";
	eager_load = "eager_load";
	statement = "stmt";
	
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
Literal::construct(zval_user val)
{
	value_ = val;
}

zstr_mgr 
Literal::toString() const
{
	const char k_sqt = '\'';

	zval_user temp(value_);
	if (temp.isString())
	{
		zstr_buffer buf;
		buf << k_sqt << str_replace(StrView("'"), StrView("''"), temp) << k_sqt;
		return buf.zstr();
	}
	else {
		return temp.to_zstr();
	}
}

void Literal::debug_info(htab_write di)
{
	di.set(SQSTR.partid, LIT_PID);
	di.set(SQSTR.value, value_);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void Expr::debug_info(htab_write di)
{
	di.set(SQSTR.partid, EXPR_PID);
	di.set(SQSTR.value,  expr_);
}

void Expr::construct(zstr_user val)
{
	expr_ = val;
	//showstr("construct expr", expr_);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


void 
JoinExpr::debug_info(htab_write di)
{
	di.set(SQSTR.partid, JE_PID);
	di.set(SQSTR.lhs_val, lattr_);
	di.set(SQSTR.rhs_val, rattr_);
	di.set(SQSTR.val_op, op_);
	di.set(SQSTR.next_op, nextop_);
}

zstr_user //static
JoinExpr::opStr(int op)
{
	return SQSTR.opstr[op];
}

zstr_user //static
JoinExpr::boolStr(int nextop)
{
	return SQSTR.boolstr[nextop];
}

void 
JoinExpr::construct(zval_user leftval, zval_user rightval, int op, int nextop)
{
	lattr_ = leftval;
	rattr_ = rightval;
	op_ = op;
	nextop_ = nextop;
}

zstr_mgr 
JoinExpr::emit(int ix, Bindings* bind, 
	zstr_user Lalias, zstr_user Ralias)
{

	zstr_buffer sqlbuf;
	zstr_mgr    temp;

	const char blank = ' ';

	ISql* isql = zobj_toc<ISql>(bind->isql());

	// if not the first
	if ((ix > 0) && (nextop_ != B_NULL))
	{
		sqlbuf << blank << SQSTR.boolstr[nextop_];
	}

	zval_user L_attr(lattr_);
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
		sqlbuf << temp; 
	}

	if (op_ < OP_NOP)
	{
		sqlbuf << blank << SQSTR.opstr[op_];
	}

	zval_user R_attr(rattr_);

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
		sqlbuf << temp;
	}
	return sqlbuf.zstr();
}

int
JoinExpr::toLogic(zstr_user s)
{
	size_t opsize = s.size();

	if (!opsize)
	{
		return B_NULL;
	}
	const char* c = s.data();
	int c1 = toupper(c[0]);
	switch(c1)
	{
	case ' ':
		return B_NULL;
	case 'A':
		return B_AND;
	case 'O':
		return B_OR;
	default:
		break;
	}
	throw std::runtime_error("Invalid SQL logic operator");
}

int
JoinExpr::toOperator(zstr_user s)
{
	size_t opsize = s.size();

	if (!opsize)
	{
		return OP_EQ;
	}
	const char* c = s.data();

	int c1 = toupper(c[0]);
	int c2 = 0;
	zstr_mgr upw;
	zstr_user ps(s);

	switch(c1)
	{
	case '=':
		return OP_EQ;
	case '<':
		if (opsize==1)
		{
			return OP_LT;
		}	
		if (opsize==2)
		{
			c2 = toupper(c[1]);
			if (c2 == '>')
			{
				return OP_NEQ;
			}
			else if (c2 == '=')
			{
				return OP_LE;
			}
		}
		//error
		break;
	case '>':
		if (opsize==1)
		{
			return OP_GT;
		}
		if (opsize==2)
		{
			c2 = toupper(c[1]);
			if (c2 == '=')
			{
				return OP_GE;
			}
		}
		//error
		break;
	case 'L':
		upw = ps.to_upper();
		if (upw.vstr() == SQSTR.op_like.vstr())
			return OP_LIKE;
		//error
		break;
	case 'A':
		upw = ps.to_upper();
		if (upw.vstr() == SQSTR.op_and.vstr())
			return OP_AND;
		//error
		break;
	case 'O':
		upw = ps.to_upper();
		if (upw.vstr() == SQSTR.op_or.vstr())
			return OP_OR;
		//error
		break;
	case 'I':
		upw = ps.to_upper();
		if (upw.vstr() == SQSTR.is_null.vstr())
			return OP_ISNULL;
		if (upw.vstr() == SQSTR.is_not_null.vstr())
			return OP_NOTNULL;		
		//ERROR
	default:
		break;
	}
	throw std::runtime_error("Invalid SQL comparison operator");
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void
IColumns::debug_info(htab_write di)
{
	di.set(SQSTR.partid, ICOL_PID);
	di.set(SQSTR.alias, alias_);
	di.set(SQSTR.columns, colnames_);
	di.set(SQSTR.colexpr, expr_);
	di.set(SQSTR.owner, owner_);
}	

void 
IColumns::construct(zval_user owner)
{
	owner_ = owner;
}

void IColumns::clear()
{
	colnames_.reset();
	expr_.reset();
	alias_.init();
}
		
void 
IColumns::add(zval_user columns)		
{
	if (columns.isArray())
	{
		htab_walk wk;

		zval_mgr vFalse;

		vFalse.set_bool(false);

		auto ix = wk.key();
		auto name = wk.value();
		htab_write names(colnames_);

		for(wk.start(columns.zarray()); wk.ok(); wk.next())
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
IColumns::setColAlias(zstr_user name, zstr_user alias)
{
	zval_mgr  val;

	if (alias.isNull())
	{
		val.set_bool(false);
	}
	else {
		val = alias;
	}
	htab_write hw(colnames_);
	hw.set(name, val);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

zobj_mgr
TableAttr::makeTA(zstr_user t, zstr_user a)
{
	zobj_mgr result = TableAttr::omg.new_zobj();
	TableAttr* ta = zobj_toc<TableAttr>(result);
	ta->construct(t, a);
	return result;
}

zval_mgr
TableAttr::splitDot(zstr_user s)
{
	int pos = s.find('.');
	zval_mgr result;

	if (pos > 0)
	{
		zstr_mgr  tname = s.substr(0,pos);
		zstr_mgr  tattr = s.substr(pos+1);
		result = makeTA(tname, tattr);
	}
	else {
		result = s;
	}
	return result;
}

void 
TableAttr::construct(zstr_user t, zstr_user a)
{
	table_ = t;
	attr_ = a;
}

void TableAttr::debug_info(htab_write di)
{
	di.set(SQSTR.partid, TA_PID);
	di.set(SQSTR.table, table_);
	di.set(SQSTR.attr, attr_);

}

zstr_mgr 
TableAttr::toString() const
{
	zstr_buffer buf;

	buf << table_ << '.' << attr_;
	return buf.zstr();
}


//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void // virtual
TColumns::debug_info(htab_write di)
{
	IColumns::debug_info(di);

	di.set(SQSTR.partid, TCOL_PID);
	di.set(SQSTR.namekey, name_);
	di.set(SQSTR.attr, attr_map_);
}	

void 
TColumns::construct(zstr_user tname,  zstr_user talias,  zval_user tcol)
{
	zval_mgr null_owner;

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
		zstr_user falseval;

		this->setColAlias(tcol.zstr(), falseval);
	}
}

zval_mgr
TColumns::attr(zstr_user name)
{
	zval_mgr result;
	zval_user test;

	if (attr_map_.try_fetch(name, test))
	{
		result = test;
		return result;
	}

	zstr_user table(this->alias_);
	if (table.isNull())
	{
		table = this->name_;
	}

	zobj_mgr ta = TableAttr::makeTA(table, name);

	result = ta;
	htab_write hw(attr_map_);

	hw.set(name, result);

	return result;
}

void 
TColumns::setName(zstr_user name)
{
	this->name_ = name;
}

zval_mgr //static
TColumns::tableCol(zstr_user expr)
{
	

	zobj_mgr  zobj = omg.new_zobj();
	TColumns* cobj = zobj_toc<TColumns>(zobj);

	zstr_user nullstr;
	zval_user tcol;

	zval_mgr  pair_z = explode(SQSTR.dot_char, expr);
	zval_user pair(pair_z);

	if (pair.isArray())
	{
		htab_read hpair(pair.zarray());
		zstr_user name = hpair.get(int(0));
		
		tcol = hpair.get(int(1));

		cobj->construct(name, nullstr, tcol);
	}
	else {
		cobj->construct(expr, nullstr, tcol);
	}
	return zval_mgr(zobj);
	//zval_own explode(zstr_user sep,  zstr_user split, long limit = 0);
}

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void 
Param::debug_info(htab_write di)
{
	di.set(SQSTR.valuekey, value_);
}

void 
Param::construct(zval_user zp)
{
	value_ = zp;
}


}; //namespace wcd

using namespace wcd;

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
	zval_user value = cobj->getValue();
	value.return_zv(return_value);
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
	zstr_mgr result = cobj->toString();

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
	zstr_user s = cobj->toString();
	s.return_zv(return_value);	
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
	
	zstr_mgr s = cobj->emit(ix, bind, lalias, ralias);
	s.move_zv(return_value);

}

ZEND_METHOD(Wcd_Sql_JoinExpr, opstr)
{
	zend_long ix;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(ix)
	ZEND_PARSE_PARAMETERS_END();

	zstr_user s = JoinExpr::opStr(ix);
	s.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_JoinExpr, boolstr)
{
	zend_long ix;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(ix)
	ZEND_PARSE_PARAMETERS_END();

	zstr_user s = JoinExpr::boolStr(ix);
	s.return_zv(return_value);
}

//@@@@@@@@@@@@@ 

ZEND_METHOD(Wcd_Sql_IColumns, __construct)
{
	zval* owner;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OR_NULL(owner)
	ZEND_PARSE_PARAMETERS_END();


	IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
	cobj->construct(owner);
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
	zobj_user obj = cobj->getOwner();
	obj.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_IColumns, getColNames)
{
	ZEND_PARSE_PARAMETERS_NONE();

	IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
	htab_read array = cobj->getColNames();
	array.return_zv(return_value);

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
	zstr_user str = cobj->getAlias();
	str.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_IColumns, getName)
{
	ZEND_PARSE_PARAMETERS_NONE();
	IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
	zstr_user str = cobj->getName();
	str.return_zv(return_value);
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

ZEND_METHOD(Wcd_Sql_IColumns, add)
{
	zval* more;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(more)
	ZEND_PARSE_PARAMETERS_END();
	IColumns* cobj = zval_toc<IColumns> (ZEND_THIS);
	cobj->add(more);
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
	htab_read array = cobj->getExpr();
	array.return_zv(return_value);
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

	zstr_mgr s = cobj->toString();
	s.move_zv(return_value);

}

ZEND_METHOD(Wcd_Sql_TableAttr, getTable)
{
	ZEND_PARSE_PARAMETERS_NONE();
	TableAttr* cobj = zval_toc<TableAttr> (ZEND_THIS);
	zstr_user s = cobj->getTable();
	s.return_zv(return_value);

}

ZEND_METHOD(Wcd_Sql_TableAttr, getAttr)
{
	ZEND_PARSE_PARAMETERS_NONE();
	TableAttr* cobj = zval_toc<TableAttr> (ZEND_THIS);
	zstr_user s = cobj->getAttr();
	s.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_TableAttr, splitDot)
{
	zend_string* attr;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(attr)
	ZEND_PARSE_PARAMETERS_END();

	zval_mgr result = TableAttr::splitDot(attr);
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

	zval_mgr tempval;
	if (tcol)
	{
		tempval = tcol;
	}
	else if (tstr)
	{
		tempval = zstr_user(tstr);
	}

	cobj->construct(tname,alias,tempval);

}

ZEND_METHOD(Wcd_Sql_TColumns, attr)
{
	zend_string* value;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(value)
	ZEND_PARSE_PARAMETERS_END();

	TColumns* cobj = zval_toc<TColumns> (ZEND_THIS);
	zobj_mgr obj = cobj->attr(value);
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
	zstr_user s = cobj->getName();
	s.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_TColumns, tableCol)
{
	zend_string* expr;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(expr)
	ZEND_PARSE_PARAMETERS_END();

	zval_mgr obj = TColumns::tableCol(expr);

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
	zval_mgr ret = cobj->getValue();

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
