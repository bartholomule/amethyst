/*
 * $Id: quick_vector.hpp,v 1.1 2004/04/09 06:52:19 kpharris Exp $
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

namespace amethyst
{ 

  /**
   * A simple random_access iterator.
   */
  template <class T>
  class simple_iterator : public std::iterator<std::random_access_iterator_tag,T,size_t,T*,T&>
  {
  public: 
    simple_iterator(): ptr(NULL)
    {
    }
    simple_iterator(T* p): ptr(p)
    {
    }
    simple_iterator(const simple_iterator<T>& old): ptr(old.ptr)
    {
    }
    ~simple_iterator()
    {
    }
    simple_iterator& operator=(const simple_iterator<T>& old)
    {
      ptr = old.ptr;
      return *this;
    }
    
    T* operator->() { return ptr; }
    T& operator*() { return *ptr; }
    simple_iterator operator++() 
    {
      ++ptr;
      return *this;
    }
    simple_iterator operator++(int) 
    {
      simple_iterator temp(*this);
      ++ptr;
      return temp;
    }
    bool operator!=(const simple_iterator& it) const
    {
      return ptr != it.ptr;
    }
    bool operator==(const simple_iterator& it) const
    {
      return ptr == it.ptr;
    }
    bool operator<(const simple_iterator& it) const
    {
      return ptr < it.ptr;
    }
    simple_iterator<T> operator--() 
    {
      --ptr;
      return *this;
    }
    simple_iterator<T> operator--(int) 
    {
      simple_iterator<T> temp(*this);
      --ptr;
      return temp;
    }
    size_t operator-(const simple_iterator<T>& iter) const
    {
      return ptr - iter.ptr;
    }
    simple_iterator<T> operator+(size_t i) const
    {
      return simple_iterator<T>(ptr + i);
    }
    simple_iterator<T> operator-(size_t i) const
    {
      return simple_iterator<T>(ptr - i);
    }
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
    const_simple_iterator(): ptr(NULL)
    {
    }
    explicit const_simple_iterator(const T* p): ptr(p)
    {
    }
    const_simple_iterator(const const_simple_iterator<T>& old): ptr(old.ptr)
    {
    }
    ~const_simple_iterator()
    {
    }
    const_simple_iterator& operator=(const const_simple_iterator<T>& old)
    {
      ptr = old.ptr;
      return *this;
    }
    
    const T* operator->() const { return ptr; }
    const T& operator*() const { return *ptr; }
    const_simple_iterator operator++() 
    {
      ++ptr;
      return *this;
    }
    const_simple_iterator operator++(int) 
    {
      const_simple_iterator temp(*this);
      ++ptr;
      return temp;
    }
    bool operator!=(const const_simple_iterator& it)
    {
      return ptr != it.ptr;
    }
    bool operator==(const const_simple_iterator& it)
    {
      return ptr == it.ptr;
    }
    bool operator<(const const_simple_iterator& it) const
    {
      return ptr < it.ptr;
    }    
    const_simple_iterator<T> operator--() 
    {
      --ptr;
      return *this;
    }
    const_simple_iterator<T> operator--(int) 
    {
      const_simple_iterator<T> temp(*this);
      --ptr;
      return temp;
    }
    size_t operator-(const const_simple_iterator<T>& iter) const
    {
      return ptr - iter.ptr;
    }
    const_simple_iterator<T> operator+(size_t i) const
    {
      return const_simple_iterator<T>(ptr + i);
    }
    const_simple_iterator<T> operator-(size_t i) const
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
   * Note: This may not be thread safe.  While attempts have been made to make
   * many operations (append, for example) safe for access while appending
   * (read-only, of course), locks/mutexes should be used around any critical
   * sections.
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

  private:
    T* data_pointer;
    size_t data_size;

    /**
     * Call the destructor for the first count items, free the data pointer, 
     * and set things to a known state (0 and NULL);
     */
    void destroy_data(size_t count);
  
  protected:

  public:
    /** Default constructor (empty) */
    quick_vector();

    /** Secondary constructor (sized) */
    quick_vector(size_t size);
    
    /** Tertiary constructor (sized/initialized) */
    quick_vector(size_t size, const T& initial_value);

    /** Destructor */
    virtual ~quick_vector();

    /** Copy constructor */
    quick_vector(const quick_vector& old);

    /** Assignment operator */
    quick_vector& operator= (const quick_vector& old);

    /** Subscript operator (not size checked) */
    T& operator[](size_t index)
    {
      return data_pointer[index];
    }
    /** Subscript operator (not size checked) */
    T operator[](size_t index) const
    {
      return data_pointer[index];
    }
    /** Returns the size of the vector */
    size_t size() const
    {
      return data_size;
    }

    iterator begin() { return iterator(data_pointer); }
    iterator end() { return iterator(data_pointer + data_size); }
    const_iterator begin() const { return const_iterator(data_pointer); }
    const_iterator end() const { return const_iterator(data_pointer + data_size); }
    
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    void append(const T& val);
    void append(const quick_vector<T>& vec);
    
  }; // class quick_vector


  //-------------------------------------------
  // Default constructor for class quick_vector
  //-------------------------------------------
  template<class T>
  inline quick_vector<T>::quick_vector():
    data_pointer(NULL),
    data_size(0)    
  {
  
  } // quick_vector()


  template<class T>
  inline void quick_vector<T>::destroy_data(size_t count)
  {
    if( data_pointer )
    {
      for( size_t i = 0; i < count; ++i )
      {
        data_pointer[i].~T();
      }
      delete[] reinterpret_cast<char*>(data_pointer);
    }
    data_pointer = NULL;
    data_size = 0;
  }

  //-------------------------------------------
  // Secondary constructor for class quick_vector
  //-------------------------------------------
  template<class T>
  inline quick_vector<T>::quick_vector(size_t size):
    data_size(size)
  {
    data_pointer = reinterpret_cast<T*>(new char[size * sizeof(T)]);
    size_t i = 0;

#if defined(AMETHYST_EXCEPTION_SAFETY)    
    try
#endif
    {
      for( ; i < data_size; ++i )
      {
        new(&data_pointer[i]) T();
      }
    }
#if defined(AMETHYST_EXCEPTION_SAFETY)
    catch(...)
    {
      // Clean up any pre-constructed items.
      destroy_data(i);
      
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
    data_size(size)
  {
    data_pointer = reinterpret_cast<T*>(new char[size * sizeof(T)]);
    size_t i = 0;
#if defined(AMETHYST_EXCEPTION_SAFETY)
    try
#endif
    {
      for( ; i < data_size; ++i )
      {
        new(&data_pointer[i]) T(initial_value);
      }
    }
#if defined(AMETHYST_EXCEPTION_SAFETY)
    catch(...)
    {
      // Clean up any pre-constructed items.
      destroy_data(i);

      // Rethrow the exception.
      throw;
    }
#endif
  } // quick_vector()


  //----------------------------------
  // Destructor for class quick_vector
  //----------------------------------
  template<class T>
  inline quick_vector<T>::~quick_vector()
  {
    destroy_data(data_size);
  } // ~quick_vector()

  //----------------------------------------
  // Copy constructor for class quick_vector
  //----------------------------------------
  template<class T>
  inline quick_vector<T>::quick_vector(const quick_vector<T>& old):
    data_pointer(NULL),
    data_size(0)
  {
    if( old.data_pointer )
    {
      data_pointer = reinterpret_cast<T*>(new char[sizeof(T) * old.data_size]);
      data_size = old.data_size;

      size_t i = 0;

#if defined(AMETHYST_EXCEPTION_SAFETY)
      try
#endif
      {
        for( ; i < old.data_size; ++i )
        {
          new(&data_pointer[i]) T(old.data_pointer[i]);
        }
      }
#if defined(AMETHYST_EXCEPTION_SAFETY)
      catch(...)
      {
        // Cleanup any already constructed items...
        destroy_data(i);

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
      if( data_size != old.data_size )
      {
        destroy_data(data_size);
        
        if( old.data_size )
        {
          data_pointer = reinterpret_cast<T*>(new char[sizeof(T) * old.data_size]);
          data_size = old.data_size;

          // Copy the data.  Note that this is unintuitive, as it uses the copy
          // constructor instead of the assignment operator, however it is MUCH
          // faster.
          size_t i = 0;
#if defined(AMETHYST_EXCEPTION_SAFETY)
          try
#endif
          {
            for( ; i < data_size; ++i )
            {
              new(&data_pointer[i]) T(old.data_pointer[i]);
            }
          }
#if defined(AMETHYST_EXCEPTION_SAFETY)
          catch(...)
          {
            // Cleanup any already constructed items...
            destroy_data(i);

            // Rethrow the exception.
            throw;
          }
#endif
        }
      }
      else
      {
        // The proper size.  Use the assignment operator for each element.
        // NOTE: This assumes that the data sizes are valid when the pointers
        // are non-null, and that when the pointers are NULL, the data size is
        // 0.  This should always be the case.
        for( size_t i = 0; i < data_size; ++i )
        {
          data_pointer[i] = old.data_pointer[i];
        }
      }
    }
    return (*this);
  } // quick_vector::operator=(quick_vector)

  template <class T>
  inline void quick_vector<T>::append(const T& val)
  {
    size_t new_size = data_size + 1;
    T* new_ptr = reinterpret_cast<T*>(new char[sizeof(T) * new_size]);

    size_t i = 0;

#if defined(AMETHYST_EXCEPTION_SAFETY)
    try
#endif
    {
      for( ; i < data_size; ++i )
      {
	new(&new_ptr[i]) T(data_pointer[i]);
      }
      new(&new_ptr[i++]) T(val);
    }
#if defined(AMETHYST_EXCEPTION_SAFETY)
    catch(...)
    {
      // Clean up any pre-constructed items.
      for( size_t j = 0; j < i; ++j )
      {
	new_pointer[j].~T();
      }
      delete[] new_ptr;
      // Rethrow.
      throw;
    }
#endif
    T* temp = data_pointer;
    data_pointer = new_ptr;
    data_size = new_size;
    for( size_t j = 0; j < data_size; ++j )
    {
      temp[j].~T();
    }
    delete[] reinterpret_cast<char*>(temp);
  }

  template <class T>
  inline void quick_vector<T>::append(const quick_vector<T>& vec)
  {
    size_t new_size = data_size + vec.data_size;
    T* new_ptr = reinterpret_cast<T*>(new char[sizeof(T) * new_size]);


    size_t i = 0;
    size_t j = 0;

#if defined(AMETHYST_EXCEPTION_SAFETY)
    try
#endif
    {
      for( ; i < data_size; ++i )
      {
	new(&new_ptr[i]) T(data_pointer[i]);
      }
      for( ; j < vec.data_size; ++j )
      {
	new(&new_ptr[i + j]) T(vec.data_pointer[j]);
      }
    }
#if defined(AMETHYST_EXCEPTION_SAFETY)
    catch(...)
    {
      // Clean up any pre-constructed items.
      for( size_t k = 0; k < (i+j); ++k )
      {
	new_pointer[k].~T();
      }      
      delete[] new_ptr;
      // Rethrow
      throw;
    }
#endif
    T* temp = data_pointer;
    data_pointer = new_ptr;
    data_size = new_size;    
    for( size_t k = 0; k < data_size; ++k )
    {
      temp[j].~T();
    }
    delete[] reinterpret_cast<char*>(temp);
  }
  
} // namespace amethyst


#endif /* !defined(AMETHYST__QUICK_VECTOR_HPP) */
