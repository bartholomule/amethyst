/*
 * $Id: interval.hpp,v 1.1 2003/09/24 06:01:29 kpharris Exp $
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

#if       !defined(__AMETHYST__INTERVAL_HPP__)
#define            __AMETHYST__INTERVAL_HPP__

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
*/

#include <algorithm>
#include <iosfwd>

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
  //      any other type that can be compared to a T by means of the > and <
  //      operators). 
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
    bool is_backwards;

  public:
    //
    // Default constructor -- Sets the interval to be empty
    //
    explicit interval(bool empty = true) :
      a(T(0)), b(T(0)), is_empty(empty), is_backwards(false)
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
        is_backwards = true;
      }
      else
      {
        is_backwards = false;
      }
    } // interval(T,T)

    //
    // Find out if a point is inside the interval (generic)
    //
    template <class U>
    bool inside(U d) const
    {
      if( is_empty )
      {
        return false;
      }
      if( !is_backwards )
      {
        return((d >= a) && (d <= b));
      }
      else
      {
        return((d >= b) && (d <= a));
      }
    } // inside(T) const
  
    //
    // Find out if a point is outside the interval (generic)
    //
    template <class U>
    bool outside(U d) const
    {
      if( is_empty )
      {
        return false;
      }
      if( !is_backwards )
      {
        return((d < a) || (d > b));
      }
      else
      {
        return((d > a) || (d < b));
      }
    } // outside(T) const
    

    //
    // Find out if two intervals overlap
    //
    bool overlaps(const interval& i2) const
    {
      if( is_empty )
      {
        return false;
      }
      return(i2.inside(a) || i2.inside(b));
    } // overlaps(const interval&) const

    // 
    // Return if the interval i2 is a subset of this interval.
    //
    bool subset(const interval& i2) const
    {
      if( is_empty || i2.is_empty )
      {
        return false;
      }
      return(inside(i2.a) && inside(i2.b));
    } // subset(const interval&) const

    bool empty() const { return is_empty; }
    T begin() const { return a; }
    T end()   const { return b; }

    void set(T begin, T end)
    {
      a = begin; b = end;
      if( a > b )
      {
        is_backwards = true;
      }
      is_empty = false;
    } // set(T,T)

    bool backwards() const { return is_backwards; }

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
  
    if( i1.backwards() )
    {
      result = get_overlap(interval<T>(i1.end(),i1.begin()),i2);
      result.set(result.end(), result.begin());
    }
    else if( i2.backwards() )
    {
      result = get_overlap(i1, interval<T>(i2.end(),i2.begin()));
    }
    else
    {
      if( i2.inside(i1.begin()) )
      {
        result = interval<T>(i1.begin(), std::min(i1.end(),i2.end()));
      }
      else if( i2.inside(i1.end()) )
      {
        result = interval<T>(std::max(i1.begin(),i2.begin()),i1.end());      
      }
      else // a is not inside, b is not inside... No overlap..
      {
      }
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
    else if( i1.subset(i2) ) 
    {
      // FIXME!
      // Hmm... subtracting a complete subset should result in two disjoint
      // intervals. As this is not possible, we'll return some bogus answer.    
      return i2;
    }
    else if( i2.subset(i1) )
    {
      // FIXME!
      // See the comment above in i1.subset(i2).
      return i1;
    }
    else
    {
      // Hmm... It would be easier if backwards intervals were not allowed.
      // Then the i1f and i2f would not be needed, and the subtraction would be
      // faster (as no copies/swapping are needed).
      interval<T> i1f(i1);
      interval<T> i2f(i2);
      if( i1.backwards() )
      {
        i1f.set(i1.end(), i1.begin());
      }
      if( i2.backwards() )
      {
        i2f.set(i2.end(), i2.begin());
      }    

      // If the the minimum component of i1 is less than i2, the entire region
      // from i1f.begin() to i2f.begin() is all that remains after subtraction.
      if( i1f.begin() < i2f.begin() )
      {
        return(interval<T>(i1f.begin(),i2f.begin()));
      }
      // If the the maximum component of i1 is greater than i2, the entire
      // region from i2f.end() to i1f.end() is all that remains after
      // subtraction. 
      else if( i1f.end() > i2f.end() )
      {
        return(interval<T>(i2f.end(),i1f.end()));
      }
      // FIXME! (comments and logic)
      // I can't think of when this case will be applied... Both subsets, >,
      // and < have already been tested...  They can't be empty. What would
      // cause this?  Should this case be removed?
      else
      {
        // FIXME! What should be done in such a case?
        return(interval<T>(std::min(i1f.begin(),i2f.begin()),
               std::max(i1f.end(),i2f.end())));
      }
    }
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
    
    if( !t2.backwards() )
    {
      return (t1.begin() < t2.begin()) && (t1.end() < t2.begin());
    }
    else
    {
      return (t1.begin() < t2.end()) && (t1.end() < t2.end());
    }  
  }

  template <class T>
  bool operator >(const interval<T>& t1, const interval<T>& t2)
  {
    if( t1.empty() || t2.empty() )
    {
      return false;
    }

    if( !t2.backwards() )
    {
      return (t1.begin() > t2.begin()) && (t1.end() > t2.begin());
    }
    else
    {
      return (t1.begin() > t2.end()) && (t1.end() > t2.end());
    }  
  }

  template <class T, class U>
  bool operator <(const interval<T>& t1, const U& u)
  {
    if( t1.empty() )
    {
      return false;
    }
    return (t1.begin() < u) && (t1.end() < u);
  }

  template <class T, class U>
  bool operator >(const interval<T>& t1, const U& u)
  {
    if( t1.empty() )
    {
      return false;
    }
    return (t1.begin() > u) && (t1.end() > u);
  }

  template <class T, class U>
  bool operator <(const U& u, const interval<T>& t1)
  {
    return (u < t1.begin()) && (u < t1.end());
  }

  template <class T, class U>
  bool operator >(const U& u, const interval<T>& t1)
  {
    return (u > t1.begin()) && (u > t1.end());
  }


  template <class T>
  std::ostream& operator << (std::ostream& o, const interval<T>& i)
  {
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
#endif /* !defined(__AMETHYST__INTERVAL_HPP__) */
