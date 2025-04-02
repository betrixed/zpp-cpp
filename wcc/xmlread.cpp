#ifndef XMLREAD_CPP
#define XMLREAD_CPP

#ifndef XMLREAD_H
#include "xmlread.h"
#endif

#ifndef REFLECT_CACHE_H
#include "reflect_cache.h"
#endif

extern "C" {
	#include "stub/xmlread_arginfo.h"
};


namespace zpp {

	xml_fns XML_FNS;

	xml_fns::xml_fns() : state_init()
	{

	}

	void xml_fns::init()
	{

		xmlreader = "xmlreader";
		fromString = "xmlreader::fromstring";
		open = "xmlreader::open";

		get_attribute = "getattribute";
        read_string = "readstring";
        read = "read";

		k_nodeType = "nodeType";//property name
		k_name = "name";
		k_c = "c";
		k_k = "k";
		k_close = "close";	

		xml_file.set_fname(open);
		xml_parse.set_fname(fromString);

		reader = "reader";	
		root = "root";	
		tags = "tags";	
	}

	zobj_mgr
	xmlreader_xml::call(zstr_user src)
	{
	 	//showstr("xml src", src);
		zval* pz = argsptr();
		ZVAL_STR(pz, src);
		ZVAL_NULL(pz+1);
		ZVAL_LONG(pz+2, 0);

		return zobj_mgr(call_fn());
	}

	zobj_mgr xmlreader_open::call(zstr_user path)
	{
		zval* pz = argsptr();
		ZVAL_STR(pz, path);
		ZVAL_NULL(pz+1);
		ZVAL_LONG(pz+2, 0);
		return zobj_mgr(call_fn());
	}
};

namespace wcc {
	using namespace zpp;

	base_obj_mgr<Wcc_XmlRead> Wcc_XmlRead::omg;

	XmlWrap::XmlWrap() : fileOpen_(false)
	{
	}

	XmlWrap::~XmlWrap()
	{
		if (fileOpen_)
		{
			closeFile();
		}
	}

	void XmlWrap::closeFile()
	{
		if (fileOpen_)
		{
			fileOpen_ = false;
			hold_ = (zend_string*) nullptr;
			((zobj_user*)(this))->call(XML_FNS.k_close);
			lose();
		}
	}

	
	bool XmlWrap::newobj()
	{
		lose();
		obj_ = class_data::create_object(XML_FNS.xmlreader);
		return true;
	}

	bool XmlWrap::adopt_xmlobj(zobj_mgr& test)
	{
		bool result = false;
		if (test.ok())
		{
			// ~test will decref
			lose();
			zobj_mgr::try_addref(test); // anticipate source decref
			self_ = (zend_object*) test;
			obj_ = self_;

			fn_setup();
			fileOpen_ = true;
			result = true;
		}
		else {
			self_ = (zend_object*) nullptr;
		}
		return result;
	}

	bool XmlWrap::fromString(zstr_user xml)
	{
		
		hold_ = xml;
		zobj_mgr test(XML_FNS.xml_parse.call(xml));

		return adopt_xmlobj(test);
	}
	
	bool XmlWrap::fromFile(zstr_user path)
	{
		lose();
		hold_ = path;

		zobj_mgr test(XML_FNS.xml_file.call(path));

		return adopt_xmlobj(test);
	}

	void XmlWrap::fn_setup()
	{
		getattribute_.set_fci(obj_, XML_FNS.get_attribute);
		readstring_.set_fci(obj_, XML_FNS.read_string);
		read_.set_fci(obj_, XML_FNS.read);
	}

	zval_mgr XmlWrap::xml_name_zval()
	{
		zval_mgr result;
		self_.property_get(XML_FNS.k_name, result);
		return result;
	}

	zstr_mgr XmlWrap::xml_name()
	{
		zval test = {0};

		zval* name = self_.property_get(XML_FNS.k_name, &test);
		return zstr_mgr(name);
	}

