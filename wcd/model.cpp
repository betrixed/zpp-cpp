#ifndef WCD_MODEL_CPP
#define WCD_MODEL_CPP

#ifndef WCD_MODEL_H
#include "model.h"
#endif

#ifndef WCD_IDRIVER_H
#include "idriver.h"
#endif

#ifndef WCD_ISERVER_H
#include "iserver.h"
#endif

#ifndef WCD_IBUILD_H
#include "ibuild.h"
#endif

#ifndef SQL_ISQL_H
#include "sql_isql.h"
#endif

#ifndef WCD_IROW_H
#include "irow.h"
#endif

#ifndef MODEL_ARGINFO_H
#define MODEL_ARGINFO_H
extern "C" {
     #include "stub/model_arginfo.h"
}
#endif

#include <filesystem>

namespace wcd {
	using namespace zpp;

	base_obj_mgr<Model> Model::omg;
	
	zend_class_entry* zintf_ce_IfCrud;

	Model_init MIS;

	void Model_init::init() {
			find_first = "findfirst";
			find_all = "findall";
			by_str = "by";

			eq_str = "=";
			u_model = "_model";
			r_arg = "r";
			escape_key = "escape";
			escape_str = "\\";
			get_tables = "gettables";
			columns_str = "columns";
			m_updated_at = "updatedatname";
			m_created_at = "createdatname";
			m_datetime_type = "getdatetimetype";
			type_str = "type";
			name_str = "name";
			returns_key = "_ret";

			get_primary_key = "getprimarykey";
			fn_getseqcols = "getseqcols";
			k_created_at = "created_at";
			k_updated_at = "updated_at";

			k_pkey_options = "pkey_options";
			k_seq_defs = "seq_defs";
			k_col_defs = "col_defs";
			k_pkey = "pkey";
			k_tdef = "table_def";
			k_buildme = "build_me";
			k_driver = "driver";

			k_table = "table";
			k_field = "field";
		}
	Model::Model() : base_d()
	{
		timestamps_ = NO_TS;
	}

	void 
	Model::destruct()
	{
		dbname_.init();
		builder_.init();
		builder_me_.init();
	}
	val_rc 
	Model::createFromResult(str_ptr classname, htab_ptr results)
	{
		if (results.size()==0)
		{
			return htab_rc::empty_array();
		}

		Model* m = model_instance(classname);

		val_rc rmgr;
		htab_rw r(rmgr);

		htab_walk wk;

		auto result = wk.value();


		for(wk.start(results); wk.ok(); wk.next())
		{
			r.push_back(m->newRow(result.zarray(), true));
		}

		return rmgr;
	}

	// Make sure a call to construct will succeed
	void
	Model::construct()
	{
	}

	void
	Model::debug_info(htab_rw di)
	{

		base_d::debug_info(di);
		di.set(MIS.k_driver, dbref_);

		di.set(MIS.k_pkey_options, pkey_options_);
		di.set(MIS.k_col_defs, class_cdefs_);
		di.set(MIS.k_pkey, class_pkey_);
		di.set(MIS.k_tdef, class_tdef_);
		di.set(MIS.k_buildme, builder_me_);



	}

	bool_return 
	Model::deleteRow(obj_ptr rowobj)
	{
		bool_return result;

		obj_return ibret = getBuilderForMe();
		if (ibret.has_errors())
		{
			result = ibret.move_error();
			return result;
		}
		IBuild* ib = zobj_toc<IBuild>(ibret.value_);

		val_return temp = ib->deleteRow(rowobj);
		result = temp.value_.zbool();
		if (temp.has_errors())
		{
			result = std::move(temp);
			result = false;
		}
		return result;
	}

	int Model::getTSFlags() const
	{
		return timestamps_;
	}

	str_rc 
	Model::now() const
	{
		val_rc null_ts;
		return datetime_obj::date(DTData.now_format, null_ts);
	}

	obj_rc
	Model::newRow(htab_ptr data, bool isSaved)
	{
		obj_rc result = IRow::omg.new_zobj();

		IRow* irow = zobj_toc<IRow>(result);

		irow->construct(vobj(), data, isSaved);

		return result;
	}

	obj_rc//static
	Model::row(str_ptr static_name, htab_ptr data)
	{
		Model* m = model_instance(static_name);
		return m->newRow(data);
	}

	obj_rc//static
	Model::rowSaved(str_ptr static_name, htab_ptr data)
	{
		Model* m = model_instance(static_name);
		obj_rc rec = m->newRow(data);
		IRow* irow = zobj_toc<IRow>(rec);
		irow->create();
		return rec;
	}

	obj_return
	Model::getConnect()
	{
		obj_return result;

		if (dbref_.ok())
		{
			result = dbref_.get();

			//showobj("getConnect", result.value_);
			return result;
		}

		// get the default connection
		wref_return wref = IServer::connect(str_ptr::empty_str());

		if (!wref.has_errors())
		{
			setConnect(wref.value_);
			result.value_ = dbref_.get();
		}
		else {
			result = wref.move_error();
		}
		return result;
	}

	str_rc 
	Model::createdAtName()
	{
		return MIS.k_created_at;
	}

	str_rc 
	Model::updatedAtName()
	{
		return MIS.k_updated_at;
	}

