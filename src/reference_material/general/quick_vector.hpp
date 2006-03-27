/*
 * $Id: quick_vector.hpp,v 1.1 2006/03/27 02:56:35 kpharris Exp $
 *
 * Part of "Amethyst" -- A playground for graphics development.
 * Copyright (C) 2004 Kevin Harris
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#if       !defined(AMETHYST__QUICK_VECTOR_HPP)
#define            AMETHYST__QUICK_VECTOR_HPP

// Include for the placement new operator.
#include <new>

// Include for the iterator base classes.
#include <iterator>

// Include for std::distance
#include <algorithm>

/**
 * The size (bytes) to increment after this size has been reached.  Before
 * this size, the size will be repeatedly doubled.  Note that for some types,
 * this size can never be reached evenly, so it is just an approximation.
 */
#if       !defined(AMETHYST_VECTOR_INCREMENT_SIZE)
#define AMETHYST_VECTOR_INCREMENT_SIZE 1024
#endif /* !defined(AMETHYST_VECTOR_INCREMENT_SIZE) */

namespace amethyst
{
  template <class T>
  class const_simple_iterator;

  /**
   * A simple random_access iterator.
   */
  template <class T>
  class simple_iterator : public std::iterator<std::random_access_iterator_tag,T,size_t,T*,T&>
  {
  public:
    inline simple_iterator(): ptr(NULL)
    {
    }
    inline explicit simple_iterator(T* p): ptr(p)
    {
    }
    inline simple_iterator(const simple_iterator<T>& old): ptr(old.ptr)
    {
    }
    inline ~simple_iterator()
    {
    }
    inline simple_iterator& operator=(const simple_iterator<T>& old)
    {
      ptr = old.ptr;
      return *this;
    }

    inline T* operator->() { return ptr; }
    inline T& operator*() { return *ptr; }
    inline simple_iterator operator++()
    {
      ++ptr;
      return *this;
    }
    inline simple_iterator operator++(int)
    {
      simple_iterator temp(*this);
      ++ptr;
      return temp;
    }
    inline bool operator!=(const simple_iterator& it) const
    {
      return ptr != it.ptr;
    }
    inline bool operator==(const simple_iterator& it) const
    {
      return ptr == it.ptr;
    }
    inline bool operator<(const simple_iterator& it) const
    {
      return ptr < it.ptr;
    }
    inline simple_iterator<T> operator--()
    {
      --ptr;
      return *this;
    }
    inline simple_iterator<T> operator--(int)
    {
      simple_iterator<T> temp(*this);
      --ptr;
      return temp;
    }
    inline size_t operator-(const simple_iterator<T>& iter) const
    {
      return ptr - iter.ptr;
    }
    inline simple_iterator<T> operator+(size_t i) const
    {
      return simple_iterator<T>(ptr + i);
    }
    inline simple_iterator<T> operator-(size_t i) const
    {
      return simple_iterator<T>(ptr - i);
    }

    friend class const_simple_iterator<T>;
  private:
    T* ptr;
  };

  /**
   * A simple const random_access iterator
   */
  template <class T>
  class const_simple_iterator : public std::iterator<std::random_access_iterator_tag,const T,size_t,const T*,const T&>
  {
  public:
    inline const_simple_iterator(): ptr(NULL)
    {
    }
    inline explicit const_simple_iterator(const T* p): ptr(p)
    {
    }
    inline const_simple_iterator(const simple_iterator<T>& old) : ptr(old.ptr)
    {
    }
    inline const_simple_iterator(const const_simple_iterator<T>& old): ptr(old.ptr)
    {
    }
    inline ~const_simple_iterator()
    {
    }
    inline const_simple_iterator& operator=(const const_simple_iterator<T>& old)
    {
      ptr = old.ptr;
      return *this;
    }

    inline const T* operator->() const { return ptr; }
    inline const T& operator*() const { return *ptr; }
    inline const_simple_iterator operator++()
    {
      ++ptr;
      return *this;
    }
    inline const_simple_iterator operator++(int)
    {
      const_simple_iterator temp(*this);
      ++ptr;
      return temp;
    }
    inline bool operator!=(const const_simple_iterator& it)
    {
      return ptr != it.ptr;
    }
    inline bool operator==(const const_simple_iterator& it)
    {
      return ptr == it.ptr;
    }
    inline bool operator<(const const_simple_iterator& it) const
    {
      return ptr < it.ptr;
    }
    inline const_simple_iterator<T> operator--()
    {
      --ptr;
      return *this;
    }
    inline const_simple_iterator<T> operator--(int)
    {
      const_simple_iterator<T> temp(*this);
      --ptr;
      return temp;
    }
    inline size_t operator-(const const_simple_iterator<T>& iter) const
    {
      return ptr - iter.ptr;
    }
    inline const_simple_iterator<T> operator+(size_t i) const
    {
      return const_simple_iterator<T>(ptr + i);
    }
    inline const_simple_iterator<T> operator-(size_t i) const
    {
      return const_simple_iterator<T>(ptr - i);
    }
  private:
    const T* ptr;
  };


