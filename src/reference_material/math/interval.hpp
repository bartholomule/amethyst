/*
 * $Id: interval.hpp,v 1.5 2004/03/21 22:13:42 zorthluthik Exp $
 *
 * Part of "Amethyst" a playground for graphics development
 * Copyright (C) 2003 Kevin Harris
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

#if       !defined(AMETHYST__INTERVAL_HPP)
#define            AMETHYST__INTERVAL_HPP

/*
  interval.hpp -- A simple class for use in creating/testing intervals.
*/
/*
  Revision History:
  18May2001 Created this file, "Templatized" the class... Tested it some.
  Complicated it some more (allowing backwards intervals)...
  08Jan2002 Added quite a few comments, before I forget what is going on.
  23Sep2003 Changed the structure a little, moving member functions outside,
            setting some member functions to be templated.  Also, added the 
            < and > operators.
  29Sep2003 Simplified things by removing the ability to be backwards
            (reversing an earlier change).  Also, all references to comparison
            of the given type by '>' were removed, so that only the '<'
            operator must be provided for comparisons ( >, <, and
            inside/outside ) to work. 
  
*/

#include <algorithm>
#include <iostream>
#include <ostream>

namespace amethyst
{ 
  //
  // interval class:
  // A class representing a range of numbers (between to constraints)
  // 
  // It can be used to find:
  //   1. If a number is inside an interval
  //   2. If a number is outside an interval
  //   3. If two intervals overlap
  //   4. If an interval is a subset of another
  //   5. Find the overlap of two intervals
  //   6. Subtract one interval from another
  //   7. If an interval is empty
  //   8. Output an interval (to a std::ostream) in the form [a,b] or [empty]
  //   9. If something is > or < and interval (either an interval itself, or
  //      any other type that can be compared to a T by means of the < and <=
  //      operators).  I would have prefered to only use the < operator, but it
  //      was neccessary to use the <= to allow comparing of intervals when
  //      endpoints are equal (for overlap testing).
  //
  // Notes:
  // * Comparison of anything with an empty interval always returns false.
  //   This may change in the future, if I find that something external doesn't
  //   work properly because of this comparison.  Do not rely on this behavior!
  //
  template <class T>
  class interval
  {
  private:
    T a;
    T b;
    bool is_empty;

  public:
    //
    // Default constructor -- Sets the interval to be empty
    //
    explicit interval(bool empty = true) :
      a(T(0)), b(T(0)), is_empty(empty)
    {
    }

    //
    // Secondary constructor -- Sets an interval to a given range
    //
    interval(T a, T b) : is_empty(false)
    {
      this->a = a;
      this->b = b;
      if( b < a )
      {
	std::swap(this->a, this->b);
      }
      // This doesn't use the ==, as any swapping based on the < has already
      // been done, so if we test < again, and it fails, then it must be empty.
      is_empty = !(this->a < this->b);      
    } // interval(T,T)

    //
    // Find out if a point is inside the interval (generic)
    //
    template <class U>
    bool inside(U d) const
    {
      if( empty() )
      {
        return false;
      }
      return((a < d) && (d < b));
    } // inside(T) const
  
    //
    // Find out if a point is outside the interval (generic)
    //
    template <class U>
    bool outside(U d) const
    {
      if( empty() )
      {
        return false;
      }
      // The inverse of the 'inside' test above.  This is done this way instead
      // of the >= and <= operators to minimize the number of operators that
      // must be defined for types used within an interval.
      return(!((a < d) && (d < b)));
    } // outside(T) const
    