	obj_return
	Model::getBuilder()
	{
		obj_return result;

		if (!builder_.ok())
		{
			obj_return db_mgr = getConnect();
			if (db_mgr.has_errors())
			{
				result = std::move(db_mgr);
				return result;
			}
			IDriver* db = zobj_toc<IDriver>(db_mgr.value_);
			builder_ = db->newDmlBuild();
		}
		result.value_ = builder_;
		return result; 
	}

	obj_return
	Model::getBuilderForMe()
	{
		obj_return result;

		if (!builder_me_.ok())
		{
			obj_return db_mgr = getConnect();
			if (db_mgr.has_errors())
			{
				result = std::move(db_mgr);
				return result;
			}

			IDriver* db = zobj_toc<IDriver>(db_mgr.value_);
			builder_me_ = db->newDmlBuild();
		}
		if (builder_me_.ok())
		{
			IBuild* ib = zobj_toc<IBuild>(builder_me_);
			ib->setModel(obj_ptr(this->vobj()));
			result.value_ = builder_me_;
		}
		return result;
	}

	obj_return
	Model::modelBuild(str_ptr classname)
	{
		Model* m = model_instance(classname);
		return m->getBuilderForMe();
	}

	obj_return 
	Model::byKeyValue(val_ptr keynames, val_ptr values)
	{
		error_return check;
		obj_return   builder;
		obj_return   result;

		builder = getBuilderForMe();
		if (builder.has_errors())
		{
			result = builder.move_error();
			return result;
		}

		IBuild* ib = zobj_toc<IBuild>(builder.value_);

		Bindings&  bind = ib->bindings();
		bind.limit(1);

		check = bind.whereKeyValue(keynames, values);
		if (check.has_errors())
		{
			result = check.move_error();
			return result;
		}

		val_return row = bind.select();
		if (row.has_errors())
		{
			result = row.move_error();
			return result;
		}
		result.value_ = row.value_.zobject();
		return result;	
	}

	Model* //static
	Model::model_instance(str_ptr classname)
	{
		obj_ptr model = Services::getOne(classname);
		Model* m = zobj_toc<Model>(model);

		return m;
	}

	obj_return //static
	Model::keyValue(str_ptr static_class, val_ptr keynames, val_ptr values)
	{
		
		Model* m = Model::model_instance(static_class);
		return m->byKeyValue(keynames, values);
	}

	obj_return 
	Model::withValues(str_ptr static_class, val_ptr keyvalues)
	{
		Model* m = Model::model_instance(static_class);

		htab_rc kv(keyvalues.zarray());

		htab_rc keynames = htab_rc::getKeys(kv);
		htab_rc values = htab_rc::getValues(kv);
		val_rc keynames_mgr(keynames);
		val_rc values_mgr(values);
		return m->byKeyValue(keynames_mgr, values_mgr);
	}

	val_return 
	Model::callStatic(str_ptr static_name, str_ptr method, val_ptr params)
	{
		val_return result;

		htab_ptr  parray(params.zarray());

		Model* m = Model::model_instance(static_name);
		obj_return buildret = m->getBuilderForMe();
		if (buildret.has_errors())
		{
			result = std::move(buildret);
			return result;
		}
		obj_rc& build = buildret.value_;

		IBuild* ib = zobj_toc<IBuild>(build);

		str_rc mlower(method);
		mlower.lowercase();
		
		str_ptr nullstr;
		val_rc  nullval;
		if (mlower.starts_with(MIS.find_first)) 
		{
			if (mlower.size()==9) 
			{
				ib->where(params, nullstr, nullval, SQSTR.and_str);
				return ib->oneRow();
			}
			else {
				str_rc bystr = mlower.substr(9);
				if (bystr.starts_with(MIS.by_str))
				{
					str_rc column = bystr.substr(2);

					val_rc value = parray.get((int)0);
					val_rc col_mgr(column);
					ib->where(col_mgr, MIS.eq_str, value, nullstr);
					return ib->oneRow();

				}
			}
		}
		else if (zs_cmp(mlower, MIS.find_all)==0)
		{
			if (parray.size())
			{
				ib->where(params, nullstr, nullval, SQSTR.and_str);
			}
			result = ib->allRows();
			if (result.has_errors())
			{
				return result;
			}
			val_rc& val = result.value_;
			if (val.isObject())
			{
				htab_rc rows_mgr;
				htab_rw rows(rows_mgr);

				rows.push_back(val);
				result.value_ = rows_mgr;
			}
			return result;
		}
		val_rc obj_method;
		htab_rw arg1(obj_method);

		arg1.push_back(build);
		arg1.push_back(mlower);
		
		result.value_ = call_user_func_array(obj_method, params);
		return result;
	}

	str_rc //static
	Model::getTableName(str_ptr cname)
	{
		return Model::classToTableName(cname);
	}

	str_rc 
	Model::classToTableName(str_ptr class_name)
	{
		str_rc result(class_name);
		int ix = result.rfind('\\');

		if (ix >= 0) {
			result = result.substr(ix+1);
		}

		result = result.uncamel();

		str_rc rlower = result.to_lower();

		ix = rlower.strpos(MIS.u_model);

		if (ix >= 0)
		{
			result = result.substr(0, ix);
		}
		return result;
	}