  /**
   *
   * A quick vector (appenendable only, directly accessible).  This is intended
   * to give a little faster speed over the STL vector.  The size of it cannot
   * be decreased, except by assigning another smaller vector to it.
   *
   * Note: This will not be thread safe. locks/mutexes should be used around
   * any critical sections (construction, appends, etc).
   *
   * Exception safety for constructors and other operations can be gained by
   * defining AMETHYST_EXCEPTION_SAFETY.  This ensures that exceptions thrown
   * while initializing elements of the array will not cause memory leaks from
   * any elements which have already been initialized.  For built-in types
   * (int, float, etc), this exception safety is not required, and speed will
   * be gained by NOT defining this value.
   *
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.1 $
   *
   */
  template<class T>
  class quick_vector
  {
  public:
    typedef simple_iterator<T> iterator;
    typedef const_simple_iterator<T> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    typedef T value_type;
    typedef T& reference;

  private:
    T* data_pointer;
    T* end_pointer;
    T* storage_end;

    /**
     * Call the destructor for all currently existing array elements, but leave
     * the current data pointer intact.
     */
    void destroy_data();

    /**
     * Remove the data pointer, setting things to a known state.
     */
    void remove_storage();

    enum resize_type { E_ABSOLUTE_RESIZE, E_NORMAL_RESIZE };

    /**
     * Increase the storage space in the vector.
     */
    void increase_storage(size_t desired, resize_type size_method = E_ABSOLUTE_RESIZE);

  protected:

  public:
    /** Default constructor (empty) */
    quick_vector();

    /** Secondary constructor (sized) */
    quick_vector(size_t size);

    /** Tertiary constructor (sized/initialized) */
    quick_vector(size_t size, const T& initial_value);

    /** Create a vector from an iterator set. */
    template <class iter_type>
    quick_vector(iter_type first, iter_type last);

    /** Destructor */
    virtual ~quick_vector();

    /** Copy constructor */
    quick_vector(const quick_vector& old);

    /** Assignment operator */
    quick_vector& operator= (const quick_vector& old);

    /** Subscript operator (not size checked) */
    inline T& operator[](size_t index)
    {
      return data_pointer[index];
    }
    /** Subscript operator (not size checked) */
    inline T operator[](size_t index) const
    {
      return data_pointer[index];
    }
    /** Tells if the vector is empty */
    inline bool empty() const
    {
      return begin() == end();
    }
    /** Returns the size of the vector */
    inline size_t size() const
    {
      return end() - begin();
    }

    inline iterator begin() { return iterator(data_pointer); }
    inline iterator end() { return iterator(end_pointer); }
    inline const_iterator begin() const { return const_iterator(data_pointer); }
    inline const_iterator end() const { return const_iterator(end_pointer); }

    inline reverse_iterator rbegin() { return reverse_iterator(end()); }
    inline reverse_iterator rend() { return reverse_iterator(begin()); }
    inline const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    inline const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    void append(const T& val);
    void append(const quick_vector<T>& vec);

    /* push_back is for interface compat with std::vector. It just forwards to append.*/
    void push_back(const T& val);
    void push_back(const quick_vector<T>& vec);

  }; // class quick_vector


  //-------------------------------------------
  // Default constructor for class quick_vector
  //-------------------------------------------
  template<class T>
  inline quick_vector<T>::quick_vector():
    data_pointer(NULL),
    end_pointer(NULL),
    storage_end(NULL)
  {
    const size_t minimum_size = 16;
    increase_storage(minimum_size, E_ABSOLUTE_RESIZE);
  } // quick_vector()


  template<class T>
  inline void quick_vector<T>::destroy_data()
  {
    if( data_pointer )
    {
      for( T* ptr = data_pointer; ptr != end_pointer; ++ptr )
      {
        ptr->~T();
      }
    }
    end_pointer = data_pointer;
  }

  template<class T>
  inline void quick_vector<T>::remove_storage()
  {
    destroy_data();
    delete[] reinterpret_cast<char*>(data_pointer);
    data_pointer = NULL;
    end_pointer = NULL;
    storage_end = NULL;
  }

