#if       !defined(KH_INTERVAL_H)
#define            KH_INTERVAL_H

/*
  interval.h -- A simple class for use in creating/testing intervals.
 */
/*
  Revision History:
  18May2001 Created this file, "Templatized" the class... Tested it some.
            Complicated it some more (allowing backwards intervals)...
  08Jan2002 Added quite a few comments, before I forget what is going on.
 */

#include "template_functions.h"
#include <iosfwd>

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
//
template <class T>
class interval
{
public:
  //
  // Default constructor -- Sets the interval to be empty
  //
  explicit interval(bool empty = true) : is_empty(empty), is_backwards(false)
  {
  }

  //
  // Secondary constructor -- Sets an interval to a given range
  //
  interval(T a, T b) : is_empty(false)
  {
    this->a = a;
    this->b = b;
    if(b < a)
      is_backwards = true;
    else
      is_backwards = false;
  } // interval(T,T)

  //
  // Find out if a point is inside the interval
  //
  bool inside(T d) const
  {
    if(is_empty)
      return false;
    if(!is_backwards)
      return((d >= a) && (d <= b));
    else
      return((d >= b) && (d <= a));      
  } // inside(T) const
  
  //
  // Find out if a point is outside the interval
  //
  bool outside(T d) const
  {
    if(is_empty)
      return false;
    if(!is_backwards)    
      return((d < a) || (d > b));
    else
      return((d > a) || (d < b));      
  } // outside(T) const

  //
  // Find out if two intervals overlap
  //
  bool overlaps(const interval& i2) const
  {
    if(is_empty)
      return false;    
    return(i2.inside(a) || i2.inside(b));
  } // overlaps(const interval&) const

  // 
  // Return if the interval i2 is a subset of this interval.
  //
  bool subset(const interval& i2) const
  {
    if(is_empty)
      return false;
    if(i2.is_empty)
      return false;
    return(inside(i2.a) && inside(i2.b));
  } // subset(const interval&) const


  //
  // Return the overlap of this interval and the interval i2
  //
  interval get_overlap(const interval& i2) const
  {
    interval result; // result interval (empty by default)

    // If the interval is empty, return an empty interval, as it can't overlap.
    if(is_empty)
    {
      return result;
    }
    
    if(is_backwards)
    {
      result = interval(b,a).get_overlap(i2);
      my_swap(result.a, result.b);
    }
    else if(i2.is_backwards)
    {
      result = get_overlap(interval(i2.b,i2.a));
    }
    else
    {
      if(i2.inside(a))
      {
	result = interval(a,my_min(b,i2.b));
      }
      else if(i2.inside(b))
      {
	result = interval(my_max(a,i2.a),b);      
      }
      else // a is not inside, b is not inside... No overlap..
      {
      }
    }
    return result;
  } // get_overlap(const interval&) const

  //
  // Return the interval created by subtracting the interval i2 from this
  // interval.  If this is empty, returns empty.  If i2 is empty, returns this.
  //
  interval subtract(const interval& i2) const
  {
    // If the interval is empty, it can't overlap, and thus cannot have another
    // interval subtracted from it.
    if(is_empty)
    {
      return interval();
    }
    else if(i2.is_empty)
    {
      return(*this);
    }
    else if(subset(i2))
    {
      return i2;
    }
    else if(i2.subset(*this))
    {
      return *this;
    }
    else
    {
      if(a < i2.a)
      {
	return(interval(a,i2.a));
      }
      else if(b > i2.b)
      {
	return(interval(i2.b,b));
      }
      else
      {
	return(interval(my_min(a,i2.a),my_max(b,i2.b)));
      }
    }
  } // subtract(const interval&) const
  
  bool empty() const { return is_empty; }
  T begin() const { return a; }
  T end()   const { return b; }
  void set(T begin, T end)
  {
    a = begin; b = end;
    if(a > b)
    {
      T temp = a; a = b; b = temp;
    }
    is_empty = false;
  } // set(T,T)
  
  friend std::ostream& operator << <>(std::ostream& o, const interval<T>& i);

private:
  T a;
  T b;
  bool is_empty;
  bool is_backwards;
}; // class interval<T>


template <class T>
std::ostream& operator << (std::ostream& o, const interval<T>& i)
{
  o << "[";    
  if(i.is_empty)
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

#endif /* !defined(KH_INTERVAL_H) */