	obj_return //static 
	Model::find(str_ptr static_name, val_ptr id)
	{
		obj_return result;

		Model* m = model_instance(static_name);

		htab_return pkey_ret = m->getPKey();

		if (pkey_ret.has_errors())
		{
			result = std::move(pkey_ret);
			return result;
		}
		htab_rc& pkey = pkey_ret.value_;

		val_rc pkey_mgr(pkey);

		if (!pkey.size())
		{
			result.error() << "No Primary key columns: " << static_name;
			return result;
		}
		else if (id.isArray())
		{
			htab_ptr vlist(id.zarray());
			val_ptr test = vlist.get((int)0);
			if (test.isNull()) {
				htab_rc v2 = htab_rc::sublist(pkey, vlist);
				val_rc arg2(v2);
				result = m->byKeyValue(pkey_mgr, arg2);
			}
			else {
				result = m->byKeyValue(pkey_mgr,id);
			}
		}
		else {
			val_rc vlist_tab;
			htab_rw vlist(vlist_tab);
			vlist.push_back(id);
			result = m->byKeyValue(pkey_mgr, vlist_tab);
		}
		return result;
	}

	htab_return 
	Model::getKeyOptions()
	{
		htab_return result;

		if (pkey_options_.ok())
		{
			//showdata("options set", result);
			result.value_ = pkey_options_;
			return result;
		}
		
		htab_return pkey_fields_ret = getPKey();
		if (pkey_fields_ret.has_errors())
		{
			result = std::move(pkey_fields_ret);
			return result;
		}
		htab_rc& pkey_fields = pkey_fields_ret.value_;
		//showdata("pkey get", pkey_fields);
		if (pkey_fields.size())
		{
			htab_rw pkey_options(pkey_options_);

			htab_return cdefs_ret = getColDefs();
			if (cdefs_ret.has_errors())
			{
				result = std::move(cdefs_ret);
				return result;
			}
			htab_rc& cdefs = cdefs_ret.value_;

			htab_return seqdef_ret = getSeqDefs();
			if (seqdef_ret.has_errors())
			{
				result = std::move(seqdef_ret);
				return result;
			}
			htab_rc& seqdefs = seqdef_ret.value_;

			htab_walk wk;
			auto pkey = wk.value();


			for(wk.start(pkey_fields); wk.ok(); wk.next())
			{
				//zend_printf("get pkey options ");
				//showmem("pkey", pkey);
				htab_rc options_mgr;
				htab_rw options(options_mgr);

				str_rc key_name = pkey.zstr();
				key_name.lowercase();

				val_ptr key_def = cdefs.get(key_name);
				obj_rc  pkeydef = key_def.zobject();

				val_rc seq_pkey = pkeydef.property(SQSTR.id_key);

				if (seq_pkey.isString())
				{
					options.set(SQSTR.seq_key, seq_pkey);
					options.set(SQSTR.bind_key, SQSTR.return_str);
					options.set(SQSTR.returns_str, Crud::ID_GEN);
				}
				else if (seqdefs.size()) 
				{
					seq_pkey = seqdefs.get(key_name);

					if (seq_pkey.isString())
					{
						options.set(SQSTR.seq_key, seq_pkey);
						val_rc defval = pkeydef.property(SQSTR.default_key);
						if (defval.isString())
						{
							htab_rc temp_mgr;
							htab_rw temp(temp_mgr);

							temp.push_back(SQSTR.default_key);
							temp.push_back(defval);
							options.set(SQSTR.bind_key, temp_mgr);
						}
						options.set(SQSTR.returns_str, Crud::LAST_SEQ);
					}
				}
				else 
				{
					val_rc isAutoInc = pkeydef.property(SQSTR.auto_inc);
					if (isAutoInc.ok()) 
					{
						options.set(SQSTR.returns_str, Crud::LAST_ID);
					}
				}
				if (options.size()==0)
				{
					options.set(SQSTR.returns_str, Crud::ID_SET);
				}
				pkey_options.set(key_name, options_mgr);
			}
			result.value_ = pkey_options;
		}
		return result;
	}

	htab_return Model::getColDefs()
	{
		htab_return result;

		if (class_cdefs_.ok())
		{
			result.value_ = class_cdefs_;
			return result;
		}

		obj_return tabledef_mgr = getTableDef();

		if (tabledef_mgr.has_errors())
		{
			result = tabledef_mgr.move_error();
			return result;
		}

		class_cdefs_ = tabledef_mgr.value_.property(MIS.columns_str);
		result.value_ = class_cdefs_;

		//showdata("getColDefs", class_cdefs_);
		return result;
	}

	str_rc Model::getName()
	{
		obj_ptr self(vobj());

		str_rc result = self.property(MIS.name_str);

		if (result.ok())
		{
			return result;
		}

		result = Model::classToTableName(self.className());
		self.property(MIS.name_str, result);
		return result;
	}

	htab_return 
	Model::getPKey()
	{
		//zend_printf("in getPKey()\n");
		htab_return result;
		if (class_pkey_.ok())
		{
			result.value_ = class_pkey_;
			//showdata("class pkey", result);
			return result;
		}
		obj_return tdef_ret = getTableDef();
		if (tdef_ret.has_errors())
		{	
			result = std::move(tdef_ret);
			return result;
		}
		obj_rc& tdef = tdef_ret.value_;

		obj_rc pkeydef = tdef.call(MIS.get_primary_key);
			//showobj("pkeydef:", pkeydef);

		if (pkeydef.ok())
		{
			class_pkey_ = pkeydef.property(MIS.columns_str);
			result.value_ = class_pkey_;
			return result;
		}

		class_pkey_ = htab_rc::empty_array();
		result.value_ = class_pkey_;
		return result;
	}

