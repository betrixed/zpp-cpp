#ifndef WCD_BINDINGS_CPP
#define WCD_BINDINGS_CPP

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

#ifndef BINDINGS_ARGINFO_H
#define BINDINGS_ARGINFO_H
extern "C" {
	#include "stub/bindings_arginfo.h"
};
#endif

namespace wcd {

void Bindings::debug_info(htab_rw di)
{
	di.set(SQSTR.data_key, data_);
	di.set(SQSTR.param_list, paramList_);
	di.set(SQSTR.isql, isql_);
	di.set(SQSTR.db_ref, dbref_);
	di.set(SQSTR.connect, db_);
}

obj_return 
Bindings::getDb()
{
	obj_return result;

	if (!db_.ok())
	{
		db_ = dbref_.get();
		if (!db_.ok()) {
			result.error() << "Weakref access fail";
		}
	}
	result.value_ = db_;
	return  result;
}


void 
Bindings::addarray(int key, htab_ptr value)
{
	val_rc adapt(value);
	//showarray("Add Array", value);
	if ((key==ISql::SQL_FROM)||(key==ISql::SQL_JOIN))
	{
		set(key, val_ptr(adapt));
	}
	else {
		addToArray(key, val_ptr(adapt));
	}
	
}

void 
Bindings::addstr(int key, str_ptr tname)
{
	if ((key==ISql::SQL_FROM)||(key==ISql::SQL_JOIN))
	{
		str_rc talias;

		str_rc  lcase(tname);
		lcase.lowercase();
		
		int pos = lcase.find(std::string_view(" as "),0);
		if (pos >= 0)
		{
			talias = tname.substr(pos+4);
			tname = tname.substr(0, pos);
		}

		obj_rc tc_obj = TColumns::omg.new_zobj();
		TColumns* tc = zobj_toc<TColumns>(tc_obj);

		val_rc colnames(SQSTR.asterisk);

		tc->construct(tname, talias, colnames);

		JoinTables* joint = getJoinTables();
		joint->setPrime(tc_obj);
	}
	else {
		val_rc temp(tname);
		addToArray(key, temp);
	}
}

void 
Bindings::add(int key, val_ptr value)
{
	if (value.isString())
	{
		addstr(key, value.zstr());
		return;
	}
	if ((key==ISql::SQL_FROM)||(key==ISql::SQL_JOIN))
	{
		set(key,value);
		return;
	}
	addToArray(key, value);
}

obj_rc
Bindings::primeJoin(val_ptr tcol)
{
	obj_rc jobj = getJoins();
	JoinTables* jt = zobj_toc<JoinTables>(jobj);
	jt->setPrime(tcol.zobject());
	return jobj;
}

void 
Bindings::construct(obj_ptr sql, obj_ptr connect)
{
	isql_ = sql;
	dbref_ = weak_ref::refObject(connect);
	//dbname_ = connect.property(SQSTR.namekey);
}


void 
Bindings::destruct()
{
	wipe();
	isql_.init();
}

JoinTables* 
Bindings::getJoinTables()
{
	obj_rc jobj = getJoins();
	return zobj_toc<JoinTables>(jobj);
}

obj_rc
Bindings::getJoins()
{
	val_ptr from = get(ISql::SQL_FROM);
	if (from.isObject())
	{
		return obj_rc(from.zobject());
	}

	obj_rc result = JoinTables::omg.new_zobj();
	
	htab_rw(data_).set((zend_long) ISql::SQL_FROM, result);
	return result;
}

obj_return
Bindings::getParamList()
{
	obj_return result;

	if (paramList_.ok())
	{
		result.value_ = paramList_;
		return result;
	}
	obj_return dbret = getDb();
	if (dbret.has_errors())
	{
		result = std::move(dbret);
		return result;
	}
	IDriver* db = zobj_toc<IDriver>(dbret.value_);

	paramList_ = db->newParamList();
	result.value_ = paramList_;
	return result;
}

bool
Bindings::getArray(int key, htab_rc& value)
{
	val_ptr result;
	if (data_.try_fetch(key, result))
	{
		if (result.isArray())
		{
			htab_rc array(result.zarray());
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
Bindings::addToArray(int key, val_ptr value)
{
	val_ptr listown = data_.get(key);
	//zend_printf("bind key %d ", key);
	if (listown.isNull())
	{
		htab_rc   list_mgr;
		htab_rw list(list_mgr);

		list.push_back(value);

		htab_rw mylist(data_);
		//showarray("new Array", list_mgr);

		mylist.set((zend_long)key, list_mgr);
		//showdata("data_", data_);
		
	}
	else { //? assert isArray() ?
		htab_rw vlist(listown);
		vlist.push_back(value);
		//showmem("addToArray", listown);
	}
	
}

bool_return
Bindings::addJoinData(htab_ptr data)
{
	bool_return result;

	result = false;
	str_rc atype = data.get(SQSTR.typekey);
	if (atype.isNull())
	{
		atype = SQSTR.basic;
	}
	// translated from older PHP scripts 
	// TODO: is this still needed?
	if (atype.vstr() == SQSTR.nested.vstr())
	{
		obj_rc prime;

		JoinTables* joins = getJoinTables();
		htab_ptr jia = joins->getData();

		size_t jcount = jia.size();
		if (jcount==0)
		{
			prime = joins->getPrime();
		}
		else {
			prime = jia[jcount-1]; // TODO: why not first?
		}
		str_ptr tname = data.get(SQSTR.table);

		obj_rc tcol_obj = TColumns::omg.new_zobj();
		TColumns* tcol = zobj_toc<TColumns>(tcol_obj);

		// TODO: name is own alias? - because alias used first.
		val_rc colnames(SQSTR.asterisk);
		tcol->construct(tname, tname, colnames); 

		str_ptr   jtype = data.get(SQSTR.jointype);
		int_return jenum = JoinInfo::getJoinType(jtype);
		if (jenum.has_errors())
		{
			result.value_ = false;
			goto RET_ALL;
		}

		obj_rc jiobj = JoinInfo::omg.new_zobj();
		JoinInfo* ji = zobj_toc<JoinInfo>(jiobj);

		val_rc temp(tcol_obj);
		ji->construct(temp, prime, jenum.value_);

		joins->addJoin(jiobj);

		htab_ptr nested = data.get(SQSTR.nested);
		if (nested.size())
		{
			htab_walk wk;
			val_rc lhs;
			val_rc rhs;

			auto jc = wk.value();
			for(wk.start(nested); wk.ok(); wk.next())
			{
				// must be array
				htab_ptr jah = jc.zarray();

				if (!jah.size())
				{	
					result.error() << "Nested join has no data";
					break;
				}

				str_rc c1str = jah.get(SQSTR.condition1);
				val_rc lhs = TableAttr::splitDot(c1str);

				str_rc c2str = jah.get(SQSTR.condition2);
				val_rc rhs = TableAttr::splitDot(c2str);

				str_rc opstr = jah.get(SQSTR.operator_key);
				int_return op = JoinExpr::toOperator(opstr);

				if (op.has_errors())
				{
					result = std::move(op);
					return result;
				}

				str_rc bstr = jah.get(SQSTR.boolean);
				int_return logic = JoinExpr::toLogic(bstr);	
				if (logic.has_errors()) {
					result = std::move(logic);
					return result;
				}
				ji->add(lhs, rhs, op.value_, logic.value_);			
			}
			if (!result.has_errors()) {
				result = true;
			}
			
		}
	}
RET_ALL:
	return result;
}

zval*  
Bindings::get(int key)
{
	return data_.get(key);
}

bool_return 
Bindings::aliasSelect()
{
	bool_return result;

	JoinTables* jt = getJoinTables();
	htab_ptr tables = jt->getData();


	if (tables.size())
	{
		htab_rc   cols_mgr;

		htab_rw all_cols(cols_mgr);

		htab_ptr all = jt->getTables();
		htab_walk wk;

		auto tcol = wk.value();
		for(wk.start(all); wk.ok(); wk.next())
		{
			htab_return aliased = this->columnAlias(tcol);
			if (aliased.has_errors())
			{
				result = std::move(aliased);
				return result;
			}
			all_cols.merge(aliased.value_);
		}
		if (all_cols.size())
		{
			val_rc temp(all_cols);
			this->set(ISql::SQL_RENAME, temp);
			return true;
		}
	}
	return false;
}

str_rc 
concat2(str_ptr s1, str_ptr s2)
{
	str_buf nbuf;
	nbuf << s1 << s2;
	return nbuf.zstr();
}

htab_return 
Bindings::columnAlias(obj_ptr tcolobj)
{
	htab_return result;

	str_buf buf;
	TColumns* tcol = zobj_toc<TColumns>(tcolobj);

	buf << '_' << tcol->getAlias() << '_';
	str_rc sfx = buf.zstr();

	htab_rc aliased_mgr;
	htab_rw aliased(aliased_mgr);

	if (tcol->has(SQSTR.asterisk))
	{
		tcol->unsetCol(SQSTR.asterisk);

		val_rc tname(tcol->getName());

		obj_return db = getDb();
		if (db.has_errors())
		{
			result = std::move(db);
			return result;
		}
		obj_rc model = db.value_.call(SQSTR.getTableModel, tname);

		htab_ptr columns = model.call(SQSTR.getColDefs);

		htab_walk wk;
		auto cname = wk.key();
		for(wk.start(columns); wk.ok(); wk.next())
		{
			str_ptr colname(cname.zstr());
			str_rc rename = concat2(colname, sfx);
			aliased.set(rename, colname);
			tcol->setColAlias(colname, rename);
		}
	}
	else {
		htab_ptr columns = tcol->getColNames();
		htab_walk wk;
		auto cname = wk.key();	
		auto alias = wk.value();
		for(wk.start(columns); wk.ok(); wk.next())
		{
			if (!alias.isString())
			{
				str_ptr colname(cname.zstr());
				str_rc rename = concat2(colname, sfx);
				aliased.set(rename, colname);
				tcol->setColAlias(colname, rename);
			}
		}
	}
	return aliased_mgr;
}

void 
Bindings::offset(int value)
{
	val_ptr ldata = data_.get(ISql::SQL_LIMIT);
	htab_rw hw(ldata);

	hw.set(SQSTR.offset, value);

	set(ISql::SQL_LIMIT, ldata);
}

void Bindings::set(int key, val_ptr value)
{
	htab_rw hw(data_);

	hw.set(key, value);

	//showarray("data_zval_user", data_);
}

void Bindings::set(int key, int value)
{
	htab_rw hw(data_);
	val_rc wrap(value);
	hw.set((zend_long)key, wrap);
	//showarray("data_int", data_);
}

void Bindings::set(int key, const val_rc& value)
{
	htab_rw hw(data_);
	hw.set((zend_long)key, value);
	//showarray("data_zval_mgr&", data_);
}

void Bindings::set(int key, htab_ptr value)
{
	htab_rw hw(data_);
	hw.set((zend_long)key, value);
	//showarray("data_htab_read", data_);
	//showdata("setdata", data_);
}


void Bindings::unset(int key)
{
	htab_rw(data_).unset(key);
}

void Bindings::wipe(int key)
{
	htab_rw hw(data_);

	if (key >= 0)
	{
		hw.unset(key);
	}
	else {

		hw.clear();
	}
	db_.init();
	paramList_.init();
}

str_rc alias_str_key(str_ptr malias)
{
	str_buf buf;

	buf << '_' << malias << '_';

	return buf.zstr();
}

void 
Bindings::where(val_ptr column, str_ptr opstr, val_ptr value, str_ptr blogic)
{
	val_rc args;
	htab_rw wh(args);

	wh.set(SQSTR.column, column);

	if (value.isNull())
	{
		val_rc valop(opstr);
		wh.set(SQSTR.value, valop);
		wh.set(SQSTR.operator_key, SQSTR.cmp_equal);
	}
	else {
		wh.set(SQSTR.value, value);
		wh.set(SQSTR.operator_key, opstr);
	}

	if (blogic.ok())
	{
		wh.set(SQSTR.boolean, blogic);
	}
	else {
		wh.set(SQSTR.boolean, SQSTR.op_and);
	}
	wh.set(SQSTR.typekey, SQSTR.basic);
	
	//showdata("where args", wh);
	add(ISql::SQL_WHERE, args);
}

error_return
Bindings::whereKeyValue(val_ptr key, val_ptr value)
{
	error_return result;

	if (key.isString() && !value.isArray())
	{
		where(key, SQSTR.cmp_equal, value, SQSTR.and_str);
	}
	if (key.isArray() && value.isArray())
	{
		htab_ptr keys(key.zarray());
		htab_ptr values(value.zarray());

		auto kct = keys.size();
		if ((kct > 0) && (kct == values.size()))
		{
			htab_walk wk;
			auto k = wk.key();
			auto v = wk.value();
			//zend_printf("kct = %d\n", kct);
		

			val_ptr test = values.get(int(0));
			if (test.isNull())
			{
				for(wk.start(values); wk.ok(); wk.next())
				{
					//showmem("key = ", k);
					//showmem("val = ", v);
					where(k, SQSTR.cmp_equal, v, SQSTR.and_str);
				}
			}
			else {
				int ix = 0;
				for(wk.start(keys); wk.ok(); wk.next())
				{
					//showmem("key = ", k);
					//showmem("val = ", v);
					ix = k.zlong();
					test = values.get(ix);
					//showmem("test", test);
					where(v, SQSTR.cmp_equal, test, SQSTR.and_str);
				}
			}
			return result;
		}
	}
	result.error() << "whereKeyValue parameters do not match";
	return result;
}

val_return 
Bindings::select()
{
	val_return result;

	obj_rc from = getJoins();

	val_ptr columns = get(ISql::NAME_LIST);

	if (columns.ok())
	{
		JoinTables* jt = zobj_toc<JoinTables>(from);
		obj_rc prime = jt->getPrime();
		IColumns* pc = zobj_toc<IColumns>(prime);
		pc->clear();
		pc->add(columns);
	}

	if (from.ok())
	{
		aliasSelect();
	}

	ISql* sql = zobj_toc<ISql>(isql_);

	obj_return plistret = sql->select(*this);
	if (plistret.has_errors())
	{
		result = std::move(plistret);
		return result;
	}
	ParamList* pobj = zobj_toc<ParamList>(plistret.value_);

	val_ptr fetch_z = get(ISql::FETCH_AS);

	int old_fetch = -1;
	int fetch_as = fetch_z.ok() ? fetch_z.zlong() : -1;

	obj_return dbret = getDb();
	if (dbret.has_errors())
	{
		result = std::move(dbret);
		return result;
	}

	IDriver& db = *zobj_toc<IDriver>(dbret.value_);
	if (fetch_as >= 0)
	{
		old_fetch = db.setFetch(fetch_as);
	}

	

	val_return rows_ret = RunSql::op(obj_ptr(db.vobj()), pobj->getSql(), pobj->getValues(), true);

	if (fetch_as != old_fetch)
	{
		db.setFetch(old_fetch);
	}

	

	if (rows_ret.has_errors())
	{
		result = std::move(rows_ret);
		return result;
	}
	val_rc& rows = rows_ret.value_;
	str_rc mclass;

	val_rc model_mgr = get(ISql::MODEL_OBJ);

	obj_ptr model = model_mgr.zobject();

	if (!model.ok())
	{
		mclass = get(ISql::MODEL_CLASS);

		if (mclass.isNull())
		{
			result.value_ = rows;
			return result;
		}
	}

	htab_ptr hr(val_ptr(rows).zarray());
	if (hr.ok())
	{
		size_t rct = hr.size();
		if (rct == 1)
		{
			if (!model.ok())
			{
				//showstr("model class", mclass);
				model = ReflectCache::staticInstance(mclass);
			}
			//showobj("static model obj ", model);
			Model* m = zobj_toc<Model>(model);
			result.value_ = (zend_object*) m->newRow(hr.get((int)0), true);
			return result;

		}
		else if (rct == 0)
		{
			//empty array
			result.value_.set_null();
			return result;
		}
	}
	//TODO: else what?
	// multiple rows case
	if (!mclass.ok())
	{
		mclass = model.className();
	}
	/*
	eager_load not implemented
	val_rc  eager_load_mgr = prop.property(SQSTR.eager_load);
	htab_ptr eager_load(eager_load_mgr);
	*/
	val_ptr rename = get(ISql::SQL_RENAME);

	if (rename.isArray())
	{
		htab_rc objset_mgr;
		htab_rw objset(objset_mgr);

		JoinTables* fromjt = getJoinTables();

		str_rc table_name = fromjt->getModel();

		htab_ptr tables = fromjt->getTables();

		obj_rc icols = fromjt->getTable(table_name);

		table_name = zobj_toc<IColumns>(icols)->getAlias();

		str_rc mb_id = alias_str_key(table_name);

		htab_rc alias_list_mgr;
		htab_rw alias_list(alias_list_mgr);

		htab_walk w1;

		auto alias_key = w1.key();
		for(w1.start(tables); w1.ok(); w1.next())
		{
			if (zs_cmp(table_name, alias_key.zstr()) != 0)
			{
				str_rc a_key = alias_str_key(alias_key);
				alias_list.push_back(a_key);
			}
		}

		auto r_row = w1.value();
		htab_ptr relist(rename.zarray());

		for(w1.start(hr); w1.ok(); w1.next())
		{
			obj_rc obj = JoinTables::rowSplit(r_row, relist);
			val_rc recset = obj.property(mb_id);
			htab_walk w2;
			auto ai_value = w2.value();
			// TODO: check recset ??
			htab_rw rec(recset);

			for(w2.start(alias_list); w2.ok(); w2.next())
			{
				str_rc key = ai_value.zstr();
				rec.set(key, obj.property(key));
			}
			objset.push_back(rec);
		}
		unset(ISql::SQL_RENAME);
		result.value_ = Model::createFromResult(mclass, objset_mgr);
		return result;
	}
	result.value_ = Model::createFromResult(mclass, hr);
	return result;
}

void 
Bindings::orderBy(val_ptr colspec, bool descend)
{
	val_rc args;

	htab_rw hw(args);

	if (colspec.isString())
	{
		val_rc ta_mgr = TableAttr::splitDot(colspec);

		hw.set(SQSTR.column, ta_mgr);
	}
	else 
	{
		hw.set(SQSTR.column, colspec);
	}
	
	val_rc boolmgr(descend); // otherwise taken as integer value
	hw.set(SQSTR.descend, boolmgr);

	add(ISql::SQL_ORDER, args);
}

void 
Bindings::update(str_ptr column, val_ptr value)
{
	htab_rc data_mgr;
	htab_rw data(data_mgr);

	data.set(SQSTR.column, column);
	data.set(SQSTR.valuekey, value);

	addarray(ISql::SQL_UPDATE, data_mgr);
}
void 
Bindings::limit(int lval, int offset)
{
	val_rc a1(lval);
	val_rc a2(offset);
	limit(a1,a2);
}

void 
Bindings::limit(val_ptr limit, val_ptr offset)
{
	htab_rc data;

	htab_rw hw(data);

	hw.set(SQSTR.limit, limit);
	if (offset.ok())
	{
		hw.set(SQSTR.offset, offset);
	}

	set(ISql::SQL_LIMIT, data);
}

} // namespace wcd

using namespace wcd;


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

/* public function aliasSelect(): bool {} */
ZEND_METHOD(Wcd_Sql_Bindings, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	cobj->destruct();
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
	zarg_rd args(execute_data);
	zval*     data = args.need(1);

	if (!args.throw_errors())
	{
		Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
		bool_return result = cobj->addJoinData(data);
		result.throw_errors();
		RETURN_BOOL(result.value_);
		result.throw_errors();
	}
}

/* public function aliasSelect(): bool {} */
ZEND_METHOD(Wcd_Sql_Bindings, aliasSelect)
{
	ZEND_PARSE_PARAMETERS_NONE();
	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	bool_return result = cobj->aliasSelect();
	result.throw_errors();
	RETURN_BOOL(result.value_);
}

/* public function get(int $key) : mixed {} */
ZEND_METHOD(Wcd_Sql_Bindings, get)
{
	zend_long key;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(key)
	ZEND_PARSE_PARAMETERS_END();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	val_ptr ret = cobj->get(key);
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
	htab_rc result;
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
	htab_ptr data = cobj->getData();
	data.return_zv(return_value);
}

/* public function getJoins(): JoinTables {} */
ZEND_METHOD(Wcd_Sql_Bindings, getJoins)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	obj_rc result = cobj->getJoins();

	result.move_zv(return_value);
}

/* public function getParamList() : ParamList {} */
ZEND_METHOD(Wcd_Sql_Bindings, getParamList)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	obj_return result = cobj->getParamList();
	result.throw_errors();
	result.value_.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Bindings, iSql)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	obj_ptr result = cobj->isql();