  //-------------------------------------------
  // Secondary constructor for class quick_vector
  //-------------------------------------------
  template<class T>
  inline quick_vector<T>::quick_vector(size_t size):
    data_pointer(NULL),
    end_pointer(NULL),
    storage_end(NULL)
  {
    increase_storage(size);

#if defined(AMETHYST_EXCEPTION_SAFETY)
    try
#endif
    {
      for( end_pointer = data_pointer; end_pointer != storage_end; ++end_pointer )
      {
        new(end_pointer) T();
      }
    }
#if defined(AMETHYST_EXCEPTION_SAFETY)
    catch(...)
    {
      // Clean up any pre-constructed items.
      remove_storage();

      // Rethrow the exception.
      throw;
    }
#endif
  } // quick_vector()


  //-------------------------------------------
  // Default constructor for class quick_vector
  //-------------------------------------------
  template<class T>
  inline quick_vector<T>::quick_vector(size_t size, const T& initial_value):
    data_pointer(NULL),
    end_pointer(NULL),
    storage_end(NULL)
  {
    increase_storage(size, E_ABSOLUTE_RESIZE);
#if defined(AMETHYST_EXCEPTION_SAFETY)
    try
#endif
    {
      for( end_pointer = data_pointer; end_pointer != storage_end; ++end_pointer )
      {
        new(end_pointer) T(initial_value);
      }
    }
#if defined(AMETHYST_EXCEPTION_SAFETY)
    catch(...)
    {
      // Clean up any pre-constructed items.
      remove_storage();

      // Rethrow the exception.
      throw;
    }
#endif
  } // quick_vector()

  template<class T>
  template <class iter_type>
  inline quick_vector<T>::quick_vector(iter_type first, iter_type last):
    data_pointer(NULL),
    end_pointer(NULL),
    storage_end(NULL)
  {
    increase_storage(std::distance(first, last), E_ABSOLUTE_RESIZE);
#if defined(AMETHYST_EXCEPTION_SAFETY)
    try
#endif
    {
		for( end_pointer = data_pointer; first != last; ++first, ++end_pointer )
      {
        new(end_pointer) T(*first);
      }
    }
#if defined(AMETHYST_EXCEPTION_SAFETY)
    catch(...)
    {
      // Clean up any pre-constructed items.
      remove_storage();

      // Rethrow the exception.
      throw;
    }
#endif
  }


  //----------------------------------
  // Destructor for class quick_vector
  //----------------------------------
  template<class T>
  inline quick_vector<T>::~quick_vector()
  {
    remove_storage();
  } // ~quick_vector()

  //----------------------------------------
  // Copy constructor for class quick_vector
  //----------------------------------------
  template<class T>
  inline quick_vector<T>::quick_vector(const quick_vector<T>& old):
    data_pointer(NULL),
    end_pointer(NULL),
    storage_end(NULL)
  {
    if( old.data_pointer )
    {
      size_t desired_size = old.size();
      size_t next_power = 1;

      while(next_power < desired_size)
      {
        next_power <<= 1;
      }
      desired_size = next_power;

      increase_storage(desired_size);

#if defined(AMETHYST_EXCEPTION_SAFETY)
      try
#endif
      {
        T const* old_ptr = old.data_pointer;
        T const* old_end = old.end_pointer;
        for( end_pointer = data_pointer; old_ptr != old_end; ++end_pointer, ++old_ptr)
        {
          new(end_pointer) T(*old_ptr);
        }
      }
#if defined(AMETHYST_EXCEPTION_SAFETY)
      catch(...)
      {
        // Cleanup any already constructed items...
        remove_storage();

        // Rethrow the exception.
        throw;
      }
#endif
    }
  } // quick_vector(quick_vector)

  //-------------------------------------------
  // Assignment operator for class quick_vector
  //-------------------------------------------
  template<class T>
  inline quick_vector<T>& quick_vector<T>::operator= (const quick_vector<T>& old)
  {
    // Generic check for self-assignment
    if( &old != this)
    {
      // Remove the current data (leave the storage alone).
      destroy_data();

      size_t desired_size = old.size();
      size_t next_power = 1;

      while(next_power < desired_size)
      {
        next_power <<= 1;
      }
      desired_size = next_power;

      // Increase the storage (if required) to hold all of the data to be
      // copied.
      increase_storage(desired_size);

      // NOTE: This assumes that the data sizes are valid when the pointers
      // are non-null, and that when the pointers are NULL, the data size is
      // 0.  This should always be the case.
      //
      // Also, no exception block is required around this, since all memory
      // allocated can be freed from the destructor, and this loop will leave
      // the data in good state (up until the final item that threw), so
      // data_size worth of elements can have the destructor called, should any
      // exception be thrown from a copy constructor..
      T const* old_ptr = old.data_pointer;
      T const* old_end = old.end_pointer;
      for( end_pointer = data_pointer; old_ptr != old_end; ++end_pointer, ++old_ptr)
      {
        new(end_pointer) T(*old_ptr);
      }
    }
    return (*this);
  } // quick_vector::operator=(quick_vector)