	int_return // static
	Model::importFromCSV(str_ptr static_name, str_ptr filename)
	{
		if (! std::filesystem::exists(filename.vstr()) )
		{
			return 0;
		}
		int_return result;

		int datarowct = -1;

		Model* m = model_instance(static_name);

		str_rc tableName = m->getName();

		obj_return dbret = m->getConnect();
		if (dbret.has_errors())
		{
			result = std::move(dbret);
			return result;
		}
		obj_rc& db = dbret.value_;

		IDriver* driver = zobj_toc<IDriver>(db);

		htab_return columns_ret = m->getColDefs();
		if (columns_ret.has_errors())
		{
			result = columns_ret.move_error();
			return result;
		}

		htab_rc& columns = columns_ret.value_;
		//showdata("columns", columns);

		htab_rc fieldNames;

		bool init = false;
		unsigned int  colcount = 0;

		obj_return buildret = m->getBuilderForMe();
		if (buildret.has_errors())
		{
			result = std::move(buildret);
			return result;
		}
		obj_rc& builder = buildret.value_;

		val_rc import_mgr = fopen(filename, MIS.r_arg);

		val_ptr import(import_mgr);

		if (import.isResource())
		{
			htab_rc csv_args;
			htab_rw csv(csv_args);
			csv.set(MIS.escape_key, MIS.escape_str);

			obj_rc stmt;

			while(true)
			{
				val_rc line_mgr = fgetcsv(import,csv_args);
				val_ptr line(line_mgr); 
				str_rc  cellstr;

				if (line.isFalse())
				{
					break;
				}

				htab_rc values_mgr;
				htab_rw values(values_mgr);

				htab_walk sw;

				auto item = sw.value();
				auto ix = sw.key();

				for(sw.start(line.zarray()); sw.ok(); sw.next())
				{
					cellstr = stripslashes(item.zstr());
					values.push_back(cellstr);
				}
				//showdata("line values", values);
				if (values.size() == 0)
				{
					//zend_printf("BREAK\n");
					break;
				}
				if (!init)
				{
					driver->begin();
					auto values_count = values.size();
					if (values_count) {
						colcount = 0;
						htab_walk wk;
						auto vname = wk.value();
						for(wk.start(values); wk.ok(); wk.next())
						{
							val_ptr col = columns.get(vname);
							if (!col.isNull()) {
								colcount += 1;
							}
						}
						if (colcount == values_count) {
							fieldNames = values;
						}
					}
					if (!fieldNames.size())
					{
						fieldNames = columns.slice(0, values_count);
						datarowct = 0;
						//showdata("fieldnames", fieldNames);
					}
					
					init = true;
				}
				if (datarowct == 0) {
					//  prepaire for multi inserts
					htab_rc columns_mgr;
					htab_rw columns(columns_mgr);

					for(sw.start(values); sw.ok(); sw.next())
					{
						str_rc colname = fieldNames.get(ix);
						columns.set(colname, item);
					}
					IBuild* ib = zobj_toc<IBuild>(builder);

					obj_return plist_ret = ib->getInsertSql(columns_mgr);
					if (plist_ret.has_errors())
					{
						result = std::move(plist_ret);
						return result;
					}

					ParamList* plist = zobj_toc<ParamList>(plist_ret.value_);

					str_ptr sql(plist->getSql());
					htab_ptr record(plist->getValues());

					//showstr("sql", sql);
					//showdata("record", record);

					obj_return stmt_ret = driver->prepare(sql);
					if (stmt_ret.has_errors())
					{
						result = std::move(stmt_ret);
						return result;
					}

					stmt = stmt_ret.value_;

					error_return msgerr = driver->bind(stmt, record);
					if (msgerr.has_errors())
					{
						result = msgerr.move_error();
						return result;
					}
					driver->execute(stmt, false, false);
				}
				else if (datarowct > 0) {
					driver->bind(stmt, values);
					driver->execute(stmt, false, false);
				}
				
				datarowct += 1;
				//showmem("next step", stmt);
			}
			//zend_printf("row ct %d ", datarowct);
			//showmem("loop end", stmt);

			driver->closeStmt(stmt);

			fclose(import);

			htab_return rseq_defs = m->getSeqDefs();
			if (rseq_defs.has_errors())
			{
				result = std::move(rseq_defs);
				return result;
			}

			htab_rc& seq_defs = rseq_defs.value_;

			if (seq_defs.size())
			{
				error_return test = m->sequenceMax();
				if (test.has_errors())
				{
					result = test.move_error();
					return result;
				}
			}

			driver->commit();
			//zend_printf("ROWS %d\n", datarowct);

		}
		result.value_ = datarowct+1;
		return result;
	}

