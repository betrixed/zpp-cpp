#ifndef GLOBAL_H
#define GLOBAL_H


namespace zpp {

/**
 *  A place in the $GLOBALS table
 *  
 */
class Globals;

class Global 
{
protected:
    zval_mgr gval_;
    zstr_mgr name_;

    /**
     *  Global value exists?
     *  @var bool
     */
    bool exists_;
public:

    Global(const Global &global) = delete;

    Global(Global &&global) noexcept;

    ~Global();

    
    template <typename T>
    Global &operator=(const T &value)
    {
        gval_.operator=(value);
        //showmem("gval_ =", gval_);
        gval_.make_ref();
        //showmem("gval ref_ =", gval_);
        return update();
    }
    

    /** function to set array value from integer index */

    void set(int index,  zval_user value)
    {
        update();
        htab_user hw(gval_);
        hw.set(index, value);
    }

     /** function to set array value from string key */
    void set(zstr_user key,  zval_user value)
    {
        update();
        htab_user hw(gval_);      
        hw.set(key, value);
    }

    zstr_user name()  { return zstr_user(name_); }
    zval_user value() { return zval_user(gval_); }
    

protected:
    /**
     *  Function that is called when the value is updated
     *  @return Value
     */
    Global& update();

private:
    // Without a value
    Global( zstr_user name);

    // With a found value
    Global( zstr_user name, zval_user val);

    friend class Globals;
};


};
// namespace
#endif
//global.h
