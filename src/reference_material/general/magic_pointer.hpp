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
#if       !defined(KH_MAGIC_POINTER_HPP)
#define            KH_MAGIC_POINTER_HPP

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
 * magic_pointer<int> ptr = new int;
 * or
 * magic_pointer<int> ptr = new_object<int>(value);
 *
 * Deallocation is not necessary, as the pointer will remain only until all
 * objects using it have been destroyed. 
 *
 * To allocate an array of ints:
 * magic_pointer<int,kh_array_unallocator<int> > ptr = new int[size];
 * or
 * magic_pointer<int,kh_array_unallocator<int> > ptr = new_array<int>(size);
 * or (to initialize everything to a known value)
 * magic_pointer<int,kh_array_unallocator<int> > ptr = new_array<int>(size,val);
 *
 * I wish there was something I could do about the ugliness, but it needs to
 * know how to deallocate whatever you give it, so the proper unallocator must
 * be passed (default is to a single object).
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
*/

#include <stddef.h>
#include "ref_count_object.hpp"
#include <new>


#if defined(KH_DEBUG_RCP)
#include <cstdio>
#include "textized.hpp"
#endif

#include <iostream>
using std::ostream;

/**
 * A simple unallocator for non-array deallocation.
 * This will be used for normal operation of the magic_pointer class, so that
 * the object can be properly deallocated.
 * @see kh_array_unallocator
 * @see magic_pointer
 * @author Kevin Harris
 */
template <class T>
class kh_unallocator
{
public:
  /** Returns true if the unallocator performs multiple delete. */
  inline bool multiple_delete() const { return false; }
  /** Default constructor (does nothing). */
  kh_unallocator() {}
  /** Destructor (completely useless) */  
  ~kh_unallocator() { }
  /** [Constructor] This deletes its paramater using delete (single). */
  kh_unallocator(T* ptr)
  {
    (*this)(ptr);
  }
  /** Perform the actual unallocation (using delete). */
  void operator()(T* ptr)
  {
#if defined(KH_DEBUG_RCP)
    printf("%s",__PRETTY_FUNCTION__);
#endif
    if(ptr != NULL)
    {
#if defined(KH_DEBUG_RCP)
      printf("Uncallocating address 0x%06x.  \n",(int)ptr);
#endif      
      delete ptr;
    }
#if defined(KH_DEBUG_RCP)
    else
    {
      printf("Cannot unallocate NULL pointer.\n");
    }
#endif
  }
};

/**
 * A simple unallocator for array deallocation.
 * This is only used when requested (non-default).  It properly deallocates
 * arrays allocated with new[].  NOTE: Do NOT use the rcp_static cast function
 * when using reference counted arrays.  Such use will result in memory loss,
 * as the unallocator can NOT be copied across.
 * @see kh_unallocator
 * @see magic_pointer
 * @author Kevin Harris
 */
template <class T>
class kh_array_unallocator
{
public:
  /** Returns true if the unallocator performs multiple delete. */
  inline bool multiple_delete() const { return true; }
  /** Default constructor (does nothing). */
  kh_array_unallocator() {}
  /** Destructor (completely useless) */  
  ~kh_array_unallocator() {}  
  /** [Constructor] This deletes its paramater using delete[] (multiple). */
  kh_array_unallocator(T* ptr)
  {
    (*this)(ptr);
  }  
  /** Perform the actual unallocation (using delete[]). */
  void operator()(T* ptr)
  {
#if defined(KH_DEBUG_RCP)
    printf("%s",__PRETTY_FUNCTION__);
#endif
    if(ptr != NULL)
    {
#if defined(KH_DEBUG_RCP)
      printf("Array uncallocating address 0x%06x.\n",ptr);
#endif      
      delete[] ptr;
    }
#if defined(KH_DEBUG_RCP)
    else
    {
      printf("Cannot unallocate NULL pointer.\n");
    }
#endif
  }
};



/**
 * A reference counted pointer class (magic_pointer is a much shorter name).
 * A simple class type that can be used to "allocate and forget".  As this is
 * reference counted, it should not be used for circular structures that
 * reference themselves through one of these pointers.  Notes about the actual
 * usage of the class can be found in magic_pointer.hpp.
 *
 * @see kh_unallocator
 * @see kh_array_unallocator
 * @see reference_counting_object
 * @author Kevin Harris
 */
