#ifndef REF_RC_H
#define REF_RC_H

namespace zpp {

class htab_rc;
class val_rc;
class obj_rc;

class val_ptr;
class htab_ptr;
class obj_ptr;

/**
 * @class ref_rc
 * @brief	 Reference counted reference to a zval reference
 * @author  Michael Rynn <michael.rynn.500@gmail.com>
 * @copyright copyright (c) 2025 Michael Rynn 
 * @license BSD 3-Clause License	
 * @details  	
 * Make a reference which stores value in the zval
 * inside a reference counted reference handle.
 * which protects value from reference counting!.
 * Allows returning reference types from functions.
 * Not used much yet, therefore not very well tested.
 * There isn't even a ref_ptr class, using the pattern of the other reference counted PHP types. (yet).
 */
class ref_rc {
protected:
	zend_reference* rh_;

	void lose();
	void own();
	void create();

public:
	ref_rc();

	ref_rc(const ref_rc& c);

	ref_rc(ref_rc&& m);

	ref_rc(HashTable* ht);

	ref_rc& operator=(ref_rc&& m);

	const ref_rc& operator=(const ref_rc& c);


	~ref_rc();

	/** 
	 * Move assignments give full ownership to zref_,
	 *  and nullify their argument.
	 * Also the  assign operators, replace existing zval,
	 * without changing zref_ rc.
	 * 
	 * The first assignment creates the zref_
	 */

	ref_rc& operator=(htab_rc &&m);
	ref_rc& operator=(val_rc &&m);
	ref_rc& operator=(obj_rc &&m);

	/** Check what is inside
	   Value means not array or object.
	   It seems redundant to have a reference to an object,
	   since zend_object* is own reference.
	 */
	bool isArray() const;
	bool isObject() const;

	int ref_type() const;
	
	zend_reference* ptr() const
	{
		return rh_;
	}

	/** Pull out ref counted thing
	 */
	val_ptr  value() const;
	htab_ptr array() const;
	obj_ptr  object() const;

	// steal original zval back regardless of ref count.
	// leave referenced value as null
	void takeback(val_rc& takes);
	htab_rc take_array();
};

};

#endif
