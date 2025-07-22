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

#ifndef SQL_ARGINFO_H
#define SQL_ARGINFO_H
extern "C" {
	#include "stub/sqlipart_arginfo.h"
};
#endif

namespace wcd {

void Bindings::debug_info(htab_write di)
{
	di.set(SQSTR.data_key, data_);
	di.set(SQSTR.param_list, paramList_);
	di.set(SQSTR.isql, sql_);
	di.set(SQSTR.connect, db_);
}

void 
Bindings::addarray(int key, htab_read value)
{
	zval_mgr adapt(value);
	//showarray("Add Array", value);
	if ((key==ISql::SQL_FROM)||(key==ISql::SQL_JOIN))
	{
		set(key, zval_user(adapt));
	}
	else {
		addToArray(key, zval_user(adapt));
	}
	
}

void 
Bindings::addstr(int key, zstr_user tname)
{
	if ((key==ISql::SQL_FROM)||(key==ISql::SQL_JOIN))
	{
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
	else {
		zval_mgr temp(tname);
		addToArray(key, temp);
	}
}

void 
Bindings::add(int key, zval_user value)
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
Bindings::addToArray(int key, zval_user value)
{
	zval_user listown = data_.get(key);
	//zend_printf("bind key %d ", key);
	if (listown.isNull())
	{
		htab_mgr   list_mgr;
		htab_write list(list_mgr);

		list.push_back(value);

		htab_write mylist(data_);
		//showarray("new Array", list_mgr);

		mylist.set((zend_long)key, list_mgr);
		//showdata("data_", data_);
		
	}
	else { //? assert isArray() ?
		htab_write vlist(listown);
		vlist.push_back(value);
		//showmem("addToArray", listown);
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

zval*  
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

void 
Bindings::offset(int value)
{
	zval_user ldata = data_.get(ISql::SQL_LIMIT);
	htab_write hw(ldata);

	hw.set(SQSTR.offset, value);

	set(ISql::SQL_LIMIT, ldata);
}

void Bindings::set(int key, zval_user value)
{
	htab_write hw(data_);

	hw.set(key, value);

	//showarray("data_zval_user", data_);
}

void Bindings::set(int key, int value)
{
	htab_write hw(data_);
	zval_mgr wrap(value);
	hw.set((zend_long)key, wrap);
	//showarray("data_int", data_);
}

void Bindings::set(int key, const zval_mgr& value)
{
	htab_write hw(data_);
	hw.set((zend_long)key, value);
	//showarray("data_zval_mgr&", data_);
}

void Bindings::set(int key, htab_read value)
{
	htab_write hw(data_);
	hw.set((zend_long)key, value);
	//showarray("data_htab_read", data_);
	//showdata("setdata", data_);
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
		//showarray("after wipe", data_);
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
	zval_mgr args;
	htab_write wh(args);

	wh.set(SQSTR.column, column);

	if (value.isNull())
	{
		zval_mgr valop(opstr);
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

		auto kct = keys.size();
		if ((kct > 0) && (kct == values.size()))
		{
			htab_walk wk;
			auto k = wk.key();
			auto v = wk.value();
			zend_printf("kct = %d\n", kct);
		

			zval_user test = values.get(int(0));
			if (test.isNull())
			{
				for(wk.start(values); wk.ok(); wk.next())
				{
					showmem("key = ", k);
					showmem("val = ", v);
					where(k, SQSTR.cmp_equal, v, SQSTR.and_str);
				}
			}
			else {
				int ix = 0;
				for(wk.start(keys); wk.ok(); wk.next())
				{
					showmem("key = ", k);
					showmem("val = ", v);
					ix = k.zlong();
					test = values.get(ix);
					showmem("test", test);
					where(v, SQSTR.cmp_equal, test, SQSTR.and_str);
				}
			}
			return;
		}
	}
	zend_throw_error(zend_ce_error,"whereKeyValue parameters do not match");
}

zval_mgr 
Bindings::select()
{
	zobj_mgr from = getJoins();

	zval_user columns = get(ISql::NAME_LIST);

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

	zval_user fetch_z = get(ISql::FETCH_AS);

	int old_fetch = -1;
	int fetch_as = fetch_z.ok() ? fetch_z.zlong() : -1;

	IDriver* db = zobj_toc<IDriver>(db_);
	if (fetch_as >= 0)
	{
		old_fetch = db->setFetch(fetch_as);
	}

	ParamList* pobj = zobj_toc<ParamList>(plist);

	zval_mgr rows = RunSql::op(db_, pobj->getSql(), pobj->getValues(), true);


	if (fetch_as != old_fetch)
	{
		db->setFetch(old_fetch);
	}
	zstr_mgr mclass;

	zval_mgr model_mgr = get(ISql::MODEL_OBJ);

	zobj_user model = model_mgr.zobject();

	if (!model.ok())
	{

		mclass = get(ISql::MODEL_CLASS);

		if (mclass.isNull())
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
			if (!model.ok())
			{
				showstr("model class", mclass);
				model = ReflectCache::staticInstance(mclass);
			}
			showobj("static model obj ", model);
			Model* m = zobj_toc<Model>(model);
			return (zend_object*) m->newRow(hr.get((int)0), true);
		}
		else if (rct == 0)
		{
			//empty array
			rows.set_null();
			return rows;
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
	zval_mgr  eager_load_mgr = prop.property(SQSTR.eager_load);
	htab_read eager_load(eager_load_mgr);
	*/
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

void 
Bindings::orderBy(zstr_user cname, bool descend)
{
	zval_mgr args;

	htab_write hw(args);

	hw.set(SQSTR.column, cname);
	hw.set(SQSTR.descend, descend);

	add(ISql::SQL_ORDER, args);
}

void 
Bindings::update(zstr_user column, zval_user value)
{
	htab_mgr data_mgr;
	htab_write data(data_mgr);

	data.set(SQSTR.column, column);
	data.set(SQSTR.valuekey, value);

	addarray(ISql::SQL_UPDATE, data_mgr);
}

void 
Bindings::limit(int limit, int offset)
{
	htab_mgr data;

	htab_write hw(data);

	hw.set(SQSTR.limit, limit);
	if (offset)
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
	cobj->set(key, zval_user(value));

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
	ZEND_PARSE_PARAMETERS_NONE();

	Bindings* cobj = zval_toc<Bindings>(ZEND_THIS);
	zval_mgr result = cobj->select();

	result.move_zv(return_value);
}

#endif