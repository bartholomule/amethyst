/*
 * $Id: intersection_info.hpp,v 1.4 2004/03/21 23:25:02 kpharris Exp $
 *
 * Part of "Amethyst" a playground for graphics development
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

#if       !defined(AMETHYST__INTERSECTION_INFO_HPP)
#define            AMETHYST__INTERSECTION_INFO_HPP

namespace amethyst
{ 
  template <class T>
  class shape;

  /**
   * 
   * Information about an intersection (point-based intersection).
   * 
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.4 $
   * 
   */
  template<class T>
  class intersection_info
  {

  private:

  protected:
    const shape<T>* shape_hit;
    T distance;
    
  public:
    /** Default constructor */
    intersection_info();

    /** Destructor */
    virtual ~intersection_info();

    /** Copy constructor */
    intersection_info(const intersection_info& old);

    /** Assignment operator */
    intersection_info& operator= (const intersection_info& old);

    void set(const shape<T>* hit, T dist);

    const shape<T>* get_shape() const { return shape_hit; }
    T get_distance() const { return distance; }
    
    void set_shape(const shape<T>* s) { shape_hit = s; }
    void set_distance(T dist) { distance = dist; }
    
    
  }; // class intersection_info



  //------------------------------------------------
  // Default constructor for class intersection_info
  //------------------------------------------------
  template<class T>
  intersection_info<T>::intersection_info():
    shape_hit(NULL),
    distance(-std::numeric_limits<T>::infinity())
  {
  
  } // intersection_info()

  //---------------------------------------
  // Destructor for class intersection_info
  //---------------------------------------
  template<class T>
  intersection_info<T>::~intersection_info()
  {
  
  } // ~intersection_info()

  //---------------------------------------------
  // Copy constructor for class intersection_info
  //---------------------------------------------
  template<class T>
  intersection_info<T>::intersection_info(const intersection_info<T>& old):
    shape_hit(old.shape_hit),
    distance(old.distance)
  {

  } // intersection_info(intersection_info)

  //------------------------------------------------
  // Assignment operator for class intersection_info
  //------------------------------------------------
  template<class T>
  intersection_info<T>& intersection_info<T>::operator= (const intersection_info<T>& old)
  {
    // Generic check for self-assignment
    if( &old != this)
    {
      shape_hit = old.shape_hit;
      distance = old.distance;
      
    }
    return (*this);
  } // intersection_info::operator=(intersection_info)


  template<class T>
  void intersection_info<T>::set(const shape<T>* hit, T dist)
  {
    shape_hit = hit;
    distance = dist;
  }
  
} // namespace amethyst


#endif /* !defined(AMETHYST__INTERSECTION_INFO_HPP) */

