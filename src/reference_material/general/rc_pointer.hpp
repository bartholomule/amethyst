/*
 *  Copyright (C) 2000, 2001, 2003 Kevin Harris
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#if       !defined(KH_RC_POINTER_HPP)
#define            KH_RC_POINTER_HPP

#include <typeinfo>

/**
 * A simple class type that can be used to "allocate and forget".  As this is
 * reference counted, it should not be used for circular structures that
 * reference themselves through one of these pointers. 
 *
 * NOTE: * Null pointer access is not checked except on delete (because of old
 *         compiler bugs).
 *       * This class is not recomended for simple types, or for types where an
 *         extra level of indirection could cause runtime problems (anything
 *         that needs REAL speed).
 *       * I have NOT provided functions for pointer arithmetic with reference
 *         counted pointers.  Doing so could cause some interesting problems if
 *         the address after pointer arith is stored.  To avoid problems here,
 *         I have not provided these capabilities (which could be VERY useful
 *         for lots of things). 
 *       * Do NOT use the rcp_static_cast function on something that really
 *         points to an array (allocated with new[]).  It will NOT be properly
 *         deallocated.  It is ONLY intended for use with single objects.
 *         Static casts are dangerous in general, and a dynamic cast should be
 *         used whenever possible (they won't cause unexpected errors or memory
 *         leaks). 
 *       * Do NOT give create magic pointer from a statically allocated array,
 *         or addresses to statically allocated data.  Doing so will cause seg
 *         faults at whatever time the magic pointer is deallocated (count
 *         drops to 0).  These should ONLY be created from newly allocated
 *         memory (as shown below).
 *
 * How to use these objects:
 * They may be freely copied, reassigned, destructed, etc, as their
 * constructors, destructor, and assignment operator work. 
 *
 * To allocate an object, pass the newly allocated pointer to the constructor
 * to an object of the correct reference counted type. 
 *
 * To allocate an int pointer:
 * rc_pointer<int> ptr = new int;
 * or
 * rc_pointer<int> ptr = new_object<int>(value);
 *
 * Deallocation is not necessary, as the pointer will remain only until all
 * objects using it have been destroyed. 
 *
 * @author Kevin Harris  
 */
/*
  Revision History:
  14Feb2000 Created this file.  Added the unallocator and array unallocator.
  Wrote the basic class.  wrote the new_array and new_object
  functions (to be used in simplifying life in some very specific
  cases). 
  22Feb2000 Added a << operator (untested), which is conditionally included if
  one of three things happens: the user includes <iostream> (at least
  GNU), includes <iosfwd> (at least GNU), or they define USE_STREAMS.
  24Feb2000 Added comparison operators for reference counted pointers and for
  pointers to whatever type is used.
  25Feb2000 Commented out the comparison operators, as there seems to be
  something wrong with them that I can't find.
  30Jul2001 Changed things to work with a newer (more compiant) compiler.
  Duplicated the rcp_dynamic_cast and rcp_static_cast functions to
  return const pointers when their argument is a const pointer.
  ??Aug2001 Added the ability to prevent automatic conversion to a pointer type
  (NO_AUTO_PTR_CONV)
  20Mar2004 Changed the name (yet again), removed the unallocator (always use delete).
*/

#include "ref_count_object.hpp"

#include <ostream>

template <class T, class counter_type>
class rc_pointer;

template <class T, class counter_type>
std::ostream& operator<< (std::ostream& o, const rc_pointer<T,counter_type>& rcp);

/**
 * A reference counted pointer class (rc_pointer is a much shorter name).
 * A simple class type that can be used to "allocate and forget".  As this is
 * reference counted, it should not be used for circular structures that
 * reference themselves through one of these pointers.  Notes about the actual
 * usage of the class can be found in rc_pointer.hpp.
 *
 * @see reference_counting_object
 * @author Kevin Harris
 */
template <class T, class counter_type=int>
class rc_pointer
{
public:
  /** A typedef of the object which performs the actual reference counting. */
  typedef reference_counting_object<counter_type> counting_object;