	error_return 
	Model::sequenceMax()
	{
		error_return result;

		htab_return sdef_ret = getSeqDefs();
		if (sdef_ret.has_errors())
		{
			result = sdef_ret.move_error();
			return result;
		}
		obj_return builder = getBuilder();
		if (builder.has_errors())
		{
			result = builder.move_error();
			return result;
		}
		str_rc tname = getName();
		IBuild* bd = zobj_toc<IBuild>(builder_);
		bd->table(tname, true);
		htab_walk wk;
		auto skey = wk.key(); // val_ptr
		auto sname = wk.value();

		str_rc max_s("MAX");
		for(wk.start(sdef_ret.value_); wk.ok(); wk.next())
		{
			str_ptr seqfield = skey.zstr();
			htab_rc hcolumn;
			htab_rw hcol(hcolumn);
			hcol.push_back(seqfield);

			val_return max = bd->aggregate(max_s,hcolumn);
			if (max.has_errors())
			{
				result = max.move_error();
				return result;
			}
			val_return sval  = bd->seqLastValue(sname.zstr());
			if (sval.has_errors())
			{
				result = sval.move_error();
				return result;
			}

			long maxseqval = max.value_.zlong();
			long actualval = sval.value_.zlong();
			if (maxseqval > actualval)
			{
				htab_return options = getKeyOptions();
				if (options.has_errors())
				{
					result = options.move_error();
					return result;
				}
				htab_rc data = options.value_.get(seqfield);
				htab_rw seqdata(data);
				
				seqdata.set(MIS.k_field, seqfield);
				seqdata.set(MIS.k_table, tname);
				bd->setSeqValue(maxseqval, data);
			}
		}
		return result;
	}

	obj_return 
	Model::getTableDef()
	{
		obj_return result;

		if (class_tdef_.ok())
		{
			result.value_ = class_tdef_;
			return result;
		}

		str_rc name = getName();
		//showstr("name", name);

		obj_return db_ret = getConnect();
		if (db_ret.has_errors())
		{
			result = db_ret.move_error();
			return result;
		}

		//showobj("db_ret", db_ret.value_);

		IDriver* db = zobj_toc<IDriver>(db_ret.value_);

		obj_rc schema = db->getSchema();

		htab_rc tables = schema.call(MIS.get_tables);
		//showdata("tables from schema", tables);

		 

		obj_rc test = tables.get(name);

		if (!test.ok())
		{
			result.error() << "No columns metadata for table " << name;
			return result;
		}

		class_tdef_ = test;

		//showobj("TDEF", class_tdef_);class_tdef_
		htab_rc columns = class_tdef_.property(MIS.columns_str);

		//showdata("columns", columns);

		htab_rc tsf_mgr;
		htab_rw tsf(tsf_mgr);

		obj_rc self = vobj();

		str_rc ts_update = self.call(MIS.m_updated_at);
		str_rc ts_create = self.call(MIS.m_created_at);

		if (ts_update.size()) {
			tsf.set(ts_update, int(1));
		}
		if (ts_create.size()) {
			tsf.set(ts_create, int(2));
		}

		str_rc stamp_type = class_tdef_.call(MIS.m_datetime_type);
		timestamps_ = 0;

		if (stamp_type.ok())
		{
			htab_walk wk;
			auto cdef_mgr = wk.value();
			for(wk.start(columns); wk.ok(); wk.next())
			{
				obj_rc cdef = cdef_mgr.zobject();
				val_rc ftype = cdef.property(MIS.type_str);
				val_rc fname = cdef.property(MIS.name_str);


				if (ftype.isString() && zs_cmp_ci(ftype.zstr(),stamp_type)==0)
				{
				   val_ptr test = tsf.get(fname.zstr());
				   switch(test.zlong())
				   {
				   case 1:
				   	timestamps_ |= UPDATE_TS;
				     break;
				   case 2:
				   	timestamps_ |= CREATE_TS;
				     break;
				   default:
				   	break;
				   }	
				}
			}
		}
		result.value_ = class_tdef_;
		return result;
	}

	bool 
	Model::hasTimeStamps() const
	{
	    return (timestamps_ != NO_TS);
	}

