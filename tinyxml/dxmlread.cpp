#ifndef DXMLREAD_CPP
#define DXMLREAD_CPP


#ifndef DXMLREAD_H
#include "dxmlread.h"
#endif

// custom new, new[], delete, delete[]
/* #ifndef ALLOC_NEW_CPP
// These fail to "replace" the standard versions,
// So cannot be used to redirect allocations to emalloc
#include "zpp/alloc_new.cpp"
#endif
*/


// only place to include tinyxml2.cpp
#include "tinyxml2.cpp"

#ifndef REFLECT_CACHE_H
#include "wcc/reflect_cache.h"
#endif


extern "C" {
	#include "stub/xmlread_arginfo.h"
};


namespace zpp {
	class xml_fns : public state_init {
    public:

    	//str_intern   	  xmlreader;
    	//str_intern   	  fromString;
    	//str_intern   	  open;
        //str_intern    	  get_attribute;
        //str_intern    	  read_string;
        //str_intern    	  read;

		//str_intern 	  	  k_nodeType;
		//str_intern	      k_attribute;
		//str_intern	      k_name;
		
		str_intern 	  	  k_c;
		str_intern	      k_k;

		//str_intern	      k_close;

		//str_intern       reader;

		str_intern       root;

		str_intern       tags;
		
		xml_fns();
		
        virtual void init();
    };

	xml_fns XML_FNS;

	xml_fns::xml_fns() : state_init()
	{

	}

	void xml_fns::init()
	{
		//xmlreader = "xmlreader";
		//fromString = "xmlreader::fromstring";
		//open = "xmlreader::open";

		//get_attribute = "getattribute";
        //read_string = "readstring";
        //read = "read";

		//k_nodeType = "nodeType";//property name
		//k_name = "name";
		k_c = "c";
		k_k = "k";
		//k_close = "close";	

		//xml_file.set_fname(open);
		//xml_parse.set_fname(fromString);

		//reader = "reader";	
		root = "root";	
		tags = "tags";	
	}

};

namespace wcc {
	using namespace zpp;
	using namespace tinyxml2;

	base_obj_mgr<Wcc_XmlRead> Wcc_XmlRead::omg;