  template <class T>
  inline void quick_vector<T>::increase_storage(size_t desired, quick_vector<T>::resize_type size_method)
  {
    // NOTE: Size cannot decrease, so the only other case is equal, in which we
    // don't need any more memory.
    size_t current_size = storage_end - data_pointer;
    if( desired > current_size )
    {
      size_t new_size = desired;
      if( size_method == E_NORMAL_RESIZE )
      {
        new_size = current_size << 1;

        if( new_size < desired )
        {
          new_size = desired;
        }
      }
      T* new_ptr = reinterpret_cast<T*>(new char[sizeof(T) * new_size]);
      T* new_end = new_ptr;

      // NOTE: this exception block is very important if containing objects
      // can allocate memory, or other resources.  Otherwise, there will be a
      // memory leak!
#if defined(AMETHYST_EXCEPTION_SAFETY)
      try
#endif
      {
        for( T* old_iter = data_pointer; old_iter != end_pointer; ++old_iter, ++new_end)
        {
          new(new_end) T(*old_iter);
        }
      }
#if defined(AMETHYST_EXCEPTION_SAFETY)
      catch(...)
      {
        // Note that this only cleans up local storage.  The class storage will
        // be freed when the destructor is called.
        for(T* new_iter = new_ptr; new_iter != new_end; ++new_iter)
        {
          new_iter->~T();
        }

        delete[] reinterpret_cast<char*>(new_ptr);

        throw;
      }
#endif
      remove_storage();
      data_pointer = new_ptr;
      end_pointer = new_end;
      storage_end = data_pointer + new_size;
    }
  }

  template <class T>
  inline void quick_vector<T>::append(const T& val)
  {
    // Call the increase_storage function if there's not enough space.
    if( end_pointer >= storage_end )
    {
      size_t new_size = size() + 1;

      increase_storage(new_size, E_NORMAL_RESIZE);
    }
    new(end_pointer++) T(val);
  }

  template <class T>
  inline void quick_vector<T>::append(const quick_vector<T>& vec)
  {
    // Call the increase_storage function if there's not enough space.
    T* new_end = end_pointer + vec.size();
    if( new_end >= storage_end )
    {
      size_t new_size = size() + vec.size();
      increase_storage(new_size, E_NORMAL_RESIZE);
    }

    const T* copy_end = vec.end_pointer;

    for( const T* copy_iter = vec.data_pointer; copy_iter != copy_end; ++copy_iter, ++end_pointer)
    {
      new(end_pointer) T(*copy_iter);
    }
  }

  template<class T>
  inline void quick_vector<T>::push_back(const T& val)
  {
    append(val);
  }

  template<class T>
  inline void quick_vector<T>::push_back(const quick_vector<T>& vec)
  {
    append(vec);
  }

	template <class T>
	bool operator==(const quick_vector<T>& vec1, const quick_vector<T>& vec2)
	{
		if( vec1.size() != vec2.size() )
		{
			return false;
		}
		typename quick_vector<T>::const_iterator iter1 = vec1.begin();
		typename quick_vector<T>::const_iterator iter2 = vec2.begin();
		while( iter1 != vec1.end() )
		{
			if( *iter1 != *iter2 )
			{
				return false;
			}
			++iter1;
			++iter2;
		}
		return true;
	}
	template <class T>
	bool operator!=(const quick_vector<T>& vec1, const quick_vector<T>& vec2)
	{
		return !(vec1 == vec2);
	}

  /**
   * Send a vector to a stream with simple formatting {  } or { a } or
   * { a, b, ..., d }
   */
  template <class T>
  std::ostream& operator <<(std::ostream& o, const quick_vector<T>& vt)
  {
    int i;
    o << "{ ";
    // output all of the elements that need a trailing comma (if any)
    for(i = 0; i < int(vt.size())-1; ++i)
    {
      o << vt[i];
      o << ", ";
    }
    // print out the remaining elements (if any)
    if(i < int(vt.size()))
    {
      o << vt[i];
    }
    o << " }";
    return(o);
  }

} // namespace amethyst


#endif /* !defined(AMETHYST__QUICK_VECTOR_HPP) */
