/*
 * $Id: raster.hpp,v 1.4 2004/04/07 05:10:05 kpharris Exp $
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

#if       !defined(AMETHYST__RASTER_HPP)
#define            AMETHYST__RASTER_HPP

#include <textized.hpp>
#include <extra_exceptions.hpp>
#include <template_functions.hpp>
#include <useful_functions.hpp>
#include <string>

namespace amethyst
{
  template <class T> class scanline;

  /**
   * 
   * A generic class to represent a raster (2d matrix).  This is useful for
   * raster graphics and possibly lots of other fun stuff.  Note:  It does NOT
   * have iterators (at this time).
   * 
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.4 $
   * 
   */
  template <class T>
  class raster
  {

  private:
    /** The width and height of the raster */
    unsigned width, height;
    /** The actual raster data. */
    T* raster_data;

  public:
    /** Default constructor */
    raster() throw();
    /** Size-specified constructor.  Creates a width by height sized raster */
    raster(unsigned width, unsigned height);

    /** Destructor */
    virtual ~raster();

    /** Copy constructor */
    raster(const raster& old);

    /** Assignment operator */
    raster& operator= (const raster& old);

    /**
     * Get the element at the given (x,y)
     * @param x The column.
     * @param y The row.
     * @throws out_of_range if either x or y is out of range.
     */
    T& operator()(unsigned x, unsigned y) throw(out_of_range);
    /**
     * Get the element at the given (x,y)
     * @param x The column.
     * @param y The row.
     * @throws out_of_range if either x or y is out of range.
     */    
    const T& operator()(unsigned x, unsigned y) const
      throw(out_of_range);

    /**
     * Get the scanline at the given row (y)
     * @param y The row of the scanline.
     * @returns A scanline for the desired row.
     * @throws out_of_range if y is out of range (height).
     */
    scanline<T> operator[](unsigned y) throw(out_of_range);

    /**
     * Get the scanline at the given row (y)
     * @param y The row of the scanline.
     * @returns A scanline for the desired row.
     * @throws out_of_range if y is out of range (height).
     */
    const scanline<T> operator[](unsigned y)const throw(out_of_range);

    /**
     * Return a raster which contains the elements in the range
     * (x1,y1) to (x2,y2) inclusive.
     * @throws out_of_range if any of x1, y1, x2, or y2 are out of range.
     * @returns a new raster that contains the data from the specified region.
     */
    raster<T> sub_raster(unsigned x1, unsigned y1,
                         unsigned x2, unsigned y2) const throw(out_of_range); 

    /**
     * Resize the current raster to the given width and height.  If preserve is
     * given, the current data (if any), is copied across. If either width or
     * height is zero, the current raster will be resized to 0.
     */
    void resize(unsigned width, unsigned height, bool preserve = true);

    /**
     * Resize the current raster to the given width and height, shifting old
     * data to be centered at cx,cy in the newly sized raster, and filling the
     * rest with the given fill data.
     * 
     */
    void resize(unsigned width, unsigned height,
                int cx, int cy, const T& fill);

    /**
     *
     * Perform a reinterpretation of the data that this class is using.
     * This is NOT good programming practice, however, it is VERY useful for
     * getting data out in a method that can be used for display/conversion in
     * pre-existing graphics libraries.
     *
     * WARNING! Do not delete or otherwise attempt to deallocate the return
     * value of this function.  Doing so will cause serious problems.  Use this
     * function at your own peril.  Don't blame me when something bad happens
     * if you use this.
     *
     * NOTE: The ability to successfully reinterpret the data contained within
     * depends on the type used for this class.  Use of built-in types is
     * strongly encouraged, or possibly the rgbcolor class in rgbcolor.hpp
     * could be used, as it was written specifically for this purpose.
     *
     */
    template <class U>
    U reinterpret();

    /** Get the width of the raster */
    unsigned get_width()  const
    {
      return width;
    }
    /** Get the height of the raster */
    unsigned get_height() const
    {
      return height;
    }
    /** Get the number of pixels in the raster */
    unsigned get_numpixels() const
    {
      return width * height;
    } 
    /** Return if the raster has no data. */
    bool empty() const
    {
      return(raster_data == NULL);
    }

  }; // class raster



  /**
   * An internal class of class raster which is used to provide simple
   * multi-dimisionality.
   *
   * This class CANNOT be used seperately from class raster.
   *
   * This makes it so that when the [] operator of raster is called, the []
   * operator can then be applied again (to the resulting scanline) to get data
   * from the 'second dimension'.
   *
   * Once a scanline is created, it will ALWAYS refer to the scanline it was
   * created for.  It cannot be changed to modify another raster, nor can the
   * row number be changed.
   *
   * @author Kevin Harris
   * @version $Revision: 1.4 $
   */
  template <class T>
  class scanline
  {
  public:
    friend class raster<T>;

    /**
     * Return an element of the raster.
     * @param x The index of the element on this scanline.
     * @throws out_of_range if x is out of range
     * @returns The element requested.
     */
    T& operator[](unsigned x) throw(out_of_range);


    /**
     * Return an element of the raster.
     * @param x The index of the element on this scanline.
     * @throws out_of_range if x is out of range
     * @returns The element requested.
     */      
    const T& operator[](unsigned x) const throw(out_of_range);

    /**
     * Tricky assignment operator, will copy one row (scanline) of any raster
     * to another (if they are the same size).
     * NOTE: The reference is NOT copied, and this scanline will ALWAYS
     * belong to the raster that created it.
     * @param line Another scanline to use for copying data.  This is NOT
     *   kept like the natural way of thinking for a real assignment.  Both
     *   references remain exactly as they were, just the raster has had its
     *   data modified.
     * @returns A reference to this scanline, for chained assignments.
     * @throws size_mismatch if the sizes of the scanlines (widths) are not
     *   the same.
     */  
    scanline& operator=(const scanline& line) throw(size_mismatch);

  private:
    /**
     * The constructor that only class raster can invoke.
     * @param ras A reference to a raster for which this instance will ALWAYS
     *   contain.  Any actions on this scanline will always affect the given
     *   raster.
     * @param row The row that this scanline represents within the raster.
     */
    scanline(raster<T>& ras, unsigned row);

    /**
     * The reference to the class which created this particular instance.
     * Once this is set in the constructor, it will not ever be changed.
     */
    raster<T>& my_raster;
    /**
     * The row (line) that this scanline is supposed to represent.
     * This is, like the above, permanent.
     */
    unsigned my_row;
  }; // class scanline


  // **********************************************************************
  // Member functions for class raster
  // **********************************************************************

  //--------------------------------------
  // Default constructor for class raster
  //--------------------------------------
  template <class T>
  raster<T>::raster() throw():
    width(0), height(0),
    raster_data(NULL)
  {
  } // raster()

  //--------------------------------------
  // Default constructor for class raster
  //--------------------------------------
  template <class T>
  raster<T>::raster(unsigned w, unsigned h):
    width(w), height(h),
    raster_data(NULL)
  {
    unsigned linear_size = w * h;

    if( linear_size > 0 )
    {
      raster_data = new T[linear_size];
    }
    else
    {
      width = 0;
      height = 0;
    }
  } // raster(unsigned,unsigned)

  //-----------------------------
  // Destructor for class raster
  //-----------------------------
  template <class T>
  raster<T>::~raster()
  {
    // note: deletion of NULL is supposed to be safe.
    delete[] raster_data;
    raster_data = NULL;
    width = 0;
    height = 0;
  } // ~raster()

  //-----------------------------------
  // Copy constructor for class raster
  //-----------------------------------
  template <class T>
  raster<T>::raster(const raster<T>& old):
    width(old.width),
    height(old.height),
    raster_data(NULL)
  {
    unsigned linear_size = width * height;

    if( linear_size > 0 )
    {
      raster_data = new T[linear_size];

      if( old.raster_data )
      {
        for( unsigned i = 0; i < linear_size; ++i )
        {
          raster_data[i] = old.raster_data[i];
        }
      }
    }
    else
    {
      width = 0;
      height = 0;
    }
  } // raster(raster)



  //--------------------------------------
  // Assignment operator for class raster
  //--------------------------------------
  template <class T>
  raster<T>& raster<T>::operator= (const raster<T>& old)
  {
    // Generic check for self-assignment
    if( &old != this )
    {
      delete[] raster_data;

      width = old.width;
      height = old.height;

      unsigned linear_size = width * height;
      // Check to see that the size is non-zero, and make sure no null-data
      // crept into the old copy.
      if( (linear_size > 0) && old.raster_data )
      {
        raster_data = new T[linear_size];

        for( unsigned i = 0; i < linear_size; ++i )
        {
          raster_data[i] = old.raster_data[i];
        }
      }
      else
      {
        raster_data = NULL;
        width = 0;
        height = 0;
      }
    }
    return(*this);
  } // raster::operator=(raster)

  template<class T>
  inline T& raster<T>::operator()(unsigned x, unsigned y) throw(out_of_range)
  {
    if( (x < width) && (y < height) )
    {
      return raster_data[x + (y * width)];
    }
    else
    {
      char buffer[1024];
      snprintf(buffer,1024,"raster<T>::op()(%d,%d): %s", x, y,
               intl("index is out of range"));
      throw out_of_range(std::string(buffer));
    }
  }

  template <class T>
  inline const T& raster<T>::operator()(unsigned x, unsigned y) const
    throw(out_of_range)
  {
    if( (x < width) && (y < height) )
    {
      return raster_data[x + (y * width)];
    }
    else
    {
      char buffer[1024];
      snprintf(buffer,1024,"raster<T>::op()(%d,%d)const: %s", x, y,
               intl("index is out of range"));
      throw out_of_range(std::string(buffer));
    }    
  }

  template <class T>
  inline scanline<T> raster<T>::operator[](unsigned y) throw(out_of_range)
  {
    if( y < height )
    {
      return scanline<T>(*(this), y);
    }
    else
    {
      char buffer[1024];
      snprintf(buffer,1024,"raster<T>::op[](%d): %s", y,
               intl("index is out of range"));
      throw out_of_range(std::string(buffer));
    }    
  }

  template <class T>
  inline const scanline<T> raster<T>::operator[](unsigned y) const
    throw(out_of_range)
  {
    if( y < height )
    {
      return scanline<T>(*const_cast<raster<T>*>(this), y);
    }
    else
    {
      char buffer[1024];
      snprintf(buffer,1024,"raster<T>::op[](%d)const: %s", y,
               intl("index is out of range"));
      throw out_of_range(std::string(buffer));
    }    
  }

  template <class T>
  raster<T> raster<T>::sub_raster(unsigned x1, unsigned y1,
                                  unsigned x2, unsigned y2) const
    throw(out_of_range)
  {
    if( (x2 < width) && (y2 < height) )
    {
      if( (x1 < x2) && (y1 < y2) )
      {
        // Both of these add 1, because it is inclusive...
        unsigned x_range = x2 - x1 + 1; 
        unsigned y_range = y2 - y1 + 1;

        raster<T> ret_ras(x_range, y_range);

        for( unsigned y = 0; y < y_range; ++y )
        {
          for( unsigned x = 0; x < x_range; ++x )
          {
            unsigned source = (x + x1) + ((y + y1) * width);
            ret_ras.raster_data[x + y * x_range] = raster_data[source];
          }
        }
        return ret_ras;
      }
      else
      {
        throw out_of_range(std::string("raster<T>::sub_raster(...): ") +
                           intl("dimensions are incorrectly ordered"));
      }
    }
    else
    {
      throw out_of_range(std::string("raster<T>::sub_raster(...): ") +
                         intl("index is out of range"));       
    }
  }

  template <class T>
  void raster<T>::resize(unsigned width, unsigned height, bool preserve)
  {
    unsigned old_width = this->width;
    unsigned old_height = this->height;
    T* old_data = raster_data;


    // If there is no change in size, do nothing but return.
    if( (width == old_width) && (height == old_height) )
    {
      return;
    }

    if( (width > 0) && (height > 0) )
    {
      this->width = width;
      this->height = height;
      raster_data = new T[width * height];
    }
    else
    {
      // Destroy the current raster, as a zero-sized resize was given.
      this->width = 0;
      this->height = 0;
      raster_data = NULL;
    }

    // If it was requested to preserve data, copy as much as possible...
    if( preserve && old_data && raster_data )
    {
      unsigned max_x = my_min(old_width, width);
      unsigned max_y = my_min(old_height, height);
      for( unsigned y = 0; y < max_y; ++y )
      {
        for( unsigned x = 0; x < max_x; ++x )
        {
          raster_data[(y * width) + x] = old_data[(y * old_width) + x];
        }
      }
    }

    // Delete the old data (if any). Deleting NULL should be safe.
    delete[] old_data;
  }

  template <class T>
  void raster<T>::resize(unsigned width, unsigned height,
                         int cx, int cy, const T& fill)
  {
    unsigned old_width = this->width;
    unsigned old_height = this->height;
    unsigned old_mid_x = old_width / 2;
    unsigned old_mid_y = old_height / 2;
    T* old_data = raster_data;

    // If there is no change, return, doing nothing.
    if( (old_width == width) && (old_height == height) &&
        (old_mid_x == cx) && (old_mid_y == cy) )
    {
      return;
    }

    if( (width > 0) && (height > 0) )
    {
      this->width = width;
      this->height = height;
      raster_data = new T[width * height];
    }
    else
    {
      // Destroy the current raster, as a zero-sized resize was given.
      this->width = 0;
      this->height = 0;
      raster_data = NULL;
    }

    // If it is possible to preserve data, copy as much as possible, and fill
    // the rest.
    if( old_data && raster_data )
    {
      int olx = cx - old_mid_x;
      int oux = cx + (old_width - old_mid_x);
      int oly = cy - old_mid_y;
      int ouy = cy + (old_height - old_mid_y);

      unsigned min_x = my_max(olx, 0);
      unsigned max_x = my_min(oux, int(width));
      unsigned min_y = my_max(oly, 0);
      unsigned max_y = my_min(ouy, int(height));


      int x_shift = -olx;
      int y_shift = -oly;
      // Copy any useful data.
      for( unsigned y = min_y; y < max_y; ++y )
      {
        unsigned old_y = y + y_shift;
        unsigned old_lin_y = old_y * old_width;
        unsigned lin_y = y * width;
        for( unsigned x = min_x; x < max_x; ++x )
        {
          unsigned old_x = x + x_shift;
          raster_data[lin_y + x] = old_data[old_lin_y + old_x];
        }
      }

      // Now that the data is copied, fill the rest of the raster in the
      // largest stripes possible.  Here's what it looks like in the simple
      // case:
      //
      // 1111111111111
      // 1111111111111
      // 3333      444
      // 3333      444
      // 3333      444
      // 2222222222222
      // 2222222222222
      // 2222222222222
      //
      // Where numbers correspond to the numbered loops/tests below, and the
      // blanks are prefilled with copied data.
      //
      // These divisions were chosen to maximize the filling amount of a single
      // given loop (y-major).
      //
      // Note that the loops will not execute, and a maximum of 4 useless tests
      // will be performed, if the entire raster was filled with old data.

      // (1) Fill above the copied rectangle, if it needs it...
      for( unsigned y = 0; y < min_y; ++y )
      {
        for( unsigned x = 0; x < width; ++x )
        {
          raster_data[(y * width) + x] = fill;
        }
      }
      // (2) Fill below the copied rectangle, if it needs it...
      for( unsigned y = max_y; y < height; ++y )
      {
        for( unsigned x = 0; x < width; ++x )
        {
          raster_data[(y * width) + x] = fill;
        }
      }
      // (3) Fill the gap on the left (if any)
      if( min_x > 0 )
      {
        for( unsigned y = min_y; y < max_y; ++y )
        {
          for( unsigned x = 0; x < min_x; ++x )
          {
            raster_data[(y * width) + x] = fill;
          }
        }
      }
      // (4) Fill the gap on the right (if any)
      if( max_x < width )
      {
        for( unsigned y = min_y; y < max_y; ++y )
        {
          for( unsigned x = max_x; x < width; ++x )
          {
            raster_data[(y * width) + x] = fill;
          }
        }
      }
    }
    else
    {
      // Preservation is not possible, fill everything.
      unsigned max_linear = this->width * this->height;
      for( unsigned i = 0; i < max_linear; ++i )
      {
        raster_data[i] = fill;
      }
    }

    // Delete the old data (if any). Deleting NULL should be safe.
    delete[] old_data;
  }


  template <class T>
  template <class U>
  inline U raster<T>::reinterpret()
  {
    return reinterpret_cast<U>(raster_data);
  }

  // **********************************************************************
  // Member functions for class raster::scanline
  // **********************************************************************

  //------------------------------------------------------------------
  // The one and only constructor for raster's internal class scanline
  // Note: this is a non-public constructor...
  //------------------------------------------------------------------
  template <class T>
  scanline<T>::scanline(raster<T>& ras, unsigned row):
    my_raster(ras),
    my_row(row)
  {
  }

  template <class T>
  inline T& scanline<T>::operator[](unsigned x) throw(out_of_range)
  {
    if( x < my_raster.get_width() )
    {
      return my_raster(x, my_row);
    }
    else
    {
      char buffer[1024];
      snprintf(buffer,1024,"raster<T>::scanline::op[](%d): %s", x,
               intl("index is out of range"));
      throw out_of_range(std::string(buffer));
    } 
  } // scanline::operator[](x)

  template <class T>
  inline const T& scanline<T>::operator[](unsigned x) const throw(out_of_range)
  {
    if( x < my_raster.get_width() )
    {
      return my_raster(x, my_row);
    }
    else
    {
      char buffer[1024];
      snprintf(buffer,1024,"raster<T>::scanline::op[](%d)const: %s", x,
               intl("index is out of range"));
      throw out_of_range(std::string(buffer));
    } 
  } // scanline::operator[](x) const  

  template <class T>
  scanline<T>& scanline<T>::operator=(const scanline& line) throw(size_mismatch)
  {
    // Generic check for self-assignment
    if( &line != this )
    {
      if( line.my_raster.get_width() == my_raster.get_width() )
      {
        for( unsigned i = 0; i < my_raster.get_width(); ++i )
        {
          my_raster(i, my_row) = line.my_raster(i, line.my_row);
        }
      }
      else
      {
        throw size_mismatch(std::string("raster::scanline::op=(scanline): ") +
                            intl("raster widths are different"));
      }
    }
    return *this;   
  } // scanline::operator=(scanline)

} // namespace amethyst




#endif /* !defined(AMETHYST__RASTER_HPP) */
