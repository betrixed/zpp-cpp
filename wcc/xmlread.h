#ifndef XML_READ_H
#define XML_READ_H


#ifndef ZPP_BASE_H
#include "zpp/base.h"
#endif


#ifndef FN_CALL_H
#include "zpp/fn_call.h"
#endif

namespace zpp {
		/**
		fn_call_args<1> getattribute_;
		fn_call readstring_;
		fn_call read_;
		*/

	class xmlreader_open : public fn_call_args<3> {
    public:
        obj_rc call(str_ptr path);
    };

    class xmlreader_xml : public fn_call_args<3> {
    public:
        obj_rc call(str_ptr src);
    };
    

    class xml_fns : public state_init {
    public:
    	xmlreader_open    xml_file;
    	xmlreader_xml	  xml_parse;

    	zstr_intern   	  xmlreader;
    	zstr_intern   	  fromString;
    	zstr_intern   	  open;
        zstr_intern    	  get_attribute;
        zstr_intern    	  read_string;
        zstr_intern    	  read;

		zstr_intern 	  k_nodeType;
		zstr_intern	      k_attribute;
		zstr_intern	      k_name;
		
		zstr_intern 	  k_c;
		zstr_intern	      k_k;

		zstr_intern	      k_close;

		zstr_intern       reader;

		zstr_intern       root;

		zstr_intern       tags;
		
		xml_fns();
		
        virtual void init();
    };



    extern xml_fns  XML_FNS;
};

namespace wcc {
	//using xmlstr = zstr_own;
	using namespace zpp;

	/**
	 * intended for single parse.
	 * mixed in with Wcc_XmlRead
	 */

	class  XmlWrap
	{
	protected:
		fn_call_args<1> getattribute_;
		fn_call   readstring_;
		fn_call   read_;
		bool      fileOpen_;
		str_rc  hold_; // filename or xml data
		obj_rc  self_; // XmlReader::Object

		bool adopt_xmlobj(obj_rc& test);
	public:
		XmlWrap();
		~XmlWrap();

		bool fromFile(str_ptr path);
		bool fromString(str_ptr xml);
		void fn_setup();

		str_rc  xml_name();

		str_rc  get_attribute(str_ptr name);
		//str_rc  xml_string();

		val_rc  xml_name_zval();
		val_rc  xml_str_zval();

		bool      read();
		int		  nodeType();
		void      closeFile();

		bool      ok() {
			return self_.ok();
		}

		obj_ptr xml() 
		{
			return self_;
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

	public:

		static base_obj_mgr<Wcc_XmlRead> omg;


		static val_rc fromFile(str_ptr file);

		static val_rc fromString(str_ptr src);
		
		class DStack {
		protected:
			DStack*   ds_next_;
			DStack*   ds_prev_;

			friend class Wcc_XmlRead;
		public:

			void* operator new(size_t size)
			{
				//zend_printf("new DStack %ld\n",size);
				return emalloc(size);
			}

			void operator delete(void* ptr)
			{
				//zend_printf("delete DStack %lx\n",ptr);
				efree(ptr);
			}

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


		obj_rc   addRoot_; /* preinstalled object root ? */

		htab_rc  tag_objs_; /* Array of tag name - class name */

		/** cache zend_string  used for repeated property/call access */
			
		XmlWrap xml_; // derived from obj_rc, adopts XmlReader
		bool done_; // or error condition

		/** PHP file handle returned by XMLReader::open */
		
		/** flag for finish the parse */

		DStack 	*root_;
		DStack  *top_;
		size_t  stacked_;

		bool 	tag_start(str_ptr tag, str_ptr val);
		void 	tag_end(str_ptr tag);

		void 	throwKey(str_ptr key);
		void 	throwNoKey();

		void    nextEnd();


		void	tagsTable();
		void 	attach_ds(DStack* ds);

		void    pushRoot(str_ptr classname);
		void       pushClass(str_ptr classname, str_ptr val);
		void 	   pushTable(int kind, str_ptr val);

		obj_rc  newRoot(str_ptr classname);
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

		bool 	  openfile(str_ptr file);
		bool      openstring(str_ptr src);

		void      init();
		void	  clean();
		
		val_rc  loop();

	public:
		Wcc_XmlRead();
		virtual ~Wcc_XmlRead();

		/** return PHP array version of xml data */
		val_rc parseFile(str_ptr filename);
		val_rc parse(str_ptr src);

		virtual void debug_info(htab_wr ht);
	
		VIRTUAL_ZOBJPTR
	
	};


};//namespace
//xmlread.h
#endif