	result.return_zv(return_value);
}

ZEND_METHOD(Wcd_Sql_Bindings, orderBy)
{
	zarg_rd args(execute_data);

	val_ptr colspec( args.need(0));
	bool descend = false;
	

	args.zbool(descend, args.option(1));

	if (!args.throw_errors())
	{
		Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
		cobj->orderBy(colspec, descend);
	}


}

/* public function primeJoin(TColumns $tc): JoinTables {} */
ZEND_METHOD(Wcd_Sql_Bindings, primeJoin)
{
	zval* tcol;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(tcol, zclass_sql_tcolumns)
	ZEND_PARSE_PARAMETERS_END();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	obj_rc jt = cobj->primeJoin(tcol);
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
	cobj->set(key, val_ptr(value));

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

ZEND_METHOD(Wcd_Sql_Bindings, update)
{
	zend_string* column;
	zval*        value;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(column)
	Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	cobj->update(column, value);
}

/* public function wipe(int $ix = 0) : void {}*/
ZEND_METHOD(Wcd_Sql_Bindings, wipe)
{
	zend_long key = -1;
	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(key)
	ZEND_PARSE_PARAMETERS_END();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	cobj->wipe(key);
}

ZEND_METHOD(Wcd_Sql_Bindings, select)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	val_return result = cobj->select();
	result.throw_errors();
	result.value_.move_zv(return_value);
}

#endif