	zstr_mgr
	XmlWrap::get_attribute(zstr_user name)
	{	
		ZVAL_STR(getattribute_.argsptr(), name);

		zval_mgr attr =  getattribute_.call_fn();
		//showmem("attribute ", attr);
		return zstr_mgr(zval_user(attr));
	}

	zval_mgr
	XmlWrap::xml_str_zval()
	{
		return readstring_.call_fn();
	}

	/**
	zstr_mgr
	XmlWrap::xml_string()
	{
		return readstring_.call_fn();
	}
	*/

	bool XmlWrap::read()
	{

		zval_mgr result = read_.call_fn();
		zval_user test(result);

		return test.isTrue();
	}

	int XmlWrap::nodeType()
	{
		zval test = {0};

		zval* data = self_.property_get(XML_FNS.k_nodeType, &test);
		if (data)
			return zval_user(&test).zlong();
		else
			return 0;
	}

};


using namespace wcc;


// special Xml node constants
enum Xntype {
	ELEMENT = 1,
	END_ELEMENT = 15
};

constexpr std::string_view tb_tag = "tb";
constexpr std::string_view root_tag = "root";
constexpr std::string_view a_tag = "a";

constexpr std::string_view s_tag = "s";
constexpr std::string_view i_tag = "i";
constexpr std::string_view f_tag = "f";
constexpr std::string_view n_tag = "_n";
constexpr std::string_view b_tag = "b";
constexpr std::string_view ea_tag = "ea";
constexpr std::string_view pdoc_tag = "pdoc";
constexpr std::string_view dtm_tag = "dtm";
constexpr std::string_view xtag_tag = "xtag";


void  
Wcc_XmlRead::init() 
{
	done_ = false;
	root_ = nullptr;
	top_ = nullptr;
	stacked_ = 0;

	tag_objs_.reset();
	clean();
}

void
Wcc_XmlRead::clean()
{
	// root_ is path_[0]
	if (!xml_.isNull())
	{
		xml_.closeFile();
	}
	auto ct = stacked_;
	stacked_ = 0;
	while (ct > 0) {
		ct--;
		DStack* last = top_;
		if (last) {
			top_ = last->ds_prev_;
			delete last;
		}
		else {
			top_ = nullptr;
		}
	}
	root_ = nullptr;

}

Wcc_XmlRead::~Wcc_XmlRead()
{
	clean();
}

Wcc_XmlRead::Wcc_XmlRead()
{
	init();
}

bool Wcc_XmlRead::openstring(zstr_user str)
{

	if (!xml_.fromString(str))
	{
		zend_throw_error(zend_ce_error,"Cannot open xml parser",0);
		return false;
	}
	return true;
}

bool Wcc_XmlRead::openfile(zstr_user file)
{
	if (!xml_.fromFile(file))
	{
		zend_throw_error(zend_ce_error,"Cannot open %s for xml parse ",file.data());
		return false;
	}
	return true;
}

zval_mgr 
Wcc_XmlRead::parse(zstr_user src)
{
	zval_mgr result;
	init();
	if (!openstring(src))
	{
		zstr_mgr line(src.substr(0,40));
		zstr_user test(line);

		zend_throw_error(zend_ce_error, "XMLReader::XML fail for '%s'", test.data());
		return result;
	}
	result = loop();
	return result;
}



zval_mgr //static
Wcc_XmlRead::fromString(zstr_user src)
{
	zobj_mgr xmlr = Wcc_XmlRead::omg.new_zobj();
	Wcc_XmlRead* cobj = zobj_toc<Wcc_XmlRead>(xmlr);
	
	return cobj->parse(src);
}

zval_mgr //static
Wcc_XmlRead::fromFile(zstr_user filename)
{
	zobj_mgr xmlr = Wcc_XmlRead::omg.new_zobj();
	//showobj("new_zobj", xmlr);
	Wcc_XmlRead* cobj = zobj_toc<Wcc_XmlRead>(xmlr);
	
	return cobj->parseFile(filename);
}

