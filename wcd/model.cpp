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

	base_obj_mgr<Model> Model::omg;
	
	zend_class_entry* zintf_ce_IfCrud;

	class Model_init : public state_init {
	public:
		Model_init() : state_init() {}

		str_intern find_first;
		str_intern find_all;
		str_intern by_str;

		str_intern eq_str;
		str_intern u_model;
		str_intern r_arg;
		str_intern escape_key;
		str_intern escape_str;
		str_intern get_tables;
		str_intern columns_str;
		str_intern m_updated_at;
		str_intern m_created_at;
		str_intern m_datetime_type;
		str_intern type_str;
		str_intern name_str;
		str_intern returns_key;
		str_intern get_primary_key;
		str_intern fn_getseqcols;
		str_intern k_created_at;
		str_intern k_updated_at;

		str_intern k_pkey_options;
		str_intern k_seq_defs;
		str_intern k_col_defs;
		str_intern k_pkey;
		str_intern k_tdef;
		str_intern k_buildme;
		str_intern k_driver;



		void init() override {
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


		}
	};

	Model_init MIS;

	Model::Model() : base_d()
	{
		timestamps_ = NO_TS;
	}

	void 
	Model::destruct()
	{
		db_.init();
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
			r.push_back(m->newRow(result, true));
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
		di.set(MIS.k_driver, db_);

		di.set(MIS.k_pkey_options, pkey_options_);
		di.set(MIS.k_col_defs, class_cdefs_);
		di.set(MIS.k_pkey, class_pkey_);
		di.set(MIS.k_tdef, class_tdef_);
		di.set(MIS.k_buildme, builder_me_);



	}

	bool 
	Model::deleteRow(obj_ptr rowobj)
	{
		//TODO: preconfirm exists? i.e. original_ has content
		//zend_printf("deleteRow ");
		//showobj("rowobj", rowobj);
		obj_rc builder(getBuilderForMe());
		IBuild* ib = zobj_toc<IBuild>(builder);
		return ib->deleteRow(rowobj);
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

	obj_rc 
	Model::getConnect()
	{
		if (db_.ok())
		{
			return IServer::connect(db_);
		}

		obj_rc dbobj = IServer::connect(val_ptr());
		if (dbobj.ok())
		{
			IDriver* driver = zobj_toc<IDriver>(dbobj);
			db_ = driver->getName();
		}
		return dbobj;
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

	obj_rc 
	Model::getBuilderForMe()
	{
		if (!builder_me_.ok())
		{
 
			obj_rc db_mgr = getConnect();
			IDriver* db = zobj_toc<IDriver>(db_mgr);

			builder_me_ = db->newDmlBuild();
		}
		if (builder_me_.ok())
		{
			IBuild* ib = zobj_toc<IBuild>(builder_me_);

			ib->setModel(obj_ptr(this->vobj()));
		}
		return builder_me_;
	}

	obj_rc
	Model::modelBuild(str_ptr classname)
	{
		Model* m = model_instance(classname);
		return m->getBuilderForMe();
	}

	obj_rc  
	Model::byKeyValue(val_ptr keynames, val_ptr values)
	{
		obj_rc build = getBuilderForMe();
		IBuild* ib = zobj_toc<IBuild>(build);
		Bindings&  bind = ib->bindings();
		bind.limit(1);
		bind.whereKeyValue(keynames, values);
		obj_rc result(bind.select());
		//showobj("byKeyValue return", result);
		return result;	
	}

	Model* //static
	Model::model_instance(str_ptr classname)
	{
		obj_ptr model = Services::getOne(classname);
		Model* m = zobj_toc<Model>(model);

		return m;
	}

	obj_rc //static
	Model::keyValue(str_ptr static_class, val_ptr keynames, val_ptr values)
	{
		
		Model* m = Model::model_instance(static_class);
		return m->byKeyValue(keynames, values);
	}

	obj_rc 
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

	val_rc 
	Model::callStatic(str_ptr static_name, str_ptr method, val_ptr params)
	{
		htab_ptr  parray(params.zarray());

		Model* m = Model::model_instance(static_name);
		obj_rc build = m->getBuilderForMe();

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
				return ib->allRows();
			}
			val_rc   result = ib->allRows();

			if (val_ptr(result).isObject())
			{
				htab_rc rows_mgr;
				htab_rw rows(rows_mgr);

				rows.push_back(result);
				result = rows_mgr;
			}
			return result;
		}
		val_rc obj_method;
		htab_rw arg1(obj_method);

		arg1.push_back(build);
		arg1.push_back(mlower);
		
		return call_user_func_array(obj_method, params);
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

	obj_rc //static 
	Model::find(str_ptr static_name, val_ptr id)
	{
		
		obj_rc result;

		Model* m = model_instance(static_name);
		htab_rc pkey = m->getPKey();
		val_rc pkey_mgr(pkey);

		if (!pkey.size())
		{
			zend_throw_error(zend_ce_error,"Model without Primary key columns");
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
		//showdata("pkey_mgr", pkey_mgr.zarray());
		//showdata("vlist_tab", vlist_tab.zarray());
		//zend_printf("Return find\n");
		return result;
	}

	htab_rc 
	Model::getKeyOptions()
	{
		htab_rc result;

		result = pkey_options_;

		if (result.ok())
		{
			//showdata("options set", result);
			return result;
		}
		
		htab_rc pkey_fields = getPKey();
		//showdata("pkey get", pkey_fields);
		if (pkey_fields.size())
		{
			htab_rw pkey_options(result);

			htab_rc cdefs = getColDefs();
			htab_rc seqdefs = getSeqDefs();

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
			pkey_options_ = result;
		}
		return result;
	}

	htab_rc Model::getColDefs()
	{
		if (class_cdefs_.ok())
		{
			return class_cdefs_;
		}

		obj_rc tabledef_mgr = getTableDef();

		if (tabledef_mgr.ok())
		{
			class_cdefs_ = tabledef_mgr.property(MIS.columns_str);
		}
		//showdata("getColDefs", class_cdefs_);
		return class_cdefs_;
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

	htab_rc 
	Model::getPKey()
	{
		//zend_printf("in getPKey()\n");
		htab_rc result;
		if (class_pkey_.ok())
		{
			result = class_pkey_;
			//showdata("class pkey", result);
			return result;
		}
		obj_rc tdef = getTableDef();
		if (tdef.ok())
		{	
			//zend_printf("got TDEF\n");
			obj_rc pkeydef = tdef.call(MIS.get_primary_key);
			//showobj("pkeydef:", pkeydef);

			if (pkeydef.ok())
			{
				result = pkeydef.property(MIS.columns_str);
			}
		}
	
		if (!result.ok())
		{
			result = htab_rc::empty_array();
		}
		class_pkey_ = result;
		return result;
	}

	int // static
	Model::importFromCSV(str_ptr static_name, str_ptr filename)
	{
		if (! std::filesystem::exists(filename.vstr()) )
		{
			return 0;
		}

		int datarowct = -1;

		Model* m = model_instance(static_name);

		str_rc tableName = m->getName();

		obj_rc db = m->getConnect();
		IDriver* driver = zobj_toc<IDriver>(db);

		htab_rc columns = m->getColDefs();
		//showdata("columns", columns);

		htab_rc fieldNames;

		bool init = false;
		unsigned int  colcount = 0;

		obj_rc builder = m->getBuilderForMe();

		val_rc import_mgr = fopen(filename, MIS.r_arg);

		val_ptr import(import_mgr);

		if (import.isResource())
		{
			htab_rc csv_args;
			htab_rw csv(csv_args);
			csv.set(MIS.escape_key, MIS.escape_str);

			fn_fgetcsv fgetcsv;
			fgetcsv.set_named_args(csv_args);

			fn_stripslashes stripslashes;
			val_rc stmt;

			while(true)
			{
				val_rc line_mgr = fgetcsv.call(import);
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
					cellstr = stripslashes.call(item);
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

					obj_rc plist_mgr = ib->getInsertSql(columns_mgr);
					ParamList* plist = zobj_toc<ParamList>(plist_mgr);

					str_ptr sql(plist->getSql());
					htab_ptr record(plist->getValues());

					//showstr("sql", sql);
					//showdata("record", record);

					stmt = driver->prepare(sql);

					driver->bind(stmt, record);
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

			htab_rc seq_defs = m->getSeqDefs();

			if (seq_defs.size())
			{
				m->sequenceMax();
			}

			driver->commit();
			//zend_printf("ROWS %d\n", datarowct);

		}
		return datarowct+1;
	}

	obj_rc 
	Model::getTableDef()
	{
		if (class_tdef_.ok())
		{
			return class_tdef_;
		}

		str_rc name = getName();
		//showstr("name", name);

		obj_rc driver = getConnect();

		IDriver* db = zobj_toc<IDriver>(driver);

		obj_rc schema = db->getSchema();

		htab_rc tables = schema.call(MIS.get_tables);
		//showdata("tables from schema", tables);

		 

		class_tdef_ = tables.get(name);

		//showobj("TDEF", class_tdef_);
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
		return class_tdef_;
	}

	bool 
	Model::hasTimeStamps() const
	{
	    return (timestamps_ != NO_TS);
	}

	bool 
	Model::saveRow(obj_ptr row_obj, bool reload)
	{
		if (!row_obj.ok())
		{
			return false;
		}
		IRow* irow = zobj_toc<IRow>(row_obj);

		htab_rc dirty = irow->getDirty();
		bool wasRead = irow->exists();

		obj_rc builder = getBuilderForMe();
		IBuild* ibuild = zobj_toc<IBuild>(builder);

		if (wasRead && (dirty.size()==0) )
		{
			return true;
		}
		// insert operation
		//zend_printf("save-row\n");
		val_rc pkey_mgr(getPKey());
		//showmem("pkey_mgr", pkey_mgr);

		htab_ptr pkey(pkey_mgr);

		val_rc saved;

		if (wasRead) 
		{
			// update operation
			//zend_printf("save-update\n");
			if (pkey.size() == 0)
			{
				zend_throw_error(zend_ce_error, "Update table needs a primary key");
				return false;
			}

			htab_rc id = irow->getDataValues(pkey_mgr);

			if (id.size() == 0)
			{
				zend_throw_error(zend_ce_error, "Save record needs primary key values");
				return false;
			}

			
			val_rc pvalues(id);
			ibuild->whereKeyValue(pkey_mgr, pvalues);
			saved = ibuild->update(irow, dirty);
		}
		else {
			//zend_printf("save-create\n");

			htab_ptr data = irow->reader();
			htab_ptr options = getKeyOptions();

			

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
			saved = ibuild->insert(row_mgr);
			

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
										irow->set(name, vtemp);
									}	
								}
								break;						
						}
					}
					
				}
			}
		}

		irow->setExists();

		if (reload) {
			obj_rc rec = readRow(irow);
			irow->copy(rec);
		}

		return saved.ok();
	}

	obj_rc 
	Model::readRow(obj_ptr row_obj)
	{
		//zend_printf("Read Row\n");

		htab_rc pkey = getPKey();
		obj_rc result(row_obj);

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
			zend_throw_error(zend_ce_error,"No primary key for table %s", name.data());
		}
		return result;

	}

	bool 
	Model::exists(obj_ptr rowobj)
	{
		obj_rc builder = getBuilderForMe();
		IBuild* ib = zobj_toc<IBuild>(builder);
		IRow*   irow = zobj_toc<IRow>(rowobj);

		val_rc pkey_mgr(getPKey());

		val_rc pkeyid(irow->getDataValues(pkey_mgr));

		str_rc tname = getName();

		ib->table(tname);
		ib->whereKeyValue(pkey_mgr, pkeyid);
		val_rc columns(SQSTR.asterisk);

		int rowct = ib->count(columns);

		return (rowct > 0);

	}

	htab_rc 
	Model::getFieldDef(str_ptr name)
	{
		htab_rc cdefs = getColDefs();
		htab_rc result;

		if (cdefs.size())
		{
			result = cdefs.get(name);
		}
		return result;
	}

	htab_rc 
	Model::getForeignKey()
	{
		htab_rc pkey = getPKey();
		str_rc table = Model::getTableName(vobj()->ce->name);

		htab_walk wk;
		auto name = wk.value();

		htab_rc result;
		htab_rw hw(result);
		str_buf buf;

		for(wk.start(pkey); wk.ok(); wk.next())
		{
			buf << table << '_' << name.zstr();
			str_rc fkey = buf.zstr();
			hw.push_back(fkey);
		}
		return result;
	}

	htab_rc 
	Model::getSeqDefs()
	{

		htab_rc result;

		if (seq_defs_.ok())
		{
			result = seq_defs_;
			return result;
		}

		obj_rc tdef = getTableDef();

		seq_defs_ = tdef.call(MIS.fn_getseqcols);
		result = seq_defs_;
		return result;

	}

	void Model::setConnect(obj_ptr db)
	{
		IDriver* driver = zobj_toc<IDriver>(db);
		db_ = driver->getName();
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

	obj_rc result = Model::keyValue(static_class->name, keynames, values);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, WithValues)
{
	zval* values;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(values)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	obj_rc result = Model::withValues(static_class->name, values);

	result.move_zv(return_value);
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
	val_rc result = Model::callStatic(static_class->name, method, params);
	result.move_zv(return_value);

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

	obj_rc result = Model::find(static_class->name, values);

	result.move_zv(return_value);	

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

	int icount = Model::importFromCSV(static_class->name, filename);
	RETURN_LONG(icount);
}