	XmlWrap::XmlWrap() : 
		xele_(nullptr)
		, fileOpen_(false)
	{
		/* getNameValue_ = (xr_strfn) xmlTextReaderConstName;
		getAttrValue_ = (xr_attrfn) xmlTextReaderGetAttribute;
		getStrValue_ = (xr_strfn) xmlTextReaderReadString;
		nextNode_ = (xr_intfn) xmlTextReaderRead;
		nodeType_ = (xr_intfn) xmlTextReaderNodeType;
		*/
	}


	
	str_rc //static
	XmlWrap::get_valid_file_path(str_ptr src) 
	{	
		return src;
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
			xdoc_.Clear();
			fileOpen_ = false;
		}
	}
	

	bool XmlWrap::fromString(str_ptr xml)
	{
		hold_ = xml;
		xele_ = nullptr;

		XMLError error = xdoc_.Parse(xml.data(), xml.size());

		//zend_printf("fromString %d\n",error);
		fileOpen_ = (error == XML_SUCCESS);
		if (fileOpen_)
		{
			visit_ = V_DOCUMENT;
		}
		else {
			visit_ = V_END_DOCUMENT;
		}
		

		return fileOpen_;
	}
	
	bool XmlWrap::fromFile(str_ptr path)
	{
		hold_ = XmlWrap::get_valid_file_path(path);
		xele_ = nullptr;

		XMLError error = xdoc_.LoadFile(path.data());
		//zend_printf("fromFile %d\n",error);
		fileOpen_ = (error == XML_SUCCESS);
		if (fileOpen_)
		{
			visit_ = V_DOCUMENT;
		}
		else {
			visit_ = V_END_DOCUMENT;
		}

		return fileOpen_;
	}

	val_rc XmlWrap::xml_name_zval()
	{
		val_rc result;
		
		if (xele_)
		{
			str_rc test(xele_->Value());
			result = test;
		}

		return result;
	}

	str_rc XmlWrap::xml_name()
	{
		str_rc result;

		if (xele_)
		{
			result = xele_->Value();
		}
		return result;
	}

	str_rc
	XmlWrap::get_attribute(str_ptr name)
	{	
		str_rc result;

		if (xele_)
		{
			const XMLAttribute* a = xele_->FindAttribute(name.data());
			if (a)
			{
				result = a->Value();
			}
		}
		return result;
	}

	val_rc
	XmlWrap::xml_str_zval()
	{
		val_rc result;
		if (xele_)
		{
			const XMLNode* node = xele_->FirstChild();
			str_buf buf;
			while (node) 
			{
				if (node->ToText())
				{
					buf << node->Value();
				}
				node = node->NextSibling();
			}
			result = buf.zstr();
		}
		return result;
	}

	static XMLElement* firstElementNode(XMLNode* node)
	{
		XMLElement* result = nullptr;
		while(node)
		{
			result = node->ToElement();
			if (result)
			{
				break;
			}
			node = node->NextSibling();
		}
		return result;
	}

	XMLElement*  
	XmlWrap::nextElement()
	{
		// Need to visit each element in the tree.
		// Depth first. 
		// Indicate start of node with
		// ect_ ==  XMLElement::OPEN
		// Indicate End of node, (No more children)
		// ect_ == XMLElement::CLOSING
		// a Node with no Children ect_ == XMLElement::CLOSED
		// visit next element node.
		// First or Next child that is Element
		// If No more children, Return parent ended.
		// If Parent ended, Next Sibling.

		XMLElement* result = nullptr;

		switch(visit_)
		{
		case Xntype::V_DOCUMENT:
			result = firstElementNode(xdoc_.FirstChild());
			if(result)
			{
				visit_ = (result->NoChildren() ? V_EMPTY : V_ELEMENT);
				return result;
			}
			break;
		case Xntype::V_ELEMENT:
			result = firstElementNode(xele_->FirstChild());
			if(result)
			{
				visit_ = (result->NoChildren() ? V_EMPTY : V_ELEMENT);
				return result;
			}
			else {
				result = xele_;
				visit_ =  V_END_ELEMENT;
			}
			break;
		case Xntype::V_EMPTY:
			{
			result = xele_;
			visit_ = V_END_ELEMENT;
			}
			break;
		case Xntype::V_END_ELEMENT:
			result = firstElementNode(xele_->NextSibling());
			if(result)
			{
				visit_ = (result->NoChildren() ? V_EMPTY : V_ELEMENT);
				return result;
			}
			else {
				result = (XMLElement*) xele_->Parent();
				visit_ = (result ? V_END_ELEMENT : V_END_DOCUMENT);
			}
			break;
		case Xntype::V_END_DOCUMENT:
			break;
		}
		return result;
	}

	bool XmlWrap::read()
	{
		xele_ = this->nextElement();

		bool result = (xele_ ? true : false);
		if (result)
		{
			//str_rc value(xele_->Value());
			//zend_printf("%lx visit %d ", (long unsigned int)xele_, visit_);
			//showstr("tag", value);
		}
		else {
			//zend_printf("ENDED\n");
		}
		return result;
	}

	// In this implementation, returns the visit_ enum value
	XmlWrap::Xntype XmlWrap::nodeType()
	{
		return visit_;
	}

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
	if ( xml_.ok())
	{
		//zend_printf("close file\n");
		xml_.closeFile();
	}
	int ct = stacked_;
	stacked_ = 0;
	//zend_printf("stacked %d\n",ct);
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

bool 
Wcc_XmlRead::openstring(str_ptr str)
{
	//zend_printf("called openstring\n");
	if (!xml_.fromString(str))
	{
		zend_throw_error(zend_ce_error,"Cannot parse");
		return false;
	}
	return true;
}

