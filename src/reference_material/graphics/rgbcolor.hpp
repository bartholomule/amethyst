/*
 * $Id: rgbcolor.hpp,v 1.1 2003/09/17 10:13:31 kpharris Exp $
 *
 * Part of "Many Games" - A nearly infinitely expandable gaming framework
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

#if       !defined(__MANYGAMES__RGBCOLOR_HPP__)
#define            __MANYGAMES__RGBCOLOR_HPP__

namespace manygames
{ 

  /**
   * 
   * A generic class to represent RGB colors.  Note that only built-in (or
   * native) types (char, float, etc), are usable in this class.  The design of
   * this class was chosen so that array accesses ([]) and direct accesses
   * (.N()) would be almost the same speed.  Also, this class *may* be able to
   * be used in an array and reinterpreted as an array of the base type.
   *
   * For example:
   *<pre>
   *@@ rgbcolor<unsigned char> foo[100];
   *@@ unsigned char* foo_reinterpreted = reinterpret_cast<char*>(foo);
   *@@ ...
   *</pre>
   *
   * The opposite should also be possible.  In either case, BE CAREFUL to make
   * sure that the correct type of destructor would be called (especially with
   * memory allocated by 'new').
   *
   * Whatver changes or additions are made in the future, NO virtual functions
   * should be added, as they would likely break the ability to reinterpret the
   * class as an array of three ints.
   *
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.1 $
   * 
   */
  template <class T>
  class rgbcolor
  {

  protected:
    /** A struct of three items directly accessible. */
    struct rgbcolor_direct
    {
      T r; ///< Red   component of RGB color
      T g; ///< Green component of RGB color
      T b; ///< Blue  component of RGB color
    };
    /** A struct of three items array accessible. */    
    struct rgbcolor_array
    {
      T components[3]; ///< Components of RGB in an 'array'
    };
    /**
     * A union to allow accesses to both indirectly through an array, and
     * directly athrough a name, without adding any extra processing time or
     * space requirements
     */        
    union rgbcolor_union
    {
      rgbcolor_union() { }
      /** A constructor, to simplify things. */
      rgbcolor_union(T r, T g, T b)
      {
	direct.r = r;
	direct.g = g;
	direct.b = b;	
      }
      /** Operator for simplification of access. */
      T& operator[](unsigned index)       { return array.components[index]; }
      /** Operator for simplification of access. */
      T  operator[](unsigned index) const { return array.components[index]; }

      rgbcolor_direct direct; ///< The directly accessible part
      rgbcolor_array  array;  ///< The array accessible part
    };

    /** The actual RGB data */
    rgbcolor_union rgb;

  public:
    /** Default constructor (does nothing) */
    rgbcolor() { }
    
    /** Secondary constructor (initializes) */
    rgbcolor(T r, T g, T b):rgb(r,g,b) { }    

    /** Destructor -- NON VIRTUAL! (does nothing) */
    ~rgbcolor() { }

    /** Copy constructor */
    rgbcolor(const rgbcolor& old);

    /** Assignment operator */
    rgbcolor& operator= (const rgbcolor& old);

    inline void set_r(T r) { rgb.direct.r = r; } ///< Set the red component
    inline void set_g(T g) { rgb.direct.g = g; } ///< Set the green component
    inline void set_b(T b) { rgb.direct.b = b; } ///< Set the blue component
    inline T r() const { return rgb.direct.r; } ///< Get the red component
    inline T g() const { return rgb.direct.g; } ///< Get the green component
    inline T b() const { return rgb.direct.b; } ///< Get the blue component

    
    
    /** Get the element specified.  No bounds checking is performed */
    inline T& operator[](unsigned index)       { return rgb[index]; }    
    /** Get the element specified.  No bounds checking is performed */
    inline T  operator[](unsigned index) const { return rgb[index]; }

    /** Set the components to the given values */
    void set(T r, T g, T b);

    rgbcolor& operator *=(T factor);          ///< Multiply by factor
    rgbcolor& operator /=(T factor);          ///< Divide by factor
    template <class U>
    rgbcolor& operator *=(U factor);          ///< Multiply by factor (general)
    template <class U>    
    rgbcolor& operator /=(U factor);          ///< Divide by factor (general)
    rgbcolor& operator +=(const rgbcolor& r); ///< Add the given to these
    rgbcolor& operator -=(const rgbcolor& r); ///< Sub the given from these
    
  }; // class rgbcolor

  /** Copy constructor */
  template <class T>
  rgbcolor<T>::rgbcolor(const rgbcolor& old):
    rgb(old.r(), old.g(), old.b())
  {
  } // rgbcolor::rgbcolor(rgbcolor)

  /** Assignment operator */
  template <class T>
  rgbcolor<T>& rgbcolor<T>::operator=(const rgbcolor& old)
  {
    // The generic check for self-assignment has been removed, as it has
    // already been specified that the type given MUST be a native type.  Thus,
    // it should be really fast to do the assignment, and the branch could be
    // very slow.
    rgb.direct.r = old.rgb.direct.r;
    rgb.direct.g = old.rgb.direct.g;
    rgb.direct.b = old.rgb.direct.b;
    return (*this);    
  } // rgbcolor::operator=(rgbcolor)

  /** Sets the data... */
  template <class T>
  void rgbcolor<T>::set(T r, T g, T b)
  {
    rgb.direct.r = r;
    rgb.direct.g = g;
    rgb.direct.b = b;
  } // rgbcolor::set(r,g,b)

  /** Multiplication by a factor/assign (specific) */
  template <class T>
  rgbcolor<T>& rgbcolor<T>::operator *=(T factor)
  {
    rgb.direct.r *= factor;
    rgb.direct.g *= factor;
    rgb.direct.b *= factor;
    return *this;
  } // rgbcolor::operator*=(T)

  /** Division by a factor/assign (specific) */
  template <class T>
  rgbcolor<T>& rgbcolor<T>::operator /=(T factor)
  {
    rgb.direct.r /= factor;
    rgb.direct.g /= factor;
    rgb.direct.b /= factor;
    return *this;
  } // rgbcolor::operator/=(T)
  
  /** Multiplication by a factor/assign (general) */
  template <class T>
  template <class U>
  rgbcolor<T>& rgbcolor<T>::operator *=(U factor)
  {
    rgb.direct.r = T(rgb.direct.r * factor);
    rgb.direct.g = T(rgb.direct.g * factor);
    rgb.direct.b = T(rgb.direct.b * factor);
    return *this;
  } // rgbcolor::operator*=(U)

  /** Division by a factor/assign (general) */  
  template <class T>
  template <class U>
  rgbcolor<T>& rgbcolor<T>::operator /=(U factor)
  {
    rgb.direct.r = T(rgb.direct.r / factor);
    rgb.direct.g = T(rgb.direct.g / factor);
    rgb.direct.b = T(rgb.direct.b / factor);
    return *this;    
  } // rgbcolor::operator/=(U)

  /** addition/assign */
  template <class T>
  rgbcolor<T>& rgbcolor<T>::operator +=(const rgbcolor<T>& r)
  {
    rgb.direct.r += r.rgb.direct.r;
    rgb.direct.g += r.rgb.direct.g;
    rgb.direct.b += r.rgb.direct.b;
    return *this;
  } // rgbcolor::operator+=(rgbcolor)

  /** subtraction/assign */
  template <class T>
  rgbcolor<T>& rgbcolor<T>::operator -=(const rgbcolor<T>& r)
  {
    rgb.direct.r -= r.rgb.direct.r;
    rgb.direct.g -= r.rgb.direct.g;
    rgb.direct.b -= r.rgb.direct.b;
    return *this;
  } // rgbcolor::operator-=(rgbcolor)


  /** Multiplication of a color by a factor (non-member) */
  template <class T>
  rgbcolor<T> operator*(T factor, const rgbcolor<T>& r)
  {
    rgbcolor<T> retval(r);
    retval *= factor;
    return retval;
  } // operator*(T,rgbcolor)
  
  /** Multiplication of a color by a factor (non-member, general) */
  template <class T, class U>
  rgbcolor<T> operator*(U factor, const rgbcolor<T>& r)
  {
    rgbcolor<T> retval(r);
    retval *= factor;
    return retval;
  } // operator*(U,rgbcolor)

  /** Addition of two colors. */
  template <class T>
  rgbcolor<T> operator+(const rgbcolor<T>& c1, const rgbcolor<T>& c2)
  {
    rgbcolor<T> retval(c1);
    retval += c2;
    return retval;
  } // operator+(rgbcolor,rgbcolor)
  
  /** Subtraction of two colors. */
  template <class T>
  rgbcolor<T> operator-(const rgbcolor<T>& c1, const rgbcolor<T>& c2)
  {
    rgbcolor<T> retval(c1);
    retval -= c2;
    return retval;
  } // operator-(rgbcolor,rgbcolor)

  /** Unary negation of a color. */
  template <class T>
  rgbcolor<T> operator-(const rgbcolor<T>& c)
  {
    return rgbcolor<T>(-c.r(),-c.g(),-c.b());
  } // operator-(rgbcolor,rgbcolor)  
  
  
} // namespace manygames


#endif /* !defined(__MANYGAMES__RGBCOLOR_HPP__) */
