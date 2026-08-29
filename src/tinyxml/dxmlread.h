#ifndef DXML_READ_H
#define DXML_READ_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif

#include <new>

#ifndef FN_CALL_H
#include "zpp/fn_call.h"
#endif

#ifndef ALLOC_PHPREQ_H
#include "zpp/alloc_phpreq.h"
#endif

#ifndef TINYXML2_INCLUDED
#include "tinyxml2.h"
#endif



namespace wcc {
	//using xmlstr = zstr_own;
	using namespace zpp;
	using namespace tinyxml2;

	class xml_fns : public state_init {
	public:
		str_intern  	  makeclass_s;
		str_intern 	  	  k_c;
		str_intern	      k_k;
		str_intern       root;
		str_intern       tags;

		void init() override;
	};

	extern xml_fns  XML_FNS;
	/**
	 * intended for single parse.
	 * mixed in with Wcc_XmlRead
	 */


	/* typedef const xmlChar* (*xr_strfn)(xmlTextReaderPtr);
	typedef long     (*xr_intfn)(xmlTextReaderPtr);
	typedef const xmlChar* (*xr_attrfn)(xmlTextReaderPtr, const xmlChar*);
	*/

	class  XmlWrap
	{
	public:
		enum Xntype {
			V_DOCUMENT = 0,
			V_ELEMENT = 1,
			V_EMPTY = 2, // empty element
			V_END_ELEMENT = 3,
			V_END_DOCUMENT = 4
		};
	protected:
		XMLDocument              xdoc_;
		XMLElement*              xele_;
		
		// Visit status for elements
		Xntype   visit_; //


		bool      				fileOpen_;
		str_rc  				hold_; // filename or xml data

		/*
		xr_strfn				getStrValue_;
		xr_strfn				getNameValue_;
		xr_attrfn				getAttrValue_;
		xr_intfn                nextNode_;
		xr_intfn                nodeType_;
		*/
	public:
		

		XmlWrap();
		~XmlWrap();

		static str_rc get_valid_file_path(str_ptr src);

		bool_return fromFile(str_ptr path);
		bool_return fromString(str_ptr xml);


		str_rc  xml_name();

		str_rc  get_attribute(str_ptr name);
		//str_rc  xml_string();

		val_rc  xml_name_zval();
		val_rc  xml_str_zval();

		XMLElement*  nextElement();
		
		bool      read();
		
		XmlWrap::Xntype	  nodeType();
		void      closeFile();

		bool      ok() {
			return fileOpen_;
		}

		operator obj_ptr* () {
			return (obj_ptr*)(this);
		}

	};

	class  Wcc_XmlRead : public base_d {
	protected:
		enum {
			XC_EMPTY,
			XC_OBJECT, // object 
			XC_TABLE, // indexed array
			XC_ARRAY // packed array
		};
		fn_call	mkclass_fn_;
	public:

		static base_obj_mgr<Wcc_XmlRead> omg;


		static val_return fromFile(str_ptr file);

		static val_return fromString(str_ptr src);
		
		class DStack : public PHPAlloc {
		protected:
			DStack*   ds_next_;
			DStack*   ds_prev_;

			friend class Wcc_XmlRead;
		public:

			val_rc  ref_; // storage always a zval
			str_rc  key_; // key must always be a string
			int       kind_; // object / packed array / keyed array


			DStack() : ds_next_(nullptr), ds_prev_(nullptr),kind_(XC_EMPTY) {}

			~DStack();

			DStack(str_ptr k, const val_rc& val, int eval);

			/*DStack& operator=(DStack&& m)
			{
				ref_ = std::move(m.ref_);
				key_ = std::move(m.key_);
				kind_ = m.kind_;
				return *this;
			}
			*/
			
			bool empty() const {
				return (kind_ == XC_EMPTY);
			}

		};

	protected:

		htab_rc  class_replace_; // classname key to replace classname
		obj_rc   addRoot_; /* preinstalled root by constructor? */

		htab_rc  tag_objs_; /* Array of tag name - class name */

		/** cache zend_string  used for repeated property/call access */
			
		XmlWrap xml_; // derived from obj_rc, adopts XmlReader
		bool done_; // or error condition

		/** PHP file handle returned by XMLReader::open */
		
		/** flag for finish the parse */

		DStack 	*root_;
		DStack  *top_;
		size_t  stacked_;

		void	classReplace(htab_ptr cnames);
		

		bool 	tag_start(str_ptr tag, str_ptr val);
		void 	tag_end(str_ptr tag);

		void 	throwKey(str_ptr key);
		void 	throwNoKey();

		void    nextEnd();


		void	tagsTable();
		void 	attach_ds(DStack* ds);

		

		void       pushRoot(str_ptr classname);
		void       pushClass(str_ptr classname, str_ptr val);
		void 	   pushTable(int kind, str_ptr val);

		obj_rc    newRoot(str_ptr classname);
		void      popStack();

		void      setEmptyArray(str_ptr key);

		void      setValue(val_ptr val, str_ptr key);

		void      setBool(str_ptr key);
		void      setInteger(str_ptr key);
		void      setFloat(str_ptr key);
		void      setArray(str_ptr key);
		void      setString(str_ptr key);
		void      setDateTime(str_ptr key);
		void      setNull(str_ptr key);

		void      setKeys();

		bool_return 	  openfile(str_ptr file);
		bool_return       openstring(str_ptr src);

		void      init();
		void	  clean();
		
		val_return  loop();

	public:

		void construct(obj_ptr obj);
		void destruct();
		

		Wcc_XmlRead();
		virtual ~Wcc_XmlRead();

		/** return PHP array version of xml data */
		val_return parseFile(str_ptr filename);
		val_return parse(str_ptr src);
		obj_return	makeClass(str_ptr classname);
		
		virtual void debug_info(htab_rw ht);
	
		VIRTUAL_ZOBJPTR
	
	};


};//namespace
//xmlread.h
#endif