/*
*  Copyright (C) 2000, 2001 Kevin Harris
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
#if       !defined(KH_REF_COUNT_OBJECT_HPP)
#define            KH_REF_COUNT_OBJECT_HPP

#if defined(KH_DEBUG_RCO)
#include <cstdio>
#endif

/*
  Revision History:
  02Jan2000 Created this file, wrote the reference_counting_object class
  14Feb2000 Changed the class to be templated on the counting type (ie. int,
            long, short, char, etc.), changed all of the printf statements to
	    only occur when KH_DEBUG_RCO is defined.
  01Mar2003 Changed/added comments to work with both doc++ and ccdoc.
 */
/**
 * A simple class that I can place inside of any other class to keep a count of
 * the number of times an object has been copied.  This is meant to be used for
 * reference counting (as in magic_pointer.hpp), but may be able to be used
 * for other purposes.
 *
 * Be sure to note the mutable count_pointer inside of the class.
 *
 * @see magic_pointer
 * @author Kevin Harris
 */
template <class counter_type>
class reference_counting_object
{
public:
  /** Create a new count starting at one */
  reference_counting_object();
  /** Copy the counts from another counting object, incrementing the count. */
  reference_counting_object(const reference_counting_object& rco);

  /** Decrement the value in the shared pointer, and free it when it reaches
      zero. */
  virtual ~reference_counting_object();
  /** Copy the counts from another counting object, incrementing the count. */
  virtual reference_counting_object& operator=(const reference_counting_object& rco);
  /** Return the count of objects accessing the reference */
  operator counter_type() const;
  /** Return the count of objects accessing the reference */
  counter_type reference_count() const { return(counter_type(*this)); }
  /**
   * This one will decrement the count for all other objects, and create a new
   * count starting at 1.  This is something that should be used when functions
   * like 'resize' are called.
   *
   * DO NOT CALL THIS UPON ASSIGN UNLESS THE REFERENCE ISN'T TRULY COPIED
   * (ie. only if reinterpreted, resized, manipulated, etc.).
   */
  void make_new_reference_count();
private:
  /** The shared pointer to the actual counting variable. Note that it is
      mutable, and can be modified even when the class is const. */
  mutable counter_type* count_pointer;
};

template <class counter_type>
reference_counting_object<counter_type>::reference_counting_object()
{
#if defined(KH_DEBUG_RCO)
  printf("%s\n", __PRETTY_FUNCTION__);
#endif

  count_pointer = new counter_type;
  *count_pointer = 1;
}

template <class counter_type>
reference_counting_object<counter_type>::~reference_counting_object()
{
#if defined(KH_DEBUG_RCO)  
  printf("%s", __PRETTY_FUNCTION__);
#endif
  if(count_pointer != NULL)
    {
#if defined(KH_DEBUG_RCO)      
      printf(" -- count=%d", (int)*count_pointer);
#endif      
      if(--*count_pointer == 0)
	{
	  delete count_pointer;
	  count_pointer = NULL;
	}
    }
#if defined(KH_DEBUG_RCO)  
  printf("\n");
#endif
}

template <class counter_type>
reference_counting_object<counter_type>::reference_counting_object(const reference_counting_object& rco)
{
#if defined(KH_DEBUG_RCO)
  printf("%s", __PRETTY_FUNCTION__);
#endif
  count_pointer = rco.count_pointer;
  if(count_pointer != NULL)
    {
      ++*count_pointer;
#if defined(KH_DEBUG_RCO)
      printf(" -- new count=%d", *count_pointer);
#endif
    }
#if defined(KH_DEBUG_RCO)  
  printf("\n");
#endif
}

template <class counter_type>
reference_counting_object<counter_type>& reference_counting_object<counter_type>::operator=(const reference_counting_object& rco)
{
#if defined(KH_DEBUG_RCO)  
  printf("%s\n", __PRETTY_FUNCTION__);
#endif
  // Avoid self assignment and the dec/inc step of assigning something with the
  // same count_pointer.
  if((&rco != this) && (rco.count_pointer != count_pointer))
  {
    // Decrement the current count
    if(count_pointer != NULL)
      if(--*count_pointer == 0)
	delete count_pointer;
    
    // Increment the count for the new object
    count_pointer = rco.count_pointer;
    if(count_pointer != NULL)
      ++*count_pointer;
  }
  return(*this);
}    

template <class counter_type>
reference_counting_object<counter_type>::operator counter_type() const
{
  counter_type my_count = 0;
  if(count_pointer != NULL)
    my_count = *count_pointer;
#if defined(KH_DEBUG_RCO)  
  printf("%s -- count = %d\n", __PRETTY_FUNCTION__, my_count);
#endif
  return(my_count);
}

template <class counter_type>
void reference_counting_object<counter_type>::make_new_reference_count()
{
  if(count_pointer != NULL)
  {
    if(--*count_pointer == 0)
      delete count_pointer;
  }
  count_pointer = new counter_type;
  *count_pointer = 1;
}

#endif // !defined(KH_REF_COUNT_OBJECT_HPP)