bool Wcc_XmlRead::openfile(str_ptr file)
{
	if (!xml_.fromFile(file))
	{
		zend_throw_error(zend_ce_error,"Cannot open %s for xml parse ",file.data());
		return false;
	}
	return true;
}

val_rc 
Wcc_XmlRead::parse(str_ptr src)
{
	val_rc result;
	init();

	if (!this->openstring(src))
	{
		str_rc line(src.substr(0,40));

		zend_throw_error(zend_ce_error, "XMLReader::XML fail for '%s'", line.data());
		return result;
	}
	//zend_printf("opened string stream\n");
	result = loop();
	return result;
}



val_rc //static
Wcc_XmlRead::fromString(str_ptr src)
{
	obj_rc xmlr = Wcc_XmlRead::omg.new_zobj();
	Wcc_XmlRead* cobj = zobj_toc<Wcc_XmlRead>(xmlr);
	
	return cobj->parse(src);
}

val_rc //static
Wcc_XmlRead::fromFile(str_ptr filename)
{
	obj_rc xmlr = Wcc_XmlRead::omg.new_zobj();
	//showobj("new_zobj", xmlr);
	Wcc_XmlRead* cobj = zobj_toc<Wcc_XmlRead>(xmlr);
	
	return cobj->parseFile(filename);
}

val_rc
Wcc_XmlRead::loop()
{
	val_rc result;
	done_ = false;

	result.set_bool(false);

	while(!done_ && xml_.read())
	{
		auto ntype = xml_.nodeType();

		switch(ntype) {
			case XmlWrap::V_ELEMENT:
			case XmlWrap::V_EMPTY:
			{
				str_rc tagstr = xml_.xml_name();
				//showstr("tag start", tagstr);

				str_rc attrstr = xml_.get_attribute(XML_FNS.k_k);

				//showstr("tagstr", tagstr);
				//showstr("attrstr", attrstr);

				bool result = tag_start(tagstr, attrstr);

				if (!result) {

					str_rc classname = htab_ptr(tag_objs_).get(tagstr);

					if (classname.isNull()) {
						zend_throw_error(zend_ce_exception, "Unmapped tag %s", tagstr.data());
						done_ = true;
						continue;
					}
					pushClass(classname, attrstr);
				}

			}
			break;
		case XmlWrap::V_END_ELEMENT:
			{
				str_rc tag = xml_.xml_name();
				tag_end(tag);
				//showstr("exit tag", tag);
			}
			break;
		case XmlWrap::V_END_DOCUMENT:
		case 0:
			done_ = true;
			zend_printf("End reached\n");
			break;
		}
	}
	if (root_ && !stacked_) {

		result = std::move(root_->ref_);
		delete root_;
		root_ = nullptr;
	}
	else {
		//zend_printf("root_ %lx, stacked_ %d", root_, stacked_);

		result.set_bool(false);
	}
	clean();
	return result;

}

val_rc 
Wcc_XmlRead::parseFile(str_ptr filename)
{
	str_rc  file(filename);
	val_rc  result;


	init();
	if (!openfile(file))
	{
		zend_throw_error(zend_ce_error, "XMLRead open fail for %s", filename.data());
		return result;
	}
	//zend_printf("opened file %s\n", filename.data());

	result = loop();
	//showmem("loop result", result);
	return result;
}

void 
Wcc_XmlRead::tagsTable()
{
	str_rc tag;
	val_rc value;
	//zend_printf("tagsTable\n");
	
	while(xml_.read()) {
		auto ntype = xml_.nodeType();
		switch(ntype) {
			case XmlWrap::V_ELEMENT:
			case XmlWrap::V_EMPTY:
				{
					tag = xml_.xml_name();
					value = xml_.xml_str_zval();

					//showarray("tag_objs", tag_objs_);*/
					htab_rw(tag_objs_).set(tag, value);	
				}
				break;
			case XmlWrap::V_END_ELEMENT:
				{
					tag = xml_.xml_name();
					std::string_view test = val_ptr(tag).vstr();
					//zend_printf("test vstr (%ld) %s\n", test.size(), test.data());
					//showmem("tagkey end", tag);
					if (test == xtag_tag) {
						return;
					}
				}
				break;	
			default:
				break;		
		}
	}
}