	bool_return
	Model::saveRow(obj_ptr row_obj, bool reload)
	{
		bool_return result;
		error_return check;
		val_return   vret;

		result.value_ = false;

		if (!row_obj.ok())
		{
			result.error() << "Null IRow object";
			return result;
		}
		IRow* irow = zobj_toc<IRow>(row_obj);

		htab_rc dirty = irow->getDirty();
		bool wasRead = irow->exists();

		obj_return build_ret = getBuilderForMe();
		if (build_ret.has_errors())
		{
			result = build_ret.move_error();
			return result;
		}

		IBuild* ibuild = zobj_toc<IBuild>(build_ret.value_);

		if (wasRead && (dirty.size()==0) )
		{
			result.value_ = true;
			return result;
		}
		// insert operation
		//zend_printf("save-row\n");

		htab_return pk_ret = getPKey();
		if (pk_ret.has_errors())
		{
			result = pk_ret.move_error();
			return result;
		}

		val_rc pkey_mgr(pk_ret.value_);

		//showmem("pkey_mgr", pkey_mgr);

		htab_ptr pkey(pkey_mgr);

		val_rc saved;

		if (wasRead) 
		{
			// update operation

			if (pkey.size() == 0)
			{
				result.error() << "Update call has no primary key";
				return result;
			}

			htab_rc id = irow->getDataValues(pkey_mgr);

			if (id.size() == 0)
			{
				result.error() << "No values for primary key";
				return result;
			}

			
			val_rc pvalues(id);
			check = ibuild->whereKeyValue(pkey_mgr, pvalues);

			if (check.has_errors())
			{
				result = check.move_error();
				return result;
			}
			vret = ibuild->update(irow, dirty);
			
			if (vret.has_errors())
			{
				result = vret.move_error();
				return result;
			}
		}
		else {
			htab_ptr data = irow->reader();
			htab_return options_ret = getKeyOptions();
			if (options_ret.has_errors())
			{
				result = std::move(options_ret);
				return result;
			}
			htab_rc& options = options_ret.value_;

			htab_rc pkey_refresh_mgr;
			htab_rw pkey_refresh(pkey_refresh_mgr);

			htab_walk wk;
			auto pname = wk.value();

			for(wk.start(pkey); wk.ok(); wk.next())
			{
				val_ptr data_value = data.get(pname);

				if (data_value.isNull()) 
				{
					val_ptr pkey_options = options.get(pname);
					if (pkey_options.isArray()) 
					{
						htab_ptr pkoption(pkey_options.zarray());

						val_ptr option_key = pkoption.get(MIS.returns_key);
						int option = option_key.zlong();
						switch(option)
						{
						case Crud::ID_SET:
						case Crud::LAST_ID:
						case Crud::ID_GEN:
						case Crud::LAST_SEQ:
							ibuild->setReturns(pkey);
							pkey_refresh.set(pname, pkey_options);
							break;
						default:	
							break;
						}
					}
				}
			}

			val_rc row_mgr(row_obj);
			vret = ibuild->insert(row_mgr);
			if (vret.has_errors())
			{
				result = std::move(vret);
				return result;
			}
			saved = vret.value_;
			//update row values from return?
			if (saved.isArray() && (pkey_refresh_mgr.size() > 0))
			{
				// its a double wrap
				htab_ptr values_wrap(saved.zarray());
				htab_ptr values(values_wrap.get(int(0)));

				//showdata("saved values", values);
				htab_walk owk;
				auto pkey_options = owk.value();

				for(owk.start(pkey_refresh_mgr); owk.ok(); owk.next())
				{

					if (pkey_options.isArray()) 
					{
						htab_ptr pkoption(pkey_options.zarray());
						//showdata("pkoption", pkoption);

						val_ptr option_key = pkoption.get(MIS.returns_key);
						int option = option_key.zlong();
						switch(option) 
						{
							case Crud::ID_SET:
							case Crud::LAST_ID:
							case Crud::ID_GEN:
							case Crud::LAST_SEQ:
								{
									htab_walk pwk;
									auto name = pwk.value();

									for(pwk.start(pkey); pwk.ok(); pwk.next())
									{
										//showmem("name", name);
										val_ptr vtemp = values.get(name);

										//showmem("vtemp", vtemp);
										irow->set(name.zstr(), vtemp);
									}	
								}
								break;						
						}
					}
					
				}
			}
		}


		irow->setExists();
		
		result.value_ = saved.ok();

		if (reload) {
			obj_return rec = readRow(irow);
			if (rec.has_errors())
			{
				zend_printf("readRow has errors\n");
				result = std::move(rec);
				if (!result.has_errors())
				{
					zend_printf("Should have errors\n");
				}
				result.value_ = false;
			}
			else if (rec.value_.ok())
			{
				irow->copy(rec.value_);
			}
		}
		
		return result;
	}

	obj_return 
	Model::readRow(obj_ptr row_obj)
	{
		//zend_printf("Read Row\n");
		obj_return result;

		htab_return pkey_ret = getPKey();
		if (pkey_ret.has_errors())
		{
			result = std::move(pkey_ret);
			return result;
		}
		htab_rc& pkey = pkey_ret.value_;

		//showdata("pkey", pkey);

		if (pkey.size())
		{
			IRow* irow = zobj_toc<IRow>(row_obj);

			val_rc key_mgr(pkey);
			

			htab_rc pkeyid = irow->getDataValues(key_mgr);

			
			val_rc val_mgr(pkeyid);

			result = byKeyValue(key_mgr, val_mgr);
		}
		else {
			str_rc name = getName();
			result.error() << "No primary key for table " << name;
			//zend_printf("error %s\n", result.errors_->data());
		}
		//showobj("obj_return", result.value_);
		return result;

	}

	bool_return
	Model::exists(obj_ptr rowobj)
	{
		bool_return result;

		obj_return buildret = getBuilderForMe();
		if (buildret.has_errors())
		{
			result = std::move(buildret);
			return result;
		}

		IBuild* ib = zobj_toc<IBuild>(buildret.value_);
		IRow*   irow = zobj_toc<IRow>(rowobj);

		htab_return pkeyret = getPKey();
		if (pkeyret.has_errors())
		{
			result = std::move(pkeyret);
			return result;
		}
		val_rc pkey_mgr(pkeyret.value_);

		val_rc pkeyid(irow->getDataValues(pkey_mgr));

		str_rc tname = getName();

		ib->table(tname);
		ib->whereKeyValue(pkey_mgr, pkeyid);
		val_rc columns(SQSTR.asterisk);

		int_return rowctret = ib->count(columns);
		if (rowctret.has_errors())
		{
			result = std::move(rowctret);
			return result;
		}
		result.value_ = (rowctret.value_ > 0);
		return result;

	}

	htab_return 
	Model::getFieldDef(str_ptr name)
	{
		htab_return result;

		htab_return cdefs_ret = getColDefs();

		if (cdefs_ret.has_errors())
		{
			result = std::move(cdefs_ret);
			return result;
		}
		htab_rc& cdefs = cdefs_ret.value_;
		if (cdefs.size())
		{
			result.value_ = cdefs.get(name);
		}
		return result;
	}

