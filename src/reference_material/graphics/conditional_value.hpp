/*
 * $Id: conditional_value.hpp,v 1.2 2004/08/16 16:04:24 kpharris Exp $
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

#if       !defined(AMETHYST__CONDITIONAL_VALUE_HPP)
#define            AMETHYST__CONDITIONAL_VALUE_HPP

#include <cassert>

namespace amethyst
{ 
  template <class T>
  struct nonzero
  {
    bool operator()(const T& value) const
    {
      return !!value;
    }
  };
  /**
   * 
   * A conditional value -- A value that has a precondition upon access.
   * 
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.2 $
   * 
   */
  template<class value_type,
	   class test_type = bool,
	   class condition_test = nonzero<test_type> >
  class conditional_value
  {

  private:
    value_type value;
    test_type test_value;
    condition_test test;
    
  protected:

  public:
    /** Default constructor */
    conditional_value(const value_type& val = value_type(), const test_type& test_val = test_type(0));

    /** Destructor */
    virtual ~conditional_value();

    /** Copy constructor */
    conditional_value(const conditional_value& old);

    /** Assignment operator */
    conditional_value& operator= (const conditional_value& old);

    value_type operator*() const;
    value_type& operator*();

    const value_type* operator->() const;
    value_type* operator->();

    test_type get_test_value() const { return test_value; }
    void set_test_value(const test_type& test_val ) const { test_value = test_val; }

    bool do_test() const { return test(test_value); }

    const value_type& peek_at_value() const { return value; }
    
  }; // class conditional_value



  //------------------------------------------------
  // Default constructor for class conditional_value
  //------------------------------------------------
  template<class value_type, class test_type, class condition_test>
  conditional_value<value_type,test_type,condition_test>::conditional_value(const value_type& val, const test_type& test_val):
    value(val),
    test_value(test_val)
  {
  
  } // conditional_value()

  //---------------------------------------
  // Destructor for class conditional_value
  //---------------------------------------
  template<class value_type, class test_type, class condition_test>
  conditional_value<value_type,test_type,condition_test>::~conditional_value()
  {
  
  } // ~conditional_value()

  //---------------------------------------------
  // Copy constructor for class conditional_value
  //---------------------------------------------
  template<class value_type, class test_type, class condition_test>
  conditional_value<value_type,test_type,condition_test>::conditional_value(const conditional_value<value_type,test_type,condition_test>& old):
    value(old.value),
    test_value(old.test_value),
    test(old.test)
  {

  } // conditional_value(conditional_value)

  //------------------------------------------------
  // Assignment operator for class conditional_value
  //------------------------------------------------
  template<class value_type, class test_type, class condition_test>
  conditional_value<value_type,test_type,condition_test>& conditional_value<value_type,test_type,condition_test>::operator= (const conditional_value<value_type,test_type,condition_test>& old)
  {
    // Generic check for self-assignment
    if( &old != this)
    {
      value = old.value;
      test_value = old.test_value;
      test = old.test;
    }
    return (*this);
  } // conditional_value::operator=(conditional_value)


  template<class value_type, class test_type, class condition_test>  
  value_type conditional_value<value_type,test_type,condition_test>::operator*() const
  {
    // FIXME!
    assert(test(test_value));
    return value;
  }
  
  template<class value_type, class test_type, class condition_test>  
  value_type& conditional_value<value_type,test_type,condition_test>::operator*()
  {
    // FIXME!
    assert(test(test_value));
    return value;    
  }
  
  template<class value_type, class test_type, class condition_test>  
  const value_type* conditional_value<value_type,test_type,condition_test>::operator->() const
  {
    // FIXME!
    assert(test(test_value));
    return &value;    
  }
  
  template<class value_type, class test_type, class condition_test>  
  value_type* conditional_value<value_type,test_type,condition_test>::operator->()
  {
    // FIXME!
    assert(test(test_value));
    return &value;    
  }
  
} // namespace amethyst


#endif /* !defined(AMETHYST__CONDITIONAL_VALUE_HPP) */