  /** Default constructor */
  rc_pointer():
    counts(),data_pointer(NULL)
  {
  }
  /** Constructor with pointer and counts */
  explicit rc_pointer(T* ptr, counting_object _counts = counting_object()):
    counts(_counts),data_pointer(ptr)
  {
  }
  /** Destructor */
  ~rc_pointer()
  {
    if(counts.reference_count() <= counter_type(1))
    {
      delete data_pointer;
    }
  }
  /** Copy constructor */
  rc_pointer(const rc_pointer& oc):
    counts(oc.counts), data_pointer(oc.data_pointer)
  {
  }
  /** Assignment operator */
  rc_pointer& operator=(const rc_pointer& oc)
  {
    if((&oc != this) && (oc.data_pointer != data_pointer))
    {
      if(counts.reference_count() <= counter_type(1))
      {
        delete data_pointer;
      }
      counts = oc.counts;
      data_pointer = oc.data_pointer;
    }
    return(*this);
  }
  /**
   * Set the pointer to a value, and start some new counts.  This will
   * (indirectly) unallocate the old pointer if the counts are 0. 
   * @param data A pointer to begin refernce counts on.
   */
  void set(T* data)
  {
    *this = rc_pointer(data);
  }
  /**
   * Replaces the old pointer and counts with the ones given.
   * @param data The reference counted pointer to copy.
   */
  void set(const rc_pointer& data)
  {
    *this = data;
  }
  /**
   * A function which will duplicate the data that this object points to. This
   * is intended to be used before modifying shared data.
   */        
  rc_pointer<T,counter_type> copy_for_write()
  {
    if( (counts.reference_count() > counter_type(1)) && !!data_pointer )
    {
      rc_pointer<T,counter_type> ptr = *this;
      
      *this = rc_pointer(new T(*data_pointer));
      
      return ptr;
    }
    return(*this);
  }
private:
  struct barf_struct_for_safe_bool
  {
    void a_non_null_function() { }
  };
public:
  typedef void (barf_struct_for_safe_bool::*safe_bool)();
  
  /** Returns true if the pointer is NULL */
  safe_bool operator!() const
  {
    return (data_pointer)?(NULL):(&barf_struct_for_safe_bool::a_non_null_function);
  }
  /** Returns false if the pointer is NULL. */
  operator safe_bool() const
  {
    return (data_pointer)?(&barf_struct_for_safe_bool::a_non_null_function):(NULL);    
  }
  /** rc_pointer == rc_pointer */
  bool operator==(const rc_pointer<T,counter_type>& oc) const
  {
    return(data_pointer == oc.data_pointer);
  }
  /** rc_pointer == pointer */
  friend bool operator==(const rc_pointer<T,counter_type>& oc, const T* ptr)
  {
    return(oc.data_pointer == ptr);
  }
  /** pointer == rc_pointer */
  friend bool operator==(const T* ptr, const rc_pointer<T,counter_type>& oc)
  {
    return(oc.data_pointer == ptr);
  }  
  /** rc_pointer != rc_pointer */
  bool operator!=(const rc_pointer<T,counter_type>& oc)
  {
    return(data_pointer != oc.data_pointer);
  }
  /** rc_pointer == pointer */
  friend bool operator!=(const rc_pointer<T,counter_type>& oc, const T* ptr)
  {
    return(oc.data_pointer != ptr);
  }
  /** pointer != rc_pointer */
  friend bool operator!=(const T* ptr,const rc_pointer<T,counter_type>& oc)
  {
    return(oc.data_pointer != ptr);
  }
  /** Dereference operator (*) */
  T& operator*()
  {
    return(*data_pointer);
  }
  /** Member access through pointer (->) */
  T* operator->()
  {
    return(data_pointer);
  }
  /** Dereference operator (*) */
  const T& operator*() const
  {
    return(*data_pointer);
  }
  /** Member access through pointer (->) */
  const T* operator->() const
  {
    return(data_pointer);
  }
  
#if defined(ALLOW_AUTO_PTR_CONV)
  //#warning "AUTO POINTER CONVERSION ENABLED!!!"
  /** Convert to a pointer.  Only if ALLOW_AUTO_PTR_CONV is defined. */
  inline operator T*() { return(data_pointer); }
  /** Convert to a pointer.  Only if ALLOW_AUTO_PTR_CONV is defined. */
  inline operator const T*() const { return(data_pointer); }
#endif
  
  /** Get the pointer. */
  const T* get_pointer() const
  {
    return(data_pointer);
  }
  /** Get the pointer. */
  T* get_pointer()
  {
    return(data_pointer);
  }  
  