	htab_return 
	Model::getForeignKey()
	{

		htab_return result;

		htab_return pkeyret = getPKey();

		if (pkeyret.has_errors())
		{
			result = std::move(pkeyret);
			return result;
		}

		htab_rc& pkey = pkeyret.value_;

		str_rc table = Model::getTableName(vobj()->ce->name);

		htab_walk wk;
		auto name = wk.value();

		htab_rc fkey_list;
		htab_rw hw(fkey_list);
		str_buf buf;

		for(wk.start(pkey); wk.ok(); wk.next())
		{
			buf << table << '_' << name.zstr();
			str_rc fkey = buf.zstr();
			hw.push_back(fkey);
		}
		result.value_ = fkey_list;
		return result;
	}

	htab_return 
	Model::getSeqDefs()
	{
		htab_return result;

		if (seq_defs_.ok())
		{
			result.value_ = seq_defs_;
			return result;
		}

		obj_return tdefret = getTableDef();
		if (tdefret.has_errors())
		{
			result = tdefret.move_error();
			return result;
		}
		obj_rc& tdef = tdefret.value_;
		seq_defs_ = tdef.call(MIS.fn_getseqcols);
		result.value_ = seq_defs_;

		return result;
	}

	void Model::setConnect(const weak_ref& db)
	{
		dbref_ = db;

		obj_rc driver = dbref_.get(); 

		IDriver* dv = zobj_toc<IDriver>(driver);
		dbname_ = dv->getName();
	}

	void 
	Model::setColDefs(htab_ptr options)
	{
		class_cdefs_ = options;
	}

	void 
	Model::setKeyOptions(htab_ptr options)
	{
		pkey_options_ = options;
	}

	void 
	Model::setPKey(htab_ptr options)
	{
		class_pkey_ = options;
	}

	void 
	Model::setName(str_ptr name)
	{
		obj_rc self(vobj());

		self.property(MIS.name_str, name);
	}

	void 
	Model::setSeqDefs(htab_ptr options)
	{
		seq_defs_ = options;
	}

	void Model::setTSFlags(int flags)
	{
		timestamps_ = flags;
	}

	htab_rc 
	Model::stampTime(str_ptr str_datetime, int flags)
	{
		int ts = timestamps_ & flags;

		htab_rc result(htab_rc::empty_array());

		if (ts != 0) 
		{
			htab_rw stamps(result);
			str_rc dkey;

			if ((ts & CREATE_TS) != 0)
			{
				dkey = createdAtName();
				stamps.set(dkey, str_datetime);
			}
			if ((ts & UPDATE_TS) != 0)
			{
				dkey = updatedAtName();
				stamps.set(dkey, str_datetime);
			}
		}
		//showarray("stampTime()", result);
		return result;

	}

}; // namespace wcd

using namespace wcd;

ZEND_METHOD(Wcd_Model, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	model->construct();
}

ZEND_METHOD(Wcd_Model, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	model->destruct();
}


ZEND_METHOD(Wcd_Model, KeyValue)
{
	zval* keynames;
	zval* values;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_ARRAY(keynames)
	Z_PARAM_ARRAY(values)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	obj_return result = Model::keyValue(static_class->name, keynames, values);
	result.throw_errors();
	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, WithValues)
{
	zval* values;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(values)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	obj_return result = Model::withValues(static_class->name, values);
	result.throw_errors();
	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, __callStatic)
{
	zend_string* method;
	zval*         params;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(method)
	Z_PARAM_ARRAY(params)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);
	val_return result = Model::callStatic(static_class->name, method, params);
	result.throw_errors();
	result.value_.move_zv(return_value);

}

ZEND_METHOD(Wcd_Model, classToTableName)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();

	str_rc result = Model::classToTableName(cname);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, createFromResult)
{
	zend_string* cname;
	zval* results;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_STR(cname)
	Z_PARAM_ARRAY(results)
	ZEND_PARSE_PARAMETERS_END();

	val_rc result = Model::createFromResult(cname, results);

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, find)
{
	zval* values;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(values)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	obj_return result = Model::find(static_class->name, values);
	result.throw_errors();
	result.value_.move_zv(return_value);	

}

ZEND_METHOD(Wcd_Model, getTableName)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	str_rc result = Model::getTableName(static_class->name);
	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, importFromCSV)
{
	zend_string* filename;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(filename)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	int_return icount = Model::importFromCSV(static_class->name, filename);
	icount.throw_errors();
	RETURN_LONG(icount.value_);
}

ZEND_METHOD(Wcd_Model, modelBuild)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	obj_return ibuild = Model::modelBuild(static_class->name);
	ibuild.throw_errors();
	ibuild.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, row)
{
	HashTable* data = nullptr;

	ZEND_PARSE_PARAMETERS_START(0,1)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_HT(data)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);


	obj_rc irow = Model::row(static_class->name, data);

	irow.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, rowSaved)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	obj_rc irow = Model::rowSaved(static_class->name, data);

	irow.move_zv(return_value);
}


void 
row_output(zend_execute_data *execute_data, zval *return_value)
{
	zarg_rd args(execute_data);
	obj_ptr irow_obj;
	bool    reload = false;

	args.obj_ofclass(irow_obj, args.need(0), IRow::omg.classEntry());
	args.zbool(reload, args.option(1));

	if (!args.throw_errors())
	{
		Model* model = zval_toc<Model>(ZEND_THIS);
		//zend_printf("\nCall save row\n");
		bool_return result = model->saveRow(irow_obj, reload);
		if (result.has_errors())
		{	
			//zend_printf("\nsaveRow errors %s\n", result.errors_->data());
			result.throw_errors();
		}
		RETURN_BOOL(result.value_);
		
	}
	else {
		RETURN_BOOL(false);
	}		
}