zval_mgr
Wcc_XmlRead::loop()
{
	zval_mgr result;
	done_ = false;

	//showobj("loop", xml_);

	while(!done_ && xml_.read())
	{
		auto ntype = xml_.nodeType();
		//zend_printf("ntype %ld\n", ntype);

		switch(ntype) {
			case Xntype::ELEMENT:
			{
				zstr_mgr tag = xml_.xml_name();
				zstr_user tagstr(tag);


				//zend_printf("tag = %s\n", tagstr.data());

				zstr_mgr attr = xml_.get_attribute(XML_FNS.k_k);
				zstr_user attrstr(attr);

				//zend_printf("k = %s\n", attr.data());

				bool result = tag_start(tagstr, attrstr);

				if (!result) {
					//zstr_make<false> name(tag);
					zstr_mgr classname = htab_read(tag_objs_).get((zend_string*)tag);
					zstr_user test(classname);

					if (test.isNull()) {
						zstr_user tag_str(tag);
						zend_throw_error(zend_ce_exception, "Unmapped tag %s", tag_str.data());
						done_ = true;
						continue;
					}
					pushClass(classname, attrstr);
				}

			}
			break;
		case Xntype::END_ELEMENT:
			{
				zstr_mgr tag = xml_.xml_name();
				tag_end(tag);
				//zend_printf("exit tag_end\n");
			}
			break;
		case 0:
			done_ = true;
			//zend_printf("End reached\n");
			break;
		}
	}
	if (root_ && !stacked_) {
		result = std::move(root_->ref_);
		delete root_;
		root_ = nullptr;
	}
	else {
		result.set_bool(false);
	}
	clean();
	return result;

}

zval_mgr 
Wcc_XmlRead::parseFile(zstr_user filename)
{
	zstr_mgr  file(filename);
	zval_mgr  result;


	init();
	if (!openfile(file))
	{
		zend_throw_error(zend_ce_error, "XMLRead open fail for %s", filename.data());
		return result;
	}

	result = loop();

	return result;
}

void 
Wcc_XmlRead::tagsTable()
{
	zval_mgr tag;
	zval_mgr value;
	//zend_printf("tagsTable\n");
	
	while(xml_.read()) {
		auto ntype = xml_.nodeType();
		switch(ntype) {
			case Xntype::ELEMENT:
				{
					tag = xml_.xml_name_zval();
					value = xml_.xml_str_zval();
					/*showmem("tagkey set", tag);
					showmem("tagkey value", value);
					showarray("tag_objs", tag_objs_);*/
					htab_write(tag_objs_).set(tag, value);	
				}
				break;
			case Xntype::END_ELEMENT:
				{
					tag = xml_.xml_name_zval();
					std::string_view test = zval_user(tag).vstr();
					//zend_printf("test vstr (%ld) %s\n", test.size(), test.data());
					//showmem("tagkey end", tag);
					if (test == xtag_tag) {
						return;
					}
				}
				break;			
		}
	}
}

zobj_mgr
Wcc_XmlRead::newRoot(zstr_user cname)
{

	zobj_mgr result;

	if (cname.size() > 0)
	{
		result = ReflectCache::staticInstance(cname);
	}
	else {
		result =  class_data::std_object();
	}
	//showobj("newRoot - ", result);
	return result;
}

void Wcc_XmlRead::attach_ds(DStack* ds)
{
	if (stacked_ == 0)
	{
		root_ = ds;
		top_ = root_;
		stacked_ = 1;
	}
	else {
		ds->ds_prev_ = top_;
		top_->ds_next_ = ds;
		top_ = ds;
		stacked_++;
	}
}

/** Push an object root */
void 
Wcc_XmlRead::pushRoot(zstr_user key)
{

	zstr_mgr cname = xml_.get_attribute(XML_FNS.k_c);

	zval_mgr newroot;
	//showstr("pushRoot c=", cname);
	if (stacked_ == 0) {
		if (!addRoot_.isNull()) {
			newroot = addRoot_;
		}
		else {
			newroot = newRoot(cname);
		}
	}
	else {
		newroot = newRoot(cname);
	}
	attach_ds(new DStack(key, newroot, XC_OBJECT));

}