obj_rc
Wcc_XmlRead::newRoot(str_ptr cname)
{

	obj_rc result;

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
Wcc_XmlRead::pushRoot(str_ptr key)
{
	str_rc cname = xml_.get_attribute(XML_FNS.k_c);

	val_rc newroot;
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

void Wcc_XmlRead::pushClass(str_ptr classname, str_ptr key)
{

	obj_rc newroot = ReflectCache::staticInstance(classname);

	//showobj("newroot", newroot);

	val_rc store(newroot);

	attach_ds(new DStack(key, store, XC_OBJECT));
}

void Wcc_XmlRead::popStack()
{
	auto ct = stacked_;
	

	if (ct > 0) {
		ct--;

		DStack* last = top_;
		top_ = last->ds_prev_;
		stacked_ = ct;

		
		if (ct > 0) {
			//showmem("last ref", last->ref_);
			//showstr("last key", last->key_);

			// set value uses current top stack member
			//showmem("last->ref_", last->ref_);

			setValue(last->ref_, last->key_);	
			//showmem("last->ref", last->ref_);

			delete last;		
		}
		else {
			//zend_printf("popStack %ld\n", ct);
			done_ = true;
			// keep root_ around a bit longer
		}
	}
	else {
		zend_throw_error(zend_ce_error,"Pop with empty element stack");
		done_ = true;
	}
	//zend_printf("exit popStack\n");
}

void Wcc_XmlRead::tag_end(str_ptr tag)
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

    /* if ( (s == pdoc_tag)) 
    {
    	done_ = true;
    	return;
    }
    */

	val_ptr otag(htab_ptr(tag_objs_).get(tag));

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

void Wcc_XmlRead::pushTable(int kind, str_ptr key) 
{
	
	//zend_printf("Push Table kind = %d\n", kind);
	val_rc data;

	data.new_array(); //refcount == 2

	//showmem("new array", data);

	//zend_printf("Pushtable at size = %ld, kind = %ld\n", ix, kind);
	attach_ds(new DStack(key, data, kind));
}

void Wcc_XmlRead::throwKey(str_ptr key)
{
	zend_throw_error(zend_ce_error, "Key not allowed here: %s", key.data());
	done_ = true;
}

void Wcc_XmlRead::throwNoKey()
{
	zend_throw_error(zend_ce_error, "Need a key here k=??");
	done_ = true;
}

void Wcc_XmlRead::debug_info(htab_rw s)
{
	val_rc temp;

	if (top_) {
		temp = top_->ref_;
	}
	s.set( XML_FNS.root, temp);
	s.set( XML_FNS.tags, tag_objs_);
}

void Wcc_XmlRead::setValue(val_ptr value,  str_ptr key)
{
	auto ix = stacked_;
	if (ix == 0) {
		zend_throw_error(zend_ce_error,"setValue on empty stack");
		done_ = true;
		return;
	}
	DStack *ds = top_;

	//zend_printf("Stacked %d at %lx, kind %d\n", ix, ds, ds->kind_);
	val_ptr  ref(ds->ref_);
	

	switch(ds->kind_) 
	{
	case XC_OBJECT: // current anchor is object, so assign property
		{
			if (key.isNull()) {
				throwNoKey();
			}
			obj_ptr obj(ref.zobject());
			// property style set

			obj.property(key, value); 
			// this is the last time this pvalue is seen, after being
			// referenced in the property.
			// dereference now?
			/*if (value.isArray())
			{
				HashTable *ht = value.zarray();

				//showarray("after set property array", ht);
			}*/
			

		}
		break;
	case XC_TABLE: // current anchor is  Array using associative keys
		{
			if (key.isNull()) 
			{
				throwNoKey();
			}
			
			htab_rw hw(ref);
			//showstr("set array key", key);
			//showmem("value", value);

			hw.set(key, value);
			//showarray("set array ht", hw);
			//showmem("set array ref", ref);
		
		}
		break;
	case XC_ARRAY: 
		{
			// current anchor is  Array, with only appended integer index
			if (!key.isNull()) 
			{
				throwKey(key);
			}
			//showmem("ref", ref);
			htab_rw hw(ref);
			//showarray("array write", hw);
			//showmem("push value", value);

			hw.push_back(value);
		}
		break;
	}
}

void Wcc_XmlRead::setInteger(str_ptr key)
{
	 val_rc ival = xml_.xml_str_zval();
	 ival.toLong();
	 setValue(ival, key);
	 nextEnd();
}

void Wcc_XmlRead::setFloat(str_ptr key)
{
	 val_rc fval = xml_.xml_str_zval();
	 fval.toDouble();
	 setValue(fval, key);
	 nextEnd();
}

void Wcc_XmlRead::setNull(str_ptr key)
{
	//val_rc nval = xml_.xml_str_zval();
	setValue(xml_.xml_str_zval(), key);
	nextEnd();
}

void Wcc_XmlRead::setString(str_ptr key)
{
	val_rc sval = xml_.xml_str_zval();
	//showmem("setString", sval);
	setValue(sval, key);
	nextEnd();
}

void 
Wcc_XmlRead::setEmptyArray(str_ptr key)
{
	val_rc array;
	array.new_array();
	setValue(array, key);
	nextEnd();
}

void Wcc_XmlRead::setBool(str_ptr key)
{
	val_rc data(xml_.xml_str_zval());

	data.toString();

	str_rc su(val_ptr(data).zstr());
	su.lowercase();

	auto c = su.vstr();

	val_rc bval = (c == "1" || c == "true" || c == "y") ? true : false;
	setValue(bval, key);
	nextEnd();
}

void Wcc_XmlRead::setDateTime(str_ptr key) 
{
	val_rc arg = xml_.xml_str_zval();


	datetime_obj obj_val(val_ptr(arg).zstr());

	//showobj("datetime", obj_val);
	val_rc zw(obj_val);

	setValue(zw, key);

	nextEnd();
}

bool Wcc_XmlRead::tag_start(
	  str_ptr tag, 
	  str_ptr key)
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
		switch(xml_.nodeType())
		{
			case XmlWrap::V_END_ELEMENT:
				return;
			default:
				break;
		}
	}
}

Wcc_XmlRead::DStack::~DStack()
{
	//showmem("~Stack ref", ref_);
}

Wcc_XmlRead::DStack::DStack(str_ptr k, const val_rc& val, int eval)
         : kind_(eval) 
{
	key_ = k;
	ref_ = val;
	//showstr("+Stack key", key_);
	//showmem("+Stack ref", ref_);
}

}; // namespace wcc

using namespace wcc;

ZEND_METHOD(Wcc_XmlRead, fromString)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();

	val_rc result = Wcc_XmlRead::fromString(cname);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_XmlRead, parse)
{
	zend_string* src;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(src)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_XmlRead>(ZEND_THIS);
	val_rc result = cobj->parse(src);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_XmlRead, fromFile)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();

	val_rc result = Wcc_XmlRead::fromFile(cname);
	result.move_zv(return_value);
}

ZEND_METHOD(Wcc_XmlRead, parseFile)
{
	zend_string* cname;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(cname)
	ZEND_PARSE_PARAMETERS_END();

	auto cobj = zval_toc<Wcc_XmlRead>(ZEND_THIS);
	val_rc result = cobj->parseFile(cname);
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