template <class T, class unallocator=kh_unallocator<T>,class counter_type=int>
class magic_pointer
{
private:
  /** An instance of the unallocator for this pointer. */
  unallocator unallocate;
public:
  /** A typedef of the object which performs the actual reference counting. */
  typedef reference_counting_object<counter_type> counting_object;
  /** Default constructor */
  magic_pointer():
    counts(),data_pointer(NULL)
  {
#if defined(KH_DEBUG_RCP)
    printf("%s\n",__PRETTY_FUNCTION__);
#endif
  }
  /** Constructor with pointer and counts */
  explicit magic_pointer(T* ptr, counting_object _counts = counting_object()):
    counts(_counts),data_pointer(ptr)
  {
#if defined(KH_DEBUG_RCP)
    printf("%s\n",__PRETTY_FUNCTION__);
#endif
  }
  /** Destructor */
  ~magic_pointer()
  {
#if defined(KH_DEBUG_RCP)
    printf("%s -- Count=%d\n",__PRETTY_FUNCTION__,int(counts.reference_count()));
#endif
    if(counts.reference_count() <= counter_type(1))
      unallocate(data_pointer);
  }
  /** Copy constructor */
  magic_pointer(const magic_pointer& oc):
    counts(oc.counts), data_pointer(oc.data_pointer)
  {
#if defined(KH_DEBUG_RCP)
    printf("%s -- copied counts=%d\n",__PRETTY_FUNCTION__,int(counts.reference_count()));
#endif
  }
  /** Assignment operator */
  magic_pointer& operator=(const magic_pointer& oc)
  {
#if defined(KH_DEBUG_RCP)
    printf("%s -- Count=%d\n",__PRETTY_FUNCTION__,int(counts.reference_count()));
#endif
    if((&oc != this) && (oc.data_pointer != data_pointer))
    {
      if(counts.reference_count() <= counter_type(1))
        unallocate(data_pointer);
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
    *this = magic_pointer(data);
  }
  /**
   * Replaces the old pointer and counts with the ones given.
   * @param data The reference counted pointer to copy.
   */
  void set(const magic_pointer& data)
  {
    *this = data;
  }
  /**
   * A function which will duplicate the data that this object points to. This
   * is intended to be used before modifying shared data.
   */        
  magic_pointer<T,unallocator,counter_type> copy_for_write()
  {
    if( (!unallocate.multiple_delete()) && 
	(counts.reference_count() > counter_type(1)) &&
	!!data_pointer )
    {
      magic_pointer<T,unallocator,counter_type> ptr = *this;
      
      *this = magic_pointer(new T(*data_pointer));
      
      return ptr;
    }
    return(*this);
  }

  /** Returns true if the pointer is NULL */
  bool operator!() const
  {
    return !data_pointer;
  }
  /** Returns false if the pointer is NULL. */
  operator bool() const
  {
    return data_pointer;
  }
  /** magic_pointer == magic_pointer */
  bool operator==(const magic_pointer<T,unallocator,counter_type>& oc) const
  {
    return(data_pointer == oc.data_pointer);
  }
  /** magic_pointer == pointer */
  friend bool operator==(const magic_pointer<T,unallocator,counter_type>& oc, const T* ptr)
  {
    return(oc.data_pointer == ptr);
  }
  /** pointer == magic_pointer */
  friend bool operator==(const T* ptr, const magic_pointer<T,unallocator,counter_type>& oc)
  {
    return(oc.data_pointer == ptr);
  }  
  /** magic_pointer != magic_pointer */
  bool operator!=(const magic_pointer<T,unallocator,counter_type>& oc)
  {
    return(data_pointer != oc.data_pointer);
  }
  /** magic_pointer == pointer */
  friend bool operator!=(const magic_pointer<T,unallocator,counter_type>& oc, const T* ptr)
  {
    return(oc.data_pointer != ptr);
  }
  /** pointer != magic_pointer */
  friend bool operator!=(const T* ptr,const magic_pointer<T,unallocator,counter_type>& oc)
  {
    return(oc.data_pointer != ptr);
  }
  /** Dereference operator (*) */
  T& operator*()
  {
#if defined(KH_DEBUG_RCP)
    printf("%s\n",__PRETTY_FUNCTION__);
#endif    
    return(*data_pointer);
  }
  /** Indexed dereference operator ([]) */
  T& operator[](size_t index)
  {
#if defined(KH_DEBUG_RCP)
    printf("%s\n",__PRETTY_FUNCTION__);
#endif        
    return(data_pointer[index]);
  }
  /** Member access through pointer (->) */
  T* operator->()
  {
#if defined(KH_DEBUG_RCP)
    printf("%s\n",__PRETTY_FUNCTION__);
#endif        
    return(data_pointer);
  }
  /** Dereference operator (*) */
  const T& operator*() const
  {
#if defined(KH_DEBUG_RCP)
    printf("%s\n",__PRETTY_FUNCTION__);
#endif        
    return(*data_pointer);
  }
  /** Indexed dereference operator ([]) */
  const T& operator[](size_t index) const
  {
#if defined(KH_DEBUG_RCP)
    printf("%s\n",__PRETTY_FUNCTION__);
#endif        
    return(data_pointer[index]);
  }  
  /** Member access through pointer (->) */
  const T* operator->() const
  {
#if defined(KH_DEBUG_RCP)
    printf("%s\n",__PRETTY_FUNCTION__);
#endif        
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
#if defined(KH_DEBUG_RCP)
    printf("%s\n",__PRETTY_FUNCTION__);
#endif
    return(data_pointer);
  }
  /** Get the pointer. */
  T* get_pointer()
  {
#if defined(KH_DEBUG_RCP)
    printf("%s\n",__PRETTY_FUNCTION__);
#endif
    return(data_pointer);
  }  
  
  /** Stream insertion operator; these are always useful. */
  friend ostream& operator<< <>(ostream& o, const magic_pointer<T,unallocator,counter_type>& rcp);

  /** Get the reference counting object. */
  const counting_object& get_counts() const { return(counts); }

private:
  /** The reference counts for this pointer. */
  counting_object counts;
  /** The actual data pointer. */
  T* data_pointer;
};


/** Create a new reference counted pointer using anallocated/copied object. */
template <class T>
magic_pointer<T> new_object(const T& old_object)
{
  return(new T(old_object));
}

/** Create a new reference counted array of the given size. */
template <class T>
magic_pointer<T,kh_array_unallocator<T> > new_array(int size)
{
  T* ptr = new T[size];
  return(ptr);
}

/**
 * Create an initialized new reference counted array of the given size and
 * value.
 */
template <class T>
magic_pointer<T,kh_array_unallocator<T> > new_array(int size, const T& initial)
{
  T* ptr = new T[size];
  /** Use the placement new operator on each element in the array to initialize
      it. */
  for(int i = 0; i < size; ++i)
  {
    ptr[i].~T();
    new(&ptr[i]) T(initial);
  }
  return(ptr);
}


/**
 * Perform a dynamic_cast<> on the given magic pointer, returning a pointer of
 * the correct type, with shared reference counts.
 */
template <class N,class T, class unallocator,class counter_type>
magic_pointer<N,unallocator,counter_type>
rcp_dynamic_cast(magic_pointer<T,unallocator,counter_type>& old_rcp)
{
  magic_pointer<N,unallocator,counter_type> ret_rcp;
  T* barf = old_rcp.get_pointer();
  N* data_ptr = dynamic_cast<N*>(barf);
  if(data_ptr != NULL)
  {
    ret_rcp = magic_pointer<N,unallocator,counter_type>(data_ptr, old_rcp.get_counts());
  }
  return(ret_rcp);
}

/**
 * Perform a dynamic_cast<> on the given magic pointer, returning a pointer of
 * the correct type, with shared reference counts.
 */
template <class N,class T, class unallocator,class counter_type>
const magic_pointer<N,unallocator,counter_type>
rcp_dynamic_cast(const magic_pointer<T,unallocator,counter_type>& old_rcp)
{
  magic_pointer<N,unallocator,counter_type> ret_rcp;
  N* data_ptr = const_cast<N*>(dynamic_cast<const N*>(old_rcp.get_pointer()));
  if(data_ptr != NULL)
  {
    ret_rcp = magic_pointer<N,unallocator,counter_type>(data_ptr,
							old_rcp.get_counts());
  }
  return(ret_rcp);
}

/**
 * Perform a static_cast<> on the given magic pointer, returning a pointer of
 * the correct type, with shared reference counts.
 */
template <class N,  class T, class unallocator,class counter_type>
magic_pointer<N>
rcp_static_cast(magic_pointer<T,unallocator,counter_type>& old_rcp)
{
  magic_pointer<N> ret_rcp;
  N* data_ptr = static_cast<N*>(old_rcp.get_pointer());
  if(data_ptr != NULL)
  {
    ret_rcp = magic_pointer<N>(data_ptr, old_rcp.get_counts());
  }
  return(ret_rcp);
}

/**
 * Perform a static_cast<> on the given magic pointer, returning a pointer of
 * the correct type, with shared reference counts.
 */
template <class N,class T, class unallocator,class counter_type>
const magic_pointer<N>
rcp_static_cast(const magic_pointer<T,unallocator,counter_type>& old_rcp)
{
  magic_pointer<N> ret_rcp;
  N* data_ptr = const_cast<N*>(static_cast<const N*>(old_rcp.get_pointer()));
  if(data_ptr != NULL)
  {
    ret_rcp = magic_pointer<N>(data_ptr, old_rcp.get_counts());
  }
  return(ret_rcp);
}

/** Stream insertion operator.  Useful for debug. */
template <class T, class unallocator, class counter_type>
ostream& operator<<(ostream& o, const magic_pointer<T,unallocator,counter_type>& rcp)
{
  o << "p=0x" << std::hex << int(rcp.data_pointer)
    << " c="  << std::dec << int(rcp.counts.reference_count())
    << "{";
  if(rcp.data_pointer != NULL)
    o << *rcp.data_pointer;
  else
    o << "NULL";
  o << "}";
  return(o);
}

#endif /* !defined(KH_MAGIC_POINTER_HPP) */