void Wcc_XmlRead::pushClass(zstr_user classname, zstr_user key)
{
	zobj_mgr newroot = ReflectCache::staticInstance(classname);
	zval_mgr store(newroot);

	attach_ds(new DStack(key, store, XC_OBJECT));
}

void Wcc_XmlRead::popStack()
{
	auto ct = stacked_;
	//zend_printf("popStack %ld\n", ct);

	if (ct > 0) {
		ct--;

		DStack* last = top_;
		top_ = last->ds_prev_;
		stacked_ = ct;

		if (ct > 0) {
			//showmem("last ref", last->ref_);
			//showstr("last key", last->key_);

			// set value uses current top stack member
			setValue(last->ref_, last->key_);	

			delete last;		
		}
		else {
			// keep root_ around a bit longer
		}
	}
	else {
		zend_throw_error(zend_ce_error,"Pop with empty element stack",0);
		done_ = true;
	}
	//zend_printf("exit popStack\n");
}

void Wcc_XmlRead::tag_end(zstr_user tag)
{
	//showstr("tag_end", tag);
	std::string_view s = tag.vstr();

	if  (  (s == tb_tag) 
	    || (s == a_tag) 
	    || (s == root_tag)
	    ) 
	{
		popStack();
		return;
	}

	if  (  (s == s_tag) || (s == i_tag) 
	    || (s == f_tag) || (s == n_tag)
		|| (s == b_tag)
		) 
	{
		return;
	}

    if ( (s == pdoc_tag)) 
    {
    	done_ = true;
    	return;
    }

	zval_user otag(htab_read(tag_objs_).get(tag));

	if (otag.isString())
	{
		popStack();
	}
	else {
		zend_throw_error(zend_ce_error,"tag end %s unmatched", tag.data());
		done_ = true;
	}
	return;

}

void Wcc_XmlRead::pushTable(int kind, zstr_user key) 
{
	//Php::out << "pushTable: " << kind << " " << key << std::endl;
	//zend_printf("Push Table\n");
	zval_mgr data;

	data.new_array();

	//zend_printf("Pushtable at size = %ld, kind = %ld\n", ix, kind);
	attach_ds(new DStack(key, data, kind));

	//showmem("new array", data);
}

void Wcc_XmlRead::throwKey(zstr_user key)
{
	zend_throw_error(zend_ce_error, "Key not allowed here: %s", key.data());
	done_ = true;
}

void Wcc_XmlRead::throwNoKey()
{
	zend_throw_error(zend_ce_error, "Need a key here k=??", 0);
	done_ = true;
}

void Wcc_XmlRead::debug_info(htab_write s)
{
	//zval_mgr temp(xml_);
	s.set( XML_FNS.reader, xml_);

	zval_mgr temp;

	if (top_) {
		temp = top_->ref_;
	}
	s.set( XML_FNS.root, temp);
	s.set( XML_FNS.tags, tag_objs_);
}

void Wcc_XmlRead::setValue(const zval_mgr& value,  zstr_user key)
{
	auto ix = stacked_;
	if (ix == 0) {
		zend_throw_error(zend_ce_error,"setValue on empty stack");
		done_ = true;
		return;
	}
	
	DStack *ds = top_;
	zval_user  ref(ds->ref_);

	switch(ds->kind_) {
	case XC_OBJECT: // current anchor is object, so assign property
		{
			if (key.isNull()) {
				throwNoKey();
			}
			zobj_user obj(ref.zobject());
			// property style set
			obj.property(key, value); 
		}
		break;
	case XC_TABLE: // current anchor is  Array using associative keys
		{
			if (key.isNull()) 
			{
				throwNoKey();
			}

			htab_write hw(ref);
			hw.set(key, value);
		}
		break;
	case XC_ARRAY: 
		{
			// current anchor is  Array, with only appended integer index
			if (!key.isNull()) 
			{
				throwKey(key);
			}
			htab_write hw(ref);
			hw.push_back(value);
		}
		break;
	}
}