    //
    // Find out if two intervals overlap
    //
    bool overlaps(const interval& i2) const
    {
      if( empty() || i2.empty() )
      {
	// If either one is empty, there can't be any overlap.
        return false;
      }
      if( (a < i2.a) && (i2.a < b) )
      {
	// a-------b    <-- i1 (this)
	//     a--b     <-- i2
	//  OR
	//     a---b    <-- i2
	//  OR
	//     a------b <-- i2
	//
	// i2.a is inside of i1 (this)
	return true;
      }
      else if( (i2.a < a) && (a < i2.b) )
      {
	// a-------b    <-- i2
	//     a--b     <-- i1 (this)
	//  OR
	//     a---b    <-- i1 (this)
	//  OR
	//     a------b <-- i1 (this)
	//
	// a is inside of i2
	return true;
      }
      else if( (a < i2.b) && (i2.b < b) )
      {
	//   a-------b    <-- i1 (this)
	// a------b       <-- i2
	//  OR
	//   a----b       <-- i2
	//  OR
	//     a--b       <-- i2
	//
	// i2.b is inside of i1 (this)
	return true;
      }
      else if( (i2.a < b) && (b < i2.b) )
      {
	//   a-------b    <-- i2
	// a------b       <-- i1 (this)
	//  OR
	//   a----b       <-- i1 (this)
	//  OR
	//     a--b       <-- i1 (this)
	//
	// b is inside of i2
	return true;
      }
      else if( ((a <= i2.a) && (i2.a <= a)) )
      {
	// i1.a == i2.a -- There MUST be some overlap.
	return true;
      }
      else if( ((b <= i2.b) && (i2.b <= b)) )
      {
	// i1.b == i2.b -- There MUST be some overlap.
	return true;
      }
      // The only remaining cases are:
      //
      //     a----b       <-- i1 (this)
      // a---b            <-- i2        i2.b==i1.a
      //       OR      
      //          a---b   <-- i2        i1.b==i2.a
      //       OR      
      // a-b              <-- i2        i2.b < i1.a
      //       OR      
      //            a-b   <-- i2        i2.a > i1.b
      //
      // 
      return false;
    } // overlaps(const interval&) const

    // 
    // Return if the interval i2 is a subset of this interval.
    //
    bool subset(const interval& i2) const
    {
      if( empty() || i2.empty() )
      {
        return false;
      }
      return ((a <= i2.a) && (i2.b <= b));
    } // subset(const interval&) const

    bool empty() const { return is_empty; }
    T begin() const { return a; }
    T end()   const { return b; }

    void set(T begin, T end)
    {
      a = begin; b = end;
      if( b < a )
      {
	std::swap(this->a, this->b);
      }
      // This doesn't use the ==, as any swapping based on the < has already
      // been done, so if we test < again, and it fails, then it must be empty.
      is_empty = !(this->a < this->b);
    } // set(T,T)
  }; // class interval<T>


  //
  // Return the overlap of interval i1 and interval i2 (as an interval)
  //
  template <class T>
  interval<T> get_overlap(const interval<T>& i1, const interval<T>& i2)
  {
    interval<T> result; // result interval (empty by default)
  
    // If either interval is empty, return an empty interval, as there can't be
    // any overlap.
    if( i1.empty() || i2.empty() )
    {
      return result;
    }

    if( (i1.begin() <= i2.begin()) && (i2.begin() <= i1.end()) )
    {
      result = interval<T>(i2.begin(), std::min(i1.end(), i2.end()));
    }
    else if( (i2.begin() <= i1.begin()) && (i1.begin() <= i2.end()) )
    {
      result = interval<T>(i1.begin(), std::min(i1.end(), i2.end()));
    }
    else if( (i1.begin() <= i2.end()) && (i2.end() <= i1.end()) )
    {
      result = interval<T>(std::max(i1.begin(), i2.begin()), i2.end());
    }
    else if( (i2.begin() <= i1.end()) && (i1.end() <= i2.end()) )
    {
      result = interval<T>(std::max(i1.begin(), i1.begin()), i1.end());
    }
    else
    {
      // Is a case here even valid?
      std::cout << "Unexpected input: " << i1 << " and " << i2 << std::endl;
    }
    return result;
  } // get_overlap(const interval&) const

