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

		zstr_intern find_first;
		zstr_intern find_all;
		zstr_intern by_str;

		zstr_intern eq_str;
		zstr_intern u_model;
		zstr_intern r_arg;
		zstr_intern escape_key;
		zstr_intern escape_str;
		zstr_intern get_tables;
		zstr_intern columns_str;
		zstr_intern m_updated_at;
		zstr_intern m_created_at;
		zstr_intern m_datetime_type;
		zstr_intern type_str;
		zstr_intern name_str;
		zstr_intern returns_key;
		zstr_intern get_primary_key;
		zstr_intern fn_getseqcols;
		zstr_intern k_created_at;
		zstr_intern k_updated_at;

		zstr_intern k_pkey_options;
		zstr_intern k_seq_defs;
		zstr_intern k_col_defs;
		zstr_intern k_pkey;
		zstr_intern k_tdef;
		zstr_intern k_buildme;
		zstr_intern k_driver;



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

	zval_mgr 
	Model::createFromResult(zstr_user classname, htab_read results)
	{
		if (results.size()==0)
		{
			return htab_mgr::empty_array();
		}

		Model* m = model_instance(classname);

		zval_mgr rmgr;
		htab_write r(rmgr);

		htab_walk wk;

		auto result = wk.value();


		for(wk.start(results); wk.ok(); wk.next())
		{
			r.push_back(m->newRow(result, true));
		}

		return rmgr;
	}

	void
	Model::debug_info(htab_write di)
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
	Model::deleteRow(zobj_user rowobj)
	{
		//TODO: preconfirm exists? i.e. original_ has content
		//zend_printf("deleteRow ");
		//showobj("rowobj", rowobj);
		zobj_mgr builder(getBuilderForMe());
		IBuild* ib = zobj_toc<IBuild>(builder);
		return ib->deleteRow(rowobj);
	}

	int Model::getTSFlags() const
	{
		return timestamps_;
	}

	zstr_mgr 
	Model::now() const
	{
		zval_mgr null_ts;
		return datetime_obj::date(DTData.now_format, null_ts);
	}

	zobj_mgr
	Model::newRow(htab_read data, bool isSaved)
	{
		zobj_mgr result = IRow::omg.new_zobj();

		IRow* irow = zobj_toc<IRow>(result);

		irow->construct(vobj(), data, isSaved);

		return result;
	}

	zobj_mgr//static
	Model::row(zstr_user static_name, zval_user data)
	{
		Model* m = model_instance(static_name);
		return m->newRow(data);
	}

	zobj_mgr//static
	Model::rowSaved(zstr_user static_name, zval_user data)
	{
		Model* m = model_instance(static_name);
		zobj_mgr rec = m->newRow(data);
		IRow* irow = zobj_toc<IRow>(rec);
		irow->create();
		return rec;
	}

	zobj_mgr 
	Model::getConnect()
	{
		if (db_.ok())
		{
			return db_;
		}

		db_ = IServer::connect(zval_user());

		return db_;

	}

	zstr_mgr 
	Model::createdAtName()
	{
		return MIS.k_created_at;
	}

	zstr_mgr 
	Model::updatedAtName()
	{
		return MIS.k_updated_at;
	}

	zobj_mgr 
	Model::getBuilderForMe()
	{
		if (!builder_me_.ok())
		{
 
			zobj_mgr db_mgr = getConnect();
			IDriver* db = zobj_toc<IDriver>(db_mgr);

			builder_me_ = db->newDmlBuild();
		}
		if (builder_me_.ok())
		{
			IBuild* ib = zobj_toc<IBuild>(builder_me_);

			ib->setModel(zobj_user(this->vobj()));
		}
		return builder_me_;
	}

	zobj_mgr
	Model::modelBuild(zstr_user classname)
	{
		Model* m = model_instance(classname);
		return m->getBuilderForMe();
	}

	zobj_mgr  
	Model::byKeyValue(zval_user keynames, zval_user values)
	{
		zobj_mgr build = getBuilderForMe();
		IBuild* ib = zobj_toc<IBuild>(build);
		Bindings&  bind = ib->bindings();
		bind.limit(1);
		bind.whereKeyValue(keynames, values);
		zobj_mgr result(bind.select());
		showobj("byKeyValue return", result);
		return result;	
	}

	Model* //static
	Model::model_instance(zstr_user classname)
	{
		zobj_user model = Services::getOne(classname);
		Model* m = zobj_toc<Model>(model);

		return m;
	}

	zobj_mgr //static
	Model::keyValue(zstr_user static_class, zval_user keynames, zval_user values)
	{
		
		Model* m = Model::model_instance(static_class);
		return m->byKeyValue(keynames, values);
	}

	zobj_mgr 
	Model::withValues(zstr_user static_class, zval_user keyvalues)
	{
		Model* m = Model::model_instance(static_class);

		htab_mgr kv(keyvalues.zarray());

		htab_mgr keynames = htab_mgr::getKeys(kv);
		htab_mgr values = htab_mgr::getValues(kv);
		zval_mgr keynames_mgr(keynames);
		zval_mgr values_mgr(values);
		return m->byKeyValue(keynames_mgr, values_mgr);
	}

	zval_mgr 
	Model::callStatic(zstr_user static_name, zstr_user method, zval_user params)
	{
		htab_read  parray(params.zarray());

		Model* m = Model::model_instance(static_name);
		zobj_mgr build = m->getBuilderForMe();

		IBuild* ib = zobj_toc<IBuild>(build);

		zstr_mgr mlower = method.to_lower();
		zstr_user nullstr;
		zval_mgr  nullval;
		if (mlower.starts_with(MIS.find_first)) 
		{
			if (mlower.size()==9) 
			{
				ib->where(params, nullstr, nullval, SQSTR.and_str);
				return ib->oneRow();
			}
			else {
				zstr_mgr bystr = mlower.substr(9);
				if (bystr.starts_with(MIS.by_str))
				{
					zstr_mgr column = bystr.substr(2);

					zval_mgr value = parray.get((int)0);
					zval_mgr col_mgr(column);
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
			zval_mgr   result = ib->allRows();

			if (zval_user(result).isObject())
			{
				htab_mgr rows_mgr;
				htab_write rows(rows_mgr);

				rows.push_back(result);
				result = rows_mgr;
			}
			return result;
		}
		zval_mgr obj_method;
		htab_write arg1(obj_method);

		arg1.push_back(build);
		arg1.push_back(mlower);
		
		return FTAB.call_user_func_array.call(obj_method, params);
	}

	zstr_mgr //static
	Model::getTableName(zstr_user cname)
	{
		return Model::classToTableName(cname);
	}

	zstr_mgr 
	Model::classToTableName(zstr_user class_name)
	{
		zstr_mgr result(class_name);
		int ix = result.rfind('\\');

		if (ix >= 0) {
			result = result.substr(ix+1);
		}

		result = result.uncamel();

		zstr_mgr rlower = result.to_lower();

		ix = rlower.strpos(MIS.u_model);

		if (ix >= 0)
		{
			result = result.substr(0, ix);
		}
		return result;
	}

	zobj_mgr //static 
	Model::find(zstr_user static_name, zval_user id)
	{
		
		zobj_mgr result;

		Model* m = model_instance(static_name);
		htab_mgr pkey = m->getPKey();
		zval_mgr pkey_mgr(pkey);

		if (!pkey.size())
		{
			zend_throw_error(zend_ce_error,"Model without Primary key columns");
		}
		else if (id.isArray())
		{
			htab_read vlist(id.zarray());
			zval_user test = vlist.get((int)0);
			if (test.isNull()) {
				htab_mgr v2 = htab_mgr::sublist(pkey, vlist);
				zval_mgr arg2(v2);
				result = m->byKeyValue(pkey_mgr, arg2);
			}
			else {
				result = m->byKeyValue(pkey_mgr,id);
			}
		}
		else {
			zval_mgr vlist_tab;
			htab_write vlist(vlist_tab);
			vlist.push_back(id);
			result = m->byKeyValue(pkey_mgr, vlist_tab);
		}
		//showdata("pkey_mgr", pkey_mgr.zarray());
		//showdata("vlist_tab", vlist_tab.zarray());
		zend_printf("Return find\n");
		return result;
	}

	htab_mgr 
	Model::getKeyOptions()
	{
		htab_mgr result;

		result = pkey_options_;

		if (result.ok())
		{
			//showdata("options set", result);
			return result;
		}
		
		htab_mgr pkey_fields = getPKey();
		//showdata("pkey get", pkey_fields);
		if (pkey_fields.size())
		{
			htab_write pkey_options(result);

			htab_mgr cdefs = getColDefs();
			htab_mgr seqdefs = getSeqDefs();

			htab_walk wk;
			auto pkey = wk.value();


			for(wk.start(pkey_fields); wk.ok(); wk.next())
			{
				//zend_printf("get pkey options ");
				//showmem("pkey", pkey);
				htab_mgr options_mgr;
				htab_write options(options_mgr);

				zstr_mgr key_name = pkey.zstr();
				key_name = key_name.to_lower();

				zval_user key_def = cdefs.get(key_name);
				zobj_mgr  pkeydef = key_def.zobject();

				zval_mgr seq_pkey = pkeydef.property(SQSTR.id_key);

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
						zval_mgr defval = pkeydef.property(SQSTR.default_key);
						if (defval.isString())
						{
							htab_mgr temp_mgr;
							htab_write temp(temp_mgr);

							temp.push_back(SQSTR.default_key);
							temp.push_back(defval);
							options.set(SQSTR.bind_key, temp_mgr);
						}
						options.set(SQSTR.returns_str, Crud::LAST_SEQ);
					}
				}
				else 
				{
					zval_mgr isAutoInc = pkeydef.property(SQSTR.auto_inc);
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

	htab_mgr Model::getColDefs()
	{
		if (class_cdefs_.ok())
		{
			return class_cdefs_;
		}

		zobj_mgr tabledef_mgr = getTableDef();

		if (tabledef_mgr.ok())
		{
			class_cdefs_ = tabledef_mgr.property(MIS.columns_str);
		}
		//showdata("getColDefs", class_cdefs_);
		return class_cdefs_;
	}

	zstr_mgr Model::getName()
	{
		zobj_user self(vobj());

		zstr_mgr result = self.property(MIS.name_str);

		if (result.ok())
		{
			return result;
		}

		result = Model::classToTableName(self.className());
		self.property(MIS.name_str, result);
		return result;
	}

	htab_mgr 
	Model::getPKey()
	{
		//zend_printf("in getPKey()\n");
		htab_mgr result;
		if (class_pkey_.ok())
		{
			result = class_pkey_;
			//showdata("class pkey", result);
			return result;
		}
		zobj_mgr tdef = getTableDef();
		if (tdef.ok())
		{	
			//zend_printf("got TDEF\n");
			zobj_mgr pkeydef = tdef.call(MIS.get_primary_key);
			//showobj("pkeydef:", pkeydef);

			if (pkeydef.ok())
			{
				result = pkeydef.property(MIS.columns_str);
			}
		}
	
		if (!result.ok())
		{
			result = htab_mgr::empty_array();
		}
		class_pkey_ = result;
		return result;
	}

	int // static
	Model::importFromCSV(zstr_user static_name, zstr_user filename)
	{
		if (! std::filesystem::exists(filename.vstr()) )
		{
			return 0;
		}

		int datarowct = -1;

		Model* m = model_instance(static_name);

		zstr_mgr tableName = m->getName();

		zobj_mgr db = m->getConnect();
		IDriver* driver = zobj_toc<IDriver>(db);

		htab_mgr columns = m->getColDefs();
		htab_mgr fieldNames;

		bool init = false;
		unsigned int  colcount = 0;

		zobj_mgr builder = m->getBuilderForMe();

		zval_mgr import_mgr = FTAB.fopen.call(filename, MIS.r_arg);

		zval_user import(import_mgr);

		if (import.isResource())
		{
			htab_mgr csv_args;
			htab_write csv(csv_args);
			csv.set(MIS.escape_key, MIS.escape_str);

			fn_fgetcsv fgetcsv;
			fgetcsv.set_named_args(csv_args);

			fn_stripslashes stripslashes;
			zval_mgr stmt;

			while(true)
			{
				zval_mgr line_mgr = fgetcsv.call(import);
				zval_user line(line_mgr); 
				zstr_mgr  cellstr;

				if (line.isFalse())
				{
					break;
				}

				htab_mgr values_mgr;

				htab_write values(line.zarray());
				htab_walk sw;

				auto item = sw.value();
				auto ix = sw.key();

				for(sw.start(line.zarray()); sw.ok(); sw.next())
				{
					cellstr = stripslashes.call(item);
					values.push_back(cellstr);
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
							zval_user col = columns.get(vname);
							if (!col.isNull()) {
								colcount += 1;
							}
						}
						if (colcount == values_count) {
							fieldNames = values;
						}
					}
					if (! fieldNames.size())
					{
						fieldNames = columns.slice(0, values_count);
						datarowct = 0;

					}
					init = true;
				}
				if (datarowct == 0) {
					//  prepaire for multi inserts
					htab_mgr columns_mgr;
					htab_write columns(columns_mgr);

					for(sw.start(values); sw.ok(); sw.next())
					{
						zstr_mgr colname = fieldNames.get(ix);
						columns.set(colname, item);
					}
					IBuild* ib = zobj_toc<IBuild>(builder);

					zobj_mgr plist_mgr = ib->getInsertSql(columns_mgr);
					ParamList* plist = zobj_toc<ParamList>(plist_mgr);
					zstr_user sql(plist->getSql());
					htab_read record(plist->getValues());

					zval_mgr stmt = driver->prepare(sql);

					driver->bind(stmt, record);
				}
				else if (datarowct > 0) {
					driver->bind(stmt, values);
				}
				driver->execute(stmt, false, false);
				datarowct += 1;
			}

			driver->closeStmt(stmt);

			FTAB.fclose.call(import);

			htab_mgr seq_defs = m->getSeqDefs();

			if (seq_defs.size())
			{
				m->sequenceMax();
			}

			driver->commit();

		}
		return datarowct;
	}

	zobj_mgr 
	Model::getTableDef()
	{
		if (class_tdef_.ok())
		{
			return class_tdef_;
		}

		zstr_mgr name = getName();
		//showstr("name", name);

		zobj_mgr driver = getConnect();

		IDriver* db = zobj_toc<IDriver>(driver);

		zobj_mgr schema = db->getSchema();

		htab_mgr tables = schema.call(MIS.get_tables);
		//showdata("tables from schema", tables);

		 

		class_tdef_ = tables.get(name);

		//showobj("TDEF", class_tdef_);
		htab_mgr columns = class_tdef_.property(MIS.columns_str);

		//showdata("columns", columns);

		htab_mgr tsf_mgr;
		htab_write tsf(tsf_mgr);

		zobj_mgr self = vobj();

		zstr_mgr ts_update = self.call(MIS.m_updated_at);
		zstr_mgr ts_create = self.call(MIS.m_created_at);

		if (ts_update.size()) {
			tsf.set(ts_update, int(1));
		}
		if (ts_create.size()) {
			tsf.set(ts_create, int(2));
		}

		zstr_mgr stamp_type = class_tdef_.call(MIS.m_datetime_type);
		timestamps_ = 0;

		if (stamp_type.ok())
		{
			htab_walk wk;
			auto cdef_mgr = wk.value();
			for(wk.start(columns); wk.ok(); wk.next())
			{
				zobj_mgr cdef = cdef_mgr.zobject();
				zval_mgr ftype = cdef.property(MIS.type_str);
				zval_mgr fname = cdef.property(MIS.name_str);


				if (ftype.isString() && zs_cmp_ci(ftype.zstr(),stamp_type)==0)
				{
				   zval_user test = tsf.get(fname.zstr());
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
	Model::saveRow(zobj_user row_obj, bool reload)
	{
		if (!row_obj.ok())
		{
			return false;
		}
		IRow* irow = zobj_toc<IRow>(row_obj);

		htab_mgr dirty = irow->getDirty();
		bool wasRead = irow->exists();

		zobj_mgr builder = getBuilderForMe();
		IBuild* ibuild = zobj_toc<IBuild>(builder);

		if (wasRead && (dirty.size()==0) )
		{
			return true;
		}
		// insert operation
		//zend_printf("save-row\n");
		zval_mgr pkey_mgr(getPKey());
		//showmem("pkey_mgr", pkey_mgr);

		htab_read pkey(pkey_mgr);

		zval_mgr saved;

		if (wasRead) 
		{
			// update operation
			zend_printf("save-update\n");
			if (pkey.size() == 0)
			{
				zend_throw_error(zend_ce_error, "Update table needs a primary key");
				return false;
			}

			htab_mgr id = irow->getDataValues(pkey_mgr);

			if (id.size() == 0)
			{
				zend_throw_error(zend_ce_error, "Save record needs primary key values");
				return false;
			}

			
			zval_mgr pvalues(id);
			ibuild->whereKeyValue(pkey_mgr, pvalues);
			saved = ibuild->update(irow, dirty);
		}
		else {
			zend_printf("save-create\n");

			htab_read data = irow->reader();
			htab_read options = getKeyOptions();

			

			htab_mgr pkey_refresh_mgr;
			htab_write pkey_refresh(pkey_refresh_mgr);

			htab_walk wk;
			auto pname = wk.value();

			for(wk.start(pkey); wk.ok(); wk.next())
			{
				zval_user data_value = data.get(pname);

				if (data_value.isNull()) 
				{
					zval_user pkey_options = options.get(pname);
					if (pkey_options.isArray()) 
					{
						htab_read pkoption(pkey_options.zarray());

						zval_user option_key = pkoption.get(MIS.returns_key);
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

			zval_mgr row_mgr(row_obj);
			saved = ibuild->insert(row_mgr);
			

			if (saved.isArray() && (pkey_refresh_mgr.size() > 0))
			{
				// its a double wrap
				htab_read values_wrap(saved.zarray());
				htab_read values(values_wrap.get(int(0)));

				//showdata("saved values", values);
				htab_walk owk;
				auto pkey_options = owk.value();

				for(owk.start(pkey_refresh_mgr); owk.ok(); owk.next())
				{

					if (pkey_options.isArray()) 
					{
						htab_read pkoption(pkey_options.zarray());
						//showdata("pkoption", pkoption);

						zval_user option_key = pkoption.get(MIS.returns_key);
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
										zval_user vtemp = values.get(name);

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
			zobj_mgr rec = readRow(irow);
			irow->copy(rec);
		}

		return saved.ok();
	}

	zobj_mgr 
	Model::readRow(zobj_user row_obj)
	{
		zend_printf("Read Row\n");

		htab_mgr pkey = getPKey();
		zobj_mgr result(row_obj);

		if (pkey.size())
		{
			IRow* irow = zobj_toc<IRow>(row_obj);

			zval_mgr key_mgr(pkey);
			

			htab_mgr pkeyid = irow->getDataValues(key_mgr);

			
			zval_mgr val_mgr(pkeyid);
			result = byKeyValue(key_mgr, val_mgr);
		}
		else {
			zstr_mgr name = getName();
			zend_throw_error(zend_ce_error,"No primary key for table %s", name.data());
		}
		return result;

	}

	bool 
	Model::exists(zobj_user rowobj)
	{
		zobj_mgr builder = getBuilderForMe();
		IBuild* ib = zobj_toc<IBuild>(builder);
		IRow*   irow = zobj_toc<IRow>(rowobj);

		zval_mgr pkey_mgr(getPKey());

		zval_mgr pkeyid(irow->getDataValues(pkey_mgr));

		zstr_mgr tname = getName();

		ib->table(tname);
		ib->whereKeyValue(pkey_mgr, pkeyid);
		zval_mgr columns(SQSTR.asterisk);

		int rowct = ib->count(columns);

		return (rowct > 0);

	}

	htab_mgr 
	Model::getFieldDef(zstr_user name)
	{
		htab_mgr cdefs = getColDefs();
		htab_mgr result;

		if (cdefs.size())
		{
			result = cdefs.get(name);
		}
		return result;
	}

	htab_mgr 
	Model::getForeignKey()
	{
		htab_mgr pkey = getPKey();
		zstr_mgr table = Model::getTableName(vobj()->ce->name);

		htab_walk wk;
		auto name = wk.value();

		htab_mgr result;
		htab_write hw(result);
		zstr_buffer buf;

		for(wk.start(pkey); wk.ok(); wk.next())
		{
			buf << table << '_' << name.zstr();
			zstr_mgr fkey = buf.zstr();
			hw.push_back(fkey);
		}
		return result;
	}

	htab_mgr 
	Model::getSeqDefs()
	{

		htab_mgr result;

		if (seq_defs_.ok())
		{
			result = seq_defs_;
			return result;
		}

		zobj_mgr tdef = getTableDef();

		seq_defs_ = tdef.call(MIS.fn_getseqcols);
		result = seq_defs_;
		return result;

	}

	void 
	Model::setColDefs(htab_read options)
	{
		class_cdefs_ = options;
	}

	void 
	Model::setKeyOptions(htab_read options)
	{
		pkey_options_ = options;
	}

	void 
	Model::setPKey(htab_read options)
	{
		class_pkey_ = options;
	}

	void 
	Model::setName(zstr_user name)
	{
		zobj_mgr self(vobj());

		self.property(MIS.name_str, name);
	}

	void 
	Model::setSeqDefs(htab_read options)
	{
		seq_defs_ = options;
	}

	void Model::setTSFlags(int flags)
	{
		timestamps_ = flags;
	}

	htab_mgr 
	Model::stampTime(zstr_user str_datetime, int flags)
	{
		int ts = timestamps_ & flags;

		htab_mgr result(htab_mgr::empty_array());

		if (ts != 0) 
		{
			htab_write stamps(result);
			zstr_mgr dkey;

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

ZEND_METHOD(Wcd_Model, KeyValue)
{
	zval* keynames;
	zval* values;

	ZEND_PARSE_PARAMETERS_START(2,2)
	Z_PARAM_ARRAY(keynames)
	Z_PARAM_ARRAY(values)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	zobj_mgr result = Model::keyValue(static_class->name, keynames, values);

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, WithValues)
{
	zval* values;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(values)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	zobj_mgr result = Model::withValues(static_class->name, values);

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
	zval_mgr result = Model::callStatic(static_class->name, method, params);
	result.move_zv(return_value);

}

ZEND_METHOD(Wcd_Model, classToTableName)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();

	zstr_mgr result = Model::classToTableName(cname);

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

	zval_mgr result = Model::createFromResult(cname, results);

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, find)
{
	zval* values;
	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ZVAL(values)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	zobj_mgr result = Model::find(static_class->name, values);

	result.move_zv(return_value);	

}

ZEND_METHOD(Wcd_Model, getTableName)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	zstr_mgr result = Model::getTableName(static_class->name);
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

	zobj_mgr ibuild = Model::modelBuild(static_class->name);

	ibuild.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, row)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	zobj_mgr irow = Model::row(static_class->name, data);

	irow.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, rowSaved)
{
	zval* data;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_ARRAY(data)
	ZEND_PARSE_PARAMETERS_END();

	zend_class_entry* static_class = zend_get_called_scope(execute_data);

	zobj_mgr irow = Model::rowSaved(static_class->name, data);

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

	zstr_mgr result = model->createdAtName();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, updatedAtName)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	zstr_mgr result = model->updatedAtName();

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

	zobj_mgr result = model->getBuilder();

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, getBuilderForMe)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	zobj_mgr result = model->getBuilderForMe();

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, getColDefs)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	htab_mgr result = model->getColDefs();

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, getConnect)
{
     ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	zobj_mgr result = model->getConnect();

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, getFieldDef)
{
	zend_string* data;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_STR(data)
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);

	htab_mgr result = model->getFieldDef(data);
	result.move_zv(return_value);	

}

ZEND_METHOD(Wcd_Model, getForeignKey)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	htab_mgr result = model->getForeignKey();

	result.move_zv(return_value);	
}


ZEND_METHOD(Wcd_Model, getKeyOptions)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	htab_mgr result = model->getKeyOptions();

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, getName)
{

	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	zstr_mgr result = model->getName();

	result.move_zv(return_value);	
}

ZEND_METHOD(Wcd_Model, getPKey)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	htab_mgr result = model->getPKey();

	result.move_zv(return_value);
}

ZEND_METHOD(Wcd_Model, getSeqDefs)
{
	ZEND_PARSE_PARAMETERS_NONE();

	Model* model = zval_toc<Model>(ZEND_THIS);

	htab_mgr result = model->getSeqDefs();

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

	zobj_mgr result = model->getTableDef();

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


	zobj_mgr result = model->newRow(rdata, isSaved);

	result.move_zv(return_value);
}


ZEND_METHOD(Wcd_Model, readRow)
{
	zval* rdata;

	ZEND_PARSE_PARAMETERS_START(1,1)
	Z_PARAM_OBJECT_OF_CLASS(rdata, IRow::omg.classEntry())
	ZEND_PARSE_PARAMETERS_END();

	Model* model = zval_toc<Model>(ZEND_THIS);

	zobj_mgr result = model->readRow(rdata);

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
	htab_mgr result = model->stampTime(str_datetime, flags);

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

	/*
	class_data  cd(Model::omg.class_entry_);

	zval_mgr null_val;

	zend_type dtype =  {nullptr, 0};
	cd.typed_property(MIS.name_str, null_val, IS_STRING, ZEND_ACC_PROTECTED_SET);
	*/

	return SUCCESS;
}

#endif