void Wcc_XmlRead::setInteger(zstr_user key)
{
	 zval_mgr ival = xml_.xml_str_zval();
	 ival.toLong();
	 setValue(ival, key);
	 nextEnd();
}

void Wcc_XmlRead::setFloat(zstr_user key)
{
	 zval_mgr fval = xml_.xml_str_zval();
	 fval.toDouble();
	 setValue(fval, key);
	 nextEnd();
}

void Wcc_XmlRead::setNull(zstr_user key)
{
	//zval_mgr nval = xml_.xml_str_zval();
	setValue(xml_.xml_str_zval(), key);
	nextEnd();
}

void Wcc_XmlRead::setString(zstr_user key)
{
	//zval_mgr sval = xml_.xml_str_zval();
	//showmem("setString", sval);
	setValue(xml_.xml_str_zval(), key);
	nextEnd();
}

void 
Wcc_XmlRead::setEmptyArray(zstr_user key)
{
	zval_mgr array;
	array.empty_array();
	setValue(array, key);
	nextEnd();
}

void Wcc_XmlRead::setBool(zstr_user key)
{
	zval_mgr data(xml_.xml_str_zval());

	data.toString();

	zstr_user su(zval_user(data).zstr());
	zstr_mgr  lcstr = su.to_lower();
	su = lcstr;

	auto c = su.vstr();

	zval_mgr bval = (c == "1" || c == "true" || c == "y") ? true : false;
	setValue(bval, key);
	nextEnd();
}

void Wcc_XmlRead::setDateTime(zstr_user key) 
{
	zval_mgr arg = xml_.xml_str_zval();

	datetime_obj obj_val(arg);

	//showobj("datetime", obj_val);
	zval_mgr zw(obj_val);

	setValue(zw, key);

	nextEnd();
}

bool Wcc_XmlRead::tag_start(
	  zstr_user tag, 
	  zstr_user key)
{
	std::string_view s = tag.vstr();
	if (s == root_tag)
	{
		pushRoot(key);
	}
	else if (s == tb_tag)
	{
		pushTable(XC_TABLE, key);
	}
	else if (s == a_tag)
	{
		pushTable(XC_ARRAY, key);
	}
	else if (s == i_tag)
	{
		setInteger(key);
	}
	else if (s == ea_tag)
	{
		setEmptyArray(key);
	}
	else if (s == n_tag)
	{
		setNull(key);
	}
	else if (s == s_tag)
	{
		setString(key);
	}
	else if (s == b_tag)
	{
		setBool(key);
	}
	else if (s == f_tag)
	{
		setFloat(key);
	}
	else if (s == dtm_tag)
	{
		setDateTime(key);
	}
	else if (s == xtag_tag)
	{
		tagsTable();
	}
	else if (s == pdoc_tag)
	{
	}
	else {
		return false;
	}
	return true;
}

void Wcc_XmlRead::nextEnd() {
	while(xml_.read())
	{
		if (xml_.nodeType() == Xntype::END_ELEMENT)
		{
			return;
		}
	}

}

ZEND_METHOD(Wcc_XmlRead, fromString)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();

	zval_mgr result = Wcc_XmlRead::fromString(cname);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_XmlRead, parse)
{
	zend_string* src;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(src)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_XmlRead>(ZEND_THIS);
	zval_mgr result = cobj->parse(src);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_XmlRead, fromFile)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();

	zval_mgr result = Wcc_XmlRead::fromFile(cname);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_XmlRead, parseFile)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_XmlRead>(ZEND_THIS);
	zval_mgr result = cobj->parseFile(cname);
	//showmem("parseFile result", result);
	result.move_zv(return_value);
}

PHP_MINIT_FUNCTION(Wcc_XmlRead_reg)
{
	zend_class_entry* ce = register_class_Wcc_XmlRead();

	Wcc_XmlRead::omg.classEntry(ce);

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(Wcc_XmlRead_d)
{

	return SUCCESS;
}
//xmlread.cpp
#endif