  //
  // Return the interval created by subtracting the interval i2 from 
  // interval i1.  If i1 is empty, returns empty.  If i2 is empty, returns i1.
  //
  template <class T>
  interval<T> operator-(const interval<T>& i1, const interval<T>& i2)
  {
    if( i1.empty() || i2.empty() )
    {
      // If the interval is empty, it can't overlap, and thus cannot have
      // another interval subtracted from it.
      //
      // If the second interval is empty, then subtraction will do nothing.
      //
      // Both of these cases should work properly by just returning i1
      return i1;
    }
    
    // If the the minimum component of i1 is less than i2, the entire region
    // from i1.begin() to i2.begin() is all that remains after subtraction.
    // That is, IFF they overlap, if not, then the subtraction is the same as
    // the original i1.
    if( i1.begin() <= i2.begin() )
    {
      return(interval<T>(i1.begin(),std::min(i1.end(),i2.begin())));
    }
    // If the the maximum component of i1 is greater than i2, the entire
    // region from i2.end() to i1.end() is all that remains after
    // subtraction.
    // That is, IFF they overlap, if not, then the subtraction is the same as
    // the original i1.    
    else if( i2.end() <= i1.end() )
    {
      return(interval<T>(std::max(i1.begin(),i2.end()),i1.end()));
    }
    // FIXME! logic and implementation.  What case is left?
    /*
    else
    {
      return(interval<T>(std::min(i1.begin(),i2.begin()),
			 std::max(i1.end(),i2.end())));
    }
    */
    // Return an empty interval...
    return interval<T>();
  } // operator-(const interval&, const interval&)


  //
  // Global comparison operators:
  // (interval, interval), (interval, template U), (template U, interval)
  //
  template <class T>
  bool operator <(const interval<T>& t1, const interval<T>& t2)
  {
    if( t1.empty() || t2.empty() )
    {
      return false;
    }

    // Note that the <= is actually correct here, as being ON the endpoint is
    // not being within the interval.    
    return (t1.begin() <= t2.begin()) && (t1.end() <= t2.begin());
  }

  template <class T>
  bool operator >(const interval<T>& t1, const interval<T>& t2)
  {
    if( t1.empty() || t2.empty() )
    {
      return false;
    }

    // Note that the <= is actually correct here, as being ON the endpoint is
    // not being within the interval.    
    return (t2.begin() <= t1.begin()) && (t2.end() <= t1.begin());
  }

  template <class T, class U>
  bool operator <(const interval<T>& t1, const U& u)
  {
    if( t1.empty() )
    {
      return false;
    }
    // Note that the <= is actually correct here, as being ON the endpoint is
    // not being within the interval.
    return (t1.end() <= u);
  }

  template <class T, class U>
  bool operator >(const interval<T>& t1, const U& u)
  {
    if( t1.empty() )
    {
      return false;
    }
    // Note that the <= is actually correct here, as being ON the endpoint is
    // not being within the interval.
    return (u <= t1.begin());
  }

  template <class T, class U>
  bool operator <(const U& u, const interval<T>& t1)
  {
    if( t1.empty() )
    {
      return false;
    }
    // Note that the <= is actually correct here, as being ON the endpoint is
    // not being within the interval.    
    return (u <= t1.begin());
  }

  template <class T, class U>
  bool operator >(const U& u, const interval<T>& t1)
  {
    if( t1.empty() )
    {
      return false;
    }
    // Note that the <= is actually correct here, as being ON the endpoint is
    // not being within the interval.    
    return (t1.end() <= u);
  }


  template <class T>
  std::ostream& operator << (std::ostream& o, const interval<T>& i)
  {
    // Note: These should probably be changed to reflect the current behavior
    // of being non-inclusive in endpoints.
    o << "[";    
    if(i.empty())
    {
      o << "empty";
    }
    else
    {
      o << i.begin() << "," << i.end();
    }
    o << "]";
    return o;
  } // operator << (ostream&,const interval&)

} // namespace amethyst
#endif /* !defined(AMETHYST__INTERVAL_HPP) */