ZEND_METHOD(Wcd_Model, createRow)
{
	row_output(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

ZEND_METHOD(Wcd_Model, saveRow)
{
	row_output(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

ZEND_METHOD(Wcd_Model, updateRow)
{
	row_output(INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

ZEND_METHOD(Wcd_Model, createdAtName)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	str_rc result = model->createdAtName();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, updatedAtName)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	str_rc result = model->updatedAtName();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, deleteRow)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(data, IRow::omg.classEntry())
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);

	bool_return result = model->deleteRow(data);
	result.throw_errors();
	RETURN_BOOL(result.value_);
}

ZEND_METHOD(Wcd_Model, exists)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(data, IRow::omg.classEntry())
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);

	bool_return result = model->exists(data);
	result.throw_errors();
	RETURN_BOOL(result.value_);
}

ZEND_METHOD(Wcd_Model, getBuilder)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	obj_return result = model->getBuilder();
	result.throw_errors();
	result.value_.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, getBuilderForMe)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	obj_return result = model->getBuilderForMe();

	result.throw_errors();
	result.value_.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, getColDefs)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	htab_return result = model->getColDefs();
	result.throw_errors();

	result.value_.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, getConnect)
{
     ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	obj_return result = model->getConnect();
	result.throw_errors();
	result.value_.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, getFieldDef)
{
	zend_string* data;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(data)
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);

	htab_return result = model->getFieldDef(data);
	result.throw_errors();
	result.value_.move_zv(return_value);	

}

ZEND_METHOD(Wcd_Model, getForeignKey)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	htab_return result = model->getForeignKey();
	result.throw_errors();

	result.value_.move_zv(return_value);	
}


ZEND_METHOD(Wcd_Model, getKeyOptions)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	htab_return result = model->getKeyOptions();

	result.throw_errors();

	result.value_.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, getName)
{

	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	str_rc result = model->getName();

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, getPKey)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	htab_return result = model->getPKey();
	result.throw_errors();
	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, getSeqDefs)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	htab_return result = model->getSeqDefs();

	result.throw_errors();
	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, getTSFlags)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	int result = model->getTSFlags();

	RETURN_LONG(result);

}

ZEND_METHOD(Wcd_Model, getTableDef)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	obj_return result = model->getTableDef();

	result.throw_errors();
	result.value_.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, hasTimeStamps)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	bool result = model->hasTimeStamps();

	RETURN_BOOL(result);
}

ZEND_METHOD(Wcd_Model, newRow)
{
	HashTable* 	 rdata = (HashTable*) &zend_empty_array;
	bool  isSaved = false;

	ZEND_PARSE_PARAMETERS_START(0,2)
	Z_PARAM_OPTIONAL
	Z_PARAM_ARRAY_HT(rdata)
	Z_PARAM_BOOL(isSaved)
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);


	obj_rc result = model->newRow(rdata, isSaved);

	result.move_zv(return_value);
}


ZEND_METHOD(Wcd_Model, readRow)
{
	zval* rdata;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(rdata, IRow::omg.classEntry())
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);

	obj_return result = model->readRow(rdata);
	//zend_printf("result & %lx\n", (uint64_t) &result);

	result.throw_errors();
	result.value_.move_zv(return_value);

}

ZEND_METHOD(Wcd_Model, setColDefs)
{
	zval* rdata;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(rdata)
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);
	model->setColDefs(rdata);
}

ZEND_METHOD(Wcd_Model, setConnect)
{
	weak_ref cobj;

	zarg_rd args(execute_data);

	args.weakref(cobj, args.need(0));

	if (!args.throw_errors())
	{
		Model* model = zval_toc<Model>(ZEND_THIS);
		model->setConnect(cobj);
	}
}

ZEND_METHOD(Wcd_Model, setKeyOptions)
{
	zval* rdata;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(rdata)
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);
	model->setKeyOptions(rdata);
}

ZEND_METHOD(Wcd_Model, setName)
{
	zend_string* sdata;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(sdata)
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);
	model->setName(sdata);
}

ZEND_METHOD(Wcd_Model, setPKey)
{
	zval* rdata;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(rdata)
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);
	model->setPKey(rdata);
}

ZEND_METHOD(Wcd_Model, setSeqDefs)
{
	zval* rdata;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(rdata)
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);
	model->setSeqDefs(rdata);
}

ZEND_METHOD(Wcd_Model, setTSFlags)
{
	zend_long flags;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);
	model->setTSFlags(flags);
}


ZEND_METHOD(Wcd_Model, stampTime)
{
	zend_string* str_datetime;

	zend_long flags = Model::ALL_TS;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_STR(str_datetime)
	Z_PARAM_OPTIONAL
	Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);
	htab_rc result = model->stampTime(str_datetime, flags);

	result.move_zv(return_value);
}
/*

ZEND_METHOD(Wcd_Model, stampTime){}
*/


PHP_MINIT_FUNCTION(Wcd_Model_reg)
{
	IRow::register_class();

	zintf_ce_IfCrud = register_class_Wcd_IfCrud();

	Model::omg.classEntry(register_class_Wcd_Model(zintf_ce_IfCrud));

	STATE_INIT_ADD(MIS)
	
	return SUCCESS;
}

#endif