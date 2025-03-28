#ifndef ZPP_BASE_H
#define ZPP_BASE_H
//wc_base.h

#ifndef ZPP_EXPORT
#define ZPP_EXPORT
#endif

#include "php.h"

#include <zend_types.h>
#include <Zend/zend_exceptions.h>

#include <string> 

#include "show_zpp.h"

#include "zstr_mgr.h"
#include "zstr_user.h"

#include "zobj_mgr.h"
#include "zobj_user.h"

#include "zval_mgr.h"
#include "zval_user.h"

#include "htab_mgr.h"
#include "htab_read.h"
#include "htab_write.h"

#include "htab_walk.h"
#include "state_init.h"
#include "class_data.h"

#include "zstr_buffer.h"

#include "datetime.h"

#include "preg.h"

#include <stdexcept>

#ifdef DEBUG_EXTRA
//#define BASE_DEBUG
#endif


//#define BASE_ZOBJPTR

#ifndef BASE_ZOBJPTR
/* 
	This is virtual because problem of base class needs
	to use the sizeof(*this) of enclosing class.
*/
#define VIRTUAL_ZOBJPTR virtual zend_object* vobj() const \
	{ return (zend_object*) ((char*)(this) + sizeof(*this) + sizeof(char*)); }
#else
#define VIRTUAL_ZOBJPTR
#endif


namespace zpp {

	/**
	 * All base_d objects have a memory overhead - 3 x pointers 
	 * 	Of course a C++ virtual function table pointer (base virtual destructor)
	 *  zend_object* as first member of T*.
	 *  Pointer to *T as first negative offset from zend_object.
	 * 
	 * Base class for all C++ objects implemented here.
	 * Each class should have its own base_obj_mgr<T>,
	 * which holds its own static storage for zend_class_entry*, and
	 * zend_object_handlers. Relies on one static instance of each base_obj_mgr<T>
	 * being created in the .cpp implementation, as declared in the .h file.
	 * 
	 * Its possible, but have not tried, to create non-zobject associated 
	 * (with nullptr for zend_object*)
	 * using a different C++ new allocator. This won't be able to use 
	 * zend_object properties, or management functions.
	 * 
	 * 
	 * How to implement internal subclasses?
	 * This naive plan is :-
	 * 
	 *     Presume that the internal arrangement, will be
	 *  
	 * 	   class [baseclass] : public base_d {}
	 * 
	 *     class [subclass] : public [baseclass] {}
	 * 
	 *     In the subclass.php.stub -- declare all the public
	 *            available methods additional to the subclass
	 * 
	 * 	   		  and register the [subclass] as an instance of [baseclass]
	 * 			  by adding its parent zend_class_entry
	 * 
	 * 	   Only need to re-register any overriding versions of functions.
	 * 
	 * 	   The subclass will have its own base_obj_mgr<[subclass]>.
	 * 
	 * 	   A base_obj_mgr<[baseclass]> exists, as declared for baseclass,
	 *     and will still be used by zend for the parent class entry, functions,
	 *     and even property storage or its overrides. The methods will now 
	 *     access the p_zobj of [subclass].  
	 * 
	 * 	   The internal memory block allocated for each T is
	 * 	   
	 * 	   T itself, starting at offset zero.
	 * 	   At address of sizeof(T)+1  is stored pointer to beginning of memory block of T
	 *     At address of sizeof(T)+2  is beginning of the  zend_object std structure, 
	 * 	   To get the *T from zend_object* , subtract sizeof(*T).
	 * 
	 * 	   This follows a recommendation I read, that internal extra stuff be put at negative offsets from the zend_object. 
	 * 		
	 * 	   This is done in base_obj_mgr<T>::cpp(zend_object*)
	 *     As a check, the value of the same zend_object* should match that stored at the beginning of T.
	 * 	   It also makes it easy for all base_d derived objects to pass their own zend_object* around.
	 * 
	 * 	   If a baseclass is called with a subclass zend_object, it will have code
	 *     that calls its baseclass base_obj_mgr<baseclass>::cpp(zend_object*)  This function will still do the right thing,
	 *     it looks at the zend_object address, subtracts the sizeof(base_d*), and finds the base address of own members,
	 * 	   and checks that the zend_object* is equal. The address offset to (base_d*) will no longer be sizeof(baseclass) is not a problem.
	 * 
	 * 	   T object (includes zend_object* at offset 0)
	 * 	   *T  - address of T object
	 *     zend_object.   
	 * 		
	 * 	   This is in the base_d::operator new() zend_object_allocate asks for - sizeof(T) + sizeof(*T) + sizeof(zend_object)
	 *     called in base_obj_mgr<T>::znew_ex
	 * 
	 * 	   Other details of inheritance to cater for, include calling virtual functions of parent class where properly useful,
	 *     eg call parent virtual debug_info, or  the function handling constructor, internal function of parent.
	 */


	class   base_d {
	#ifdef BASE_ZOBJPTR
	protected:
		zend_object* p_zobj_;
	#endif
	public:
		#ifdef BASE_ZOBJPTR
		base_d() : p_zobj_(nullptr) {}
		#else
		base_d() {}
		#endif

		virtual ~base_d() 
		{
			#ifdef BASE_DEBUG
				zend_printf("~base_d() %lx\n", this);
			#endif
			// unattached have no zend_object*
			#ifdef BASE_ZOBJPTR
			if (p_zobj_) {
				#ifdef BASE_DEBUG
				zend_printf("dtor zend object %lx\n", p_zobj_);
				#endif
				//zend_object_std_dtor(p_zobj_);
			}
			p_zobj_ = nullptr;
			#endif
		}

		#ifdef BASE_ZOBJPTR
		//! Memory vs Time-Indirection trade off. Stored pointer or call function to calculate it.
		zend_object* vobj() const { return p_zobj_; }
		void set_zobj(zend_object* zo) { p_zobj_ = zo; }
		#else 
		// This needs to be redeclared in every class
		/* virtual zend_object* zobj() const {
			return (zend_object*) ((char*)(this) + sizeof(*this) + sizeof(char*));
		}*/
		VIRTUAL_ZOBJPTR
		#endif

		/**
		 * Assumptions of memory layout. 
		 * Expect zend_object pointer p_zobj_
		 * at zero offset of base_d pointer.
		 * at (base_d* + 1) is address of base_d, ie pointer to self.
		 * at (self* + 1) is storage of the zend_object.
		 * This is to allow pointer to self to be easily
		 * obtained from a zend_object* as (zend_object* - 1).
		 * A memory sanity check is then self->z_obj_ is equal to zend_object*.
		 * This seems unlikely to be true for zend_object* created elsewhere.
		 * 
		 */ 
		void* operator new(std::size_t msize,  zend_class_entry *class_type) 
		{
			return zend_object_alloc(msize + sizeof(base_d*) + sizeof(zend_object) , class_type);
		}


		/**
		 ** Zend deallocates, so this does nothing
		 */ 
		void operator delete(void* mem)
		{
		}
		
		virtual void debug_info(htab_write ht);

		virtual zstr_user extender();

		template< typename T > friend class base_obj_mgr;
	};


	

	template<typename T>
	T* zobj_toc(zend_object* zobj)
	{
		T** pp = (T**)(zobj);
		//--pp;
		return *--pp;
		//assert(zo;
	}

	template<typename T>
	T* zval_toc(zval* z)
	{
		return zobj_toc<T>(z->value.obj);
	}
	/** Linked list, 
	 *  links to every base_obj_mgr,
	 *  order not important.
	 */

	class mgr_link {
	protected:
		static mgr_link* l_start_;
		static mgr_link* l_end_;
#ifdef BASE_DEBUG
		mgr_link* next_;

		virtual zend_string* debug_classname() = 0;
		virtual int debug_alive() = 0;
#endif
	public:
		mgr_link() {
#ifdef BASE_DEBUG
			next_ = nullptr;
			if (!l_end_) {
				l_start_ = this;
				l_end_ = this;
			}
			else {
				l_end_->next_ = this;
				l_end_ = this;
			}
#endif
		}
;

		static void report() 
		{

#ifdef BASE_DEBUG	
			mgr_link* lit = l_start_;
			int total = 0;
			while(lit) {
				mgr_link* p = lit;
				lit = lit->next_;

				zend_string* name = p->debug_classname();
				int alive = p->debug_alive();
				total += alive;
				zend_printf("class: %-60s  live = %d\n", ZSTR_VAL(name), alive);
			}
			zend_printf("Total live = %d\n", total);
#endif
		}
	};

	/**
	 * This will be instantiated before being 
	 * called. Singleton and static access.
	 * Holds zend_object* class handlers and entry,
	 * and static functions for object management.
	 */
	template< typename T >
	class base_obj_mgr : public mgr_link  {

	#ifdef BASE_DEBUG
	protected:
		virtual zend_string* debug_classname()
		{
			return (mydef::class_entry_->name);
		}

		virtual int debug_alive()
		{
			return mydef::obj_count_;
		}
	#endif
		/**
		 * At module init time, one is instantiated for each class T.
		 */ 
	public:
		typedef base_obj_mgr<T> mydef;

		static zend_class_entry* 	    class_entry_;
		static zend_object_handlers     handlers_;

		/**
		 *  self_ not likely to be used, because this base_obj_mgr object has no data members 
		 */
		static base_obj_mgr<T>*         self_; 
		static size_t					self_count_;	
		static size_t                   sizeoft_;	
#ifdef BASE_DEBUG			
		static size_t					obj_count_;
#endif

		/** Initialize the zend_object handlers */
		virtual void init_class_fn()
		{
			class_entry_->create_object = mydef::znew_ex;

			// std_object_handlers is somewhere in PHP
			memcpy(&handlers_, &std_object_handlers, sizeof(zend_object_handlers));

			handlers_.offset = sizeof(T) + sizeof(base_d*);
			handlers_.get_debug_info = mydef::base_debug_info; // can be set later?
			handlers_.clone_obj = nullptr; //cloning not supported
			handlers_.dtor_obj  = zend_objects_destroy_object;
			handlers_.free_obj  = mydef::z_free;

			#ifdef BASE_DEBUG	
				showstr("init_class_fn", class_entry_->name);
			#endif
		}


	public:

		static size_t alive() 
		{
#ifdef BASE_DEBUG
			return mydef::obj_count_;
#else
			return 0;
#endif
		};

		static zend_object* zobj(T* cobj)
		{
			return (zend_object*)((char*)cobj + sizeof(char*) + sizeof(T));
		}

		static zend_string* name()
		{
			return (mydef::class_entry_->name);
		}

		static zend_object* make_new()
		{
			zend_object* nobj = mydef::znew_ex(class_entry_);
			return nobj;
		}

		static  zobj_mgr new_zobj()
		{
			// setup object with handlers
			zobj_mgr result;
			result.adopt(mydef::make_new());
			//showobj("new_zobj", result);
			return result;
		}

		static zend_string* class_name()
		{
			return (mydef::class_entry_->name);
		}
		static bool myType(zend_object* zobj)
		{
			return (zobj->ce == mydef::class_entry_);
		}
		

#ifdef BASE_DEBUG
		static void showptr(const char* s,  T* p)
		{
			zend_object* zo = p->vobj();
			zend_printf("%s %d: %lx p %lx ob (%ld) %s size %d ", s, obj_count_, p, zo, GC_REFCOUNT(zo), typeid(T).name()
				, sizeof(T) + sizeof(base_d*) + sizeof(zend_object) + zend_object_properties_size(class_entry_));
			zstr_user ext = p->extender();
			if (ext.size())
			{
				zend_printf("extd %s\n", ext.data());
			}
			else {
				zend_printf("\n");
			}
		}
#endif
		static void z_free(zend_object* obj) 
		{

			
			T* tp = mydef::cpp(obj);
			

#ifdef BASE_DEBUG			
			showptr("z_free", tp);
#endif

			tp->~T(); 	
#ifdef BASE_DEBUG
			obj_count_--;
#endif

			#ifdef BASE_DEBUG
			zend_printf("dtor zend object %lx\n", obj);
			#endif
			zend_object_std_dtor(obj);

			//efree(tp); // zend_object_alloc uses emalloc()

		}

		// establish self pointer
		base_obj_mgr() : mgr_link() {
			 if (self_count_ > 0)
	         {
	            throw std::logic_error{ "Instance exists already!" };
	         }
	         self_count_++;
	         base_obj_mgr<T>::self_ = this;
		}
		static zend_class_entry* classEntry() {
			return mydef::class_entry_;
		}

		static void classEntry(zend_class_entry* zce)
		{
			if (zce == nullptr)
			{
				throw std::logic_error("zend_class_entry cannot be 0\n");
			}
			mydef::class_entry_ = zce;
			mydef::self_->init_class_fn();
		}

		static zend_object_handlers& handlers() {
			return handlers_;
		}


		static T* cpp(zend_object* zobj) {
			
			T* cobj = zobj_toc<T>(zobj);
#ifdef BASE_DEBUG
			zend_printf(" zfree cpp %s\n", typeid(T).name());
			if(cobj->vobj() != zobj ) {

				throw std::logic_error{ " zend_object* fail in cpp!" };
			}
#endif
			return cobj;
			
		}

		static HashTable* base_debug_info(
			zend_object* object, 
			int* is_temp)
		{
			

			/* to be deleted by zend */
			*is_temp = 1; 
			
			htab_mgr ret;
			
			//showobj("obj = ", object);
			//showarray("base_debug_info", ret);
			htab_write hw(ret);
			//showarray("base_debug_info write", ret);
			T* cobj = cpp(object);
			//zend_printf("2 T* %lx zobj %lx\n", cobj, cobj->zobj());
			cobj->debug_info(hw);
			HashTable* result = ret.steal();
			
			return result;
		}

		static zend_object *znew_ex(
			zend_class_entry *class_type )
		{
			

		//pzo = static_cast<pz_route> (zend_object_alloc(sizeof(wcc_route_d), class_type));
			T* pzo = new (class_type) T();

			T** bp =  (T**) (pzo+1); // next T
			*bp = pzo;		  // self ptr
			zend_object* zobj = (zend_object*) (bp+1); // next T*

#ifdef BASE_ZOBJPTR
			pzo->set_zobj(zobj);
#endif

			zend_object_std_init(zobj, class_type);
			// maybe no properties are configured.
			object_properties_init(zobj, class_type);
			
#ifdef BASE_DEBUG
			obj_count_++;
			showptr("znew_ex", pzo);
#endif
			

			zend_class_entry*      parent = class_type;
			bool                   inherited = false;

			while(parent) {
				if (parent == mydef::class_entry_) 
				{
					// object functions table
					zobj->handlers = &mydef::handlers_; 
					break;
				}
				parent = parent->parent;
				inherited = true;
			}

			ZEND_ASSERT(parent);

			return zobj;
		}
	};

template<typename T> base_obj_mgr<T>*  base_obj_mgr<T>::self_ = nullptr;
template<typename T> zend_class_entry*  base_obj_mgr<T>::class_entry_ = nullptr;
template<typename T> zend_object_handlers base_obj_mgr<T>::handlers_;
template<typename T> size_t  base_obj_mgr<T>::self_count_ = 0;
template<typename T> size_t  base_obj_mgr<T>::sizeoft_ = sizeof(T);

#ifdef BASE_DEBUG
template<typename T> size_t  base_obj_mgr<T>::obj_count_ = 0;
#endif

}; //namespace


#endif