ZEND_METHOD(Wcd_Model, modelBuild)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	obj_rc ibuild = Model::modelBuild(static_class->name);

	ibuild.move_zv(return_value);
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
	zval* data;
	bool  reload = false;

	ZEND_PARSE_PARAMETERS_START(1,2)
	Z_PARAM_OBJECT_OF_CLASS(data, IRow::omg.classEntry())
	Z_PARAM_OPTIONAL
	Z_PARAM_BOOL(reload)
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);

	RETURN_BOOL(model->saveRow(data, reload));	
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

	bool result = model->deleteRow(data);

	RETURN_BOOL(result);
}

ZEND_METHOD(Wcd_Model, exists)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(data, IRow::omg.classEntry())
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);

	bool result = model->exists(data);

	RETURN_BOOL(result);
}

ZEND_METHOD(Wcd_Model, getBuilder)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	obj_rc result = model->getBuilder();

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, getBuilderForMe)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	obj_rc result = model->getBuilderForMe();

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, getColDefs)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	htab_rc result = model->getColDefs();

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, getConnect)
{
     ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	obj_rc result = model->getConnect();

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, getFieldDef)
{
	zend_string* data;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(data)
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);

	htab_rc result = model->getFieldDef(data);
	result.move_zv(return_value);	

}

ZEND_METHOD(Wcd_Model, getForeignKey)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	htab_rc result = model->getForeignKey();

	result.move_zv(return_value);	
}


ZEND_METHOD(Wcd_Model, getKeyOptions)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	htab_rc result = model->getKeyOptions();

	result.move_zv(return_value);	
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

	htab_rc result = model->getPKey();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, getSeqDefs)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	htab_rc result = model->getSeqDefs();

	result.move_zv(return_value);
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

	obj_rc result = model->getTableDef();

	result.move_zv(return_value);
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

	obj_rc result = model->readRow(rdata);

	result.move_zv(return_value);

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
	zval* db;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(db, IDriver::omg.class_entry_)
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);
	model->setConnect(db);
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
	auto ce = register_class_Wcd_IRow(
		Hmap::omg.classEntry()
	);

	IRow::omg.classEntry(ce);

	zintf_ce_IfCrud = register_class_Wcd_IfCrud();

	Model::omg.classEntry(register_class_Wcd_Model(zintf_ce_IfCrud));

	return SUCCESS;
}

#endif