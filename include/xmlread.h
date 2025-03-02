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
        zobj_mgr call(zstr_user path);
    };

    class xmlreader_xml : public fn_call_args<3> {
    public:
        zobj_mgr call(zstr_user src);
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
	 * intended for single parse, throw away use.
	 */

	class  XmlWrap : public zobj_mgr 
	{
	protected:
		fn_call_args<1> getattribute_;
		fn_call   readstring_;
		fn_call   read_;
		bool      fileOpen_;
		zstr_mgr  hold_;
		zobj_user self_;

		bool adopt_xmlobj(zobj_mgr& test);
	public:
		XmlWrap();
		~XmlWrap();

		bool fromFile(zstr_user path);
		bool fromString(zstr_user xml);
		void fn_setup();
		bool newobj();

		zstr_mgr  xml_name();

		zstr_mgr  get_attribute(zstr_user name);
		//zstr_mgr  xml_string();

		zval_mgr  xml_name_zval();
		zval_mgr  xml_str_zval();

		bool      read();
		int		  nodeType();
		void      closeFile();

		operator zobj_user* () {
			return (zobj_user*)(this);
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


		static zval_mgr fromFile(zstr_user file);

		static zval_mgr fromString(zstr_user src);
		
		class DStack {
		protected:
			DStack*   ds_next_;
			DStack*   ds_prev_;

			friend class Wcc_XmlRead;

			

		public:

			zval_mgr  ref_; // storage always a zval
			zstr_mgr  key_; // key must always be a string
			int       kind_; // object / packed array / keyed array


			DStack() : kind_(XC_EMPTY), ds_next_(nullptr), ds_prev_(nullptr) {}

			~DStack()
			{
				//showstr("~Stack key", key_);
				//showmem("~Stack ref", ref_);
			}
			DStack(zstr_user k, const zval_mgr& val, int eval)
			         : ref_(val), key_(k), kind_(eval) 
			{
				//showstr("+Stack key", key_);
				//showmem("+Stack ref", ref_);
			}

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


		zobj_mgr   addRoot_; /* preinstalled object root ? */

		htab_init  tag_objs_; /* Array of tag name - class name */

		/** cache zend_string  used for repeated property/call access */
			
		XmlWrap xml_; // derived from zobj_mgr, adopts XmlReader
		bool done_; // or error condition

		/** PHP file handle returned by XMLReader::open */
		
		/** flag for finish the parse */

		DStack 	*root_;
		DStack  *top_;
		size_t  stacked_;

		bool tag_start(zstr_user tag, zstr_user val);
		void tag_end(zstr_user tag);

		void throwKey(zstr_user key);
		void throwNoKey();

		void       nextEnd();


		void	   tagsTable();
		void 	   attach_ds(DStack* ds);

		void       pushRoot(zstr_user classname);
		void       pushClass(zstr_user classname, zstr_user val);
		void 	   pushTable(int kind, zstr_user val);

		zobj_mgr  newRoot(zstr_user classname);
		void      popStack();

		void      setEmptyArray(zstr_user key);

		void      setValue(const zval_mgr& val, zstr_user key);

		void      setBool(zstr_user key);
		void      setInteger(zstr_user key);
		void      setFloat(zstr_user key);
		void      setArray(zstr_user key);
		void      setString(zstr_user key);
		void      setDateTime(zstr_user key);
		void      setNull(zstr_user key);

		void      setKeys();

		bool 	  openfile(zstr_user file);
		bool      openstring(zstr_user src);

		void      init();
		void	  clean();
		
		zval_mgr  loop();

	public:
		Wcc_XmlRead();
		virtual ~Wcc_XmlRead();

		/** return PHP array version of xml data */
		zval_mgr parseFile(zstr_user filename);
		zval_mgr parse(zstr_user src);

		void debug_info(htab_write ht) override;
	};


};//namespace
//xmlread.h
#endif