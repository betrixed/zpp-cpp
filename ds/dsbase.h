#ifndef DS_BASE_H
#define DS_BASE_H
//dsbase.h


namespace dso {
using namespace zpp;

	/** A test variation of a bare C++ object base, 
	 *  following the zpp/base.h almost exactly, except this
	 *  ds_base has 2 pointers less compared to zpp/base.h. 
	 *  So no data-members at all.
	 *  A pointer to dsobj* self, is not stored just below the zend_object*
	 *  nor is a pointer zend_object* kept as a first member.
	 *  
	 *  Virtual C++ function table still used.
	 * 
	 *  Because the zobj - cobj Templates will give a fixed distance,
	 *  Classes using this should be declared final.
	 */

	class ds_base {
	protected:
		void* 
		operator new(std::size_t msize,  zend_class_entry *class_type) 
		{
			return zend_object_alloc(msize + sizeof(zend_object) , class_type);
		}

		/**
		 ** Zend deallocates, so this does nothing.
		 *  Don't even need to have this? 
		 */ 
		void 
		operator delete(void* mem)
		{
			#ifdef DS_BASE_DEBUG			
				zend_printf("delete called %lx\n", mem);
			#endif
		}

		virtual void debug_info(htab_write ht);

		 //! name of extended base class, if any
		virtual zend_string* extender();

		//!  return owning zend_object
		virtual zend_object* zobj() const;

		template< typename T > friend class ds_obj_mgr;
	};

	// might not work for base classes.
	template<typename T>
	T* zobj_toc(zend_object* zobj)
	{
		//Offset is just sizeof(T) below
		//also could grab zobj->ce->handlers->offset
		//but as 3 x pointer indirect.
		return --(T*)(zobj);
	}

	template<typename T>
	T* zval_toc(zval* z)
	{
		return --(T*)(z->value.obj);
	}

	// Only works for prime class
	template<typename T>
	zend_object* zobj(T* cobj)
	{
		return (zend_object*) (++cobj);
	}


	class dso_link {
	protected:
		static dso_link* l_start_;
		static dso_link* l_end_;
#ifdef DS_BASE_DEBUG
		dso_link* next_;

		virtual zend_string* debug_classname() = 0;
		virtual int debug_alive() = 0;
#endif
	public:
		dso_link() {
#ifdef DS_BASE_DEBUG
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

		static void report() 
		{

#ifdef DS_BASE_DEBUG	
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
	};// dso_link class


/**
	 * This will be instantiated before being 
	 * called. Singleton and static access.
	 * Holds zend_object* class handlers and entry,
	 * and static functions for object management.
	 */
	template< typename T >
	class ds_obj_mgr : public dso_link  
	{
	public:
		// A bit more readable?
		typedef ds_obj_mgr<T> mydef;

	#ifdef DS_BASE_DEBUG
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

		static zend_class_entry* 	    class_entry_;
		static zend_object_handlers     handlers_;

		/**
		 *  self_ not likely to be used, because this base_obj_mgr object has no data members 
		 */
		static ds_obj_mgr<T>*           self_; 
		static size_t					self_count_;		
#ifdef DS_BASE_DEBUG			
		static size_t					obj_count_;
#endif

		/** Initialize the zend_object handlers */
		virtual void init_class_fn()
		{
			class_entry_->create_object = mydef::znew_ex;

			// std_object_handlers is somewhere in PHP
			memcpy(&handlers_, &std_object_handlers, sizeof(zend_object_handlers));

			handlers_.offset = sizeof(T);
			handlers_.get_debug_info = mydef::base_debug_info; // can be set later?
			handlers_.clone_obj = nullptr; //clone method not supported
			handlers_.dtor_obj  = zend_objects_destroy_object; // standard destroy
			handlers_.free_obj  = mydef::z_free;

			#ifdef DS_BASE_DEBUG	
				showstr("init_class_fn", class_entry_->name);
			#endif
		}


	public:

		static size_t alive() 
		{
#ifdef DS_BASE_DEBUG
			return mydef::obj_count_;
#else
			return 0;
#endif
		};

		static zend_string* name()
		{
			return (mydef::class_entry_->name);
		}

		static T* make_new()
		{
			zend_object* nobj = mydef::znew_ex(class_entry_);
			return mydef::cpp(nobj);
		}

		static zobj_mgr new_zobj()
		{
			// setup object with handlers
			zobj_mgr result;
			// takeup without ref_count++
			result.adopt(mydef::make_new());
			//showobj("new_zobj", result);
			return result;
		}

		static zend_string* class_name()
		{
			return (mydef::class_entry_->name);
		}
		static bool isMyType(zend_object* zobj)
		{
			return (zobj->ce == mydef::class_entry_);
		}
		
#ifdef DS_BASE_DEBUG
		static void showptr(const char* s,  T* p)
		{
			zend_object* zo = p->zobj();
			zend_printf("%s %d: %lx p %lx ob (%ld) %s size %d ", 
				s, obj_count_, p, zo, GC_REFCOUNT(zo), typeid(T).name(),
				sizeof(T) + sizeof(zend_object) + zend_object_properties_size(class_entry_));
			zend_string* ex = p->extender();
			if (ex != zend_empty_string)
			{
				zend_printf("extd %s\n", ZSTR_VAL(ex));
			}
			else {
				zend_printf("\n");
			}
		}
#endif
		static void z_free(zend_object* obj) 
		{
			// zend_object_std_dtor(obj) // handled by virtual ~T()
			T* tp = mydef::cpp(obj);
#ifdef DS_BASE_DEBUG			
			showptr("z_free", tp);
#endif

			tp->~T(); 	
#ifdef DS_BASE_DEBUG
			obj_count_--;
#endif
			//efree(tp); // zend_object_alloc uses emalloc()
		}

		// establish self pointer
		ds_obj_mgr() : mgr_link() {
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
#ifdef DS_BASE_DEBUG
			if(cobj->zobj() != zobj ) {
				throw std::logic_error{ "zend_object* fail in cpp!" };
			}
#endif
			return cobj;
			
		}

		static HashTable* base_debug_info(
			zend_object* object, 
			int* is_temp)
		{
			T* cobj = cpp(object);

			/* to be deleted by zend */
			*is_temp = 1; 
			HashTable* ret = zend_new_array(6);
			cobj->debug_info(htab_write(ret));

			return ret;
		}

		static zend_object *znew_ex(
			zend_class_entry *class_type )
		{
			T* pzo = new (class_type) T();
			zend_object* zobj = (zend_object*) (pzo+1); // next T*
			zend_object_std_init(zobj, class_type);
			// can object_properties_init be skipped if not used?
			object_properties_init(zobj, class_type);
			
#ifdef DS_BASE_DEBUG
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
//static data members ready to be created
template<typename T> ds_obj_mgr<T>*  ds_obj_mgr<T>::self_ = nullptr;
template<typename T> zend_class_entry*  ds_obj_mgr<T>::class_entry_ = nullptr;
template<typename T> zend_object_handlers ds_obj_mgr<T>::handlers_;
template<typename T> size_t  ds_obj_mgr<T>::self_count_ = 0;

#ifdef DS_BASE_DEBUG
template<typename T> size_t  ds_obj_mgr<T>::obj_count_ = 0;
#endif

}; // namespace dso

//dsbase.h
#endif