  /** Stream insertion operator; these are always useful. */
  friend std::ostream& operator<< <>(std::ostream& o, const rc_pointer<T,counter_type>& rcp);

  /** Get the reference counting object. */
  const counting_object& get_counts() const { return(counts); }

private:

  
  /** The reference counts for this pointer. */
  counting_object counts;
  /** The actual data pointer. */
  T* data_pointer;
};


/** Create a new reference counted pointer using an allocated/copied object. */
template <class T>
rc_pointer<T> new_object(const T& old_object)
{
  return(new T(old_object));
}

/**
 * Perform a dynamic_cast<> on the given magic pointer, returning a pointer of
 * the correct type, with shared reference counts.
 */
template <class N,class T, class counter_type>
rc_pointer<N,counter_type>
rcp_dynamic_cast(rc_pointer<T,counter_type>& old_rcp)
{
  rc_pointer<N,counter_type> ret_rcp;
  T* barf = old_rcp.get_pointer();
  N* data_ptr = dynamic_cast<N*>(barf);
  if(data_ptr != NULL)
  {
    ret_rcp = rc_pointer<N,counter_type>(data_ptr, old_rcp.get_counts());
  }
  return(ret_rcp);
}

/**
 * Perform a dynamic_cast<> on the given magic pointer, returning a pointer of
 * the correct type, with shared reference counts.
 */
template <class N,class T, class counter_type>
const rc_pointer<N,counter_type>
rcp_dynamic_cast(const rc_pointer<T,counter_type>& old_rcp)
{
  rc_pointer<N,counter_type> ret_rcp;
  N* data_ptr = const_cast<N*>(dynamic_cast<const N*>(old_rcp.get_pointer()));
  if(data_ptr != NULL)
  {
    ret_rcp = rc_pointer<N,counter_type>(data_ptr, old_rcp.get_counts());
  }
  return(ret_rcp);
}

/**
 * Perform a static_cast<> on the given magic pointer, returning a pointer of
 * the correct type, with shared reference counts.
 */
template <class N, class T, class counter_type>
rc_pointer<N>
rcp_static_cast(rc_pointer<T,counter_type>& old_rcp)
{
  rc_pointer<N> ret_rcp;
  N* data_ptr = static_cast<N*>(old_rcp.get_pointer());
  if(data_ptr != NULL)
  {
    ret_rcp = rc_pointer<N>(data_ptr, old_rcp.get_counts());
  }
  return(ret_rcp);
}

/**
 * Perform a static_cast<> on the given magic pointer, returning a pointer of
 * the correct type, with shared reference counts.
 */
template <class N,class T, class counter_type>
const rc_pointer<N>
rcp_static_cast(const rc_pointer<T,counter_type>& old_rcp)
{
  rc_pointer<N> ret_rcp;
  N* data_ptr = const_cast<N*>(static_cast<const N*>(old_rcp.get_pointer()));
  if(data_ptr != NULL)
  {
    ret_rcp = rc_pointer<N>(data_ptr, old_rcp.get_counts());
  }
  return(ret_rcp);
}

/**
 * Perform a static_cast<> on the given magic pointer, returning a pointer of
 * the correct type, with shared reference counts.
 */
template <class N, class T, class counter_type>
rc_pointer<N>
rcp_reinterpret_cast(const rc_pointer<T,counter_type>& old_rcp)
{
  rc_pointer<N> ret_rcp;
  N* data_ptr = reinterpret_cast<N*>(old_rcp.get_pointer());
  if(data_ptr != NULL)
  {
    ret_rcp = rc_pointer<N>(data_ptr, old_rcp.get_counts());
  }
  return(ret_rcp);
}

/** Stream insertion operator.  Useful for debug. */
template <class T, class counter_type>
std::ostream& operator<<(std::ostream& o, const rc_pointer<T,counter_type>& rcp)
{
  o << "p=" << std::hex << reinterpret_cast<void*>(rcp.data_pointer)
    << " c=" << std::dec << int(rcp.counts.reference_count())
    << "{";
  if(rcp.data_pointer != NULL)
  {
    o << *rcp.data_pointer;
  }
  else
  {
    o << "NULL";
  }
  o << "}";
  return(o);
}

#endif /* !defined(KH_RC_POINTER_HPP) */
