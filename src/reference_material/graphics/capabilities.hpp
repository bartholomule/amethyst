/*
 * $Id: capabilities.hpp,v 1.1 2004/05/17 07:17:04 kpharris Exp $
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

#if       !defined(AMETHYST__CAPABILITIES_HPP)
#define            AMETHYST__CAPABILITIES_HPP

#include <string>

namespace amethyst
{ 
  struct intersection_capabilities
  {
    enum capabilities
    {
      NONE=0,
      HIT_FIRST=1,
      HIT_ALL=2,
      HIT_CONTAINERS=4,
      NORMAL_CALCULATION=8,
      UV_CALCULATION=16,
      LOCAL_SYSTEM_CALCULATION=32
    };

    capabilities my_capabilities;

    intersection_capabilities() : my_capabilities(NONE) { }
    intersection_capabilities(capabilities c): my_capabilities(c) { }
    std::string to_string() const;
    std::string to_string_short() const;

    intersection_capabilities& operator |=(const capabilities c);
    intersection_capabilities& operator |=(const intersection_capabilities& c);
    intersection_capabilities& operator &=(const capabilities c);
    intersection_capabilities& operator &=(const intersection_capabilities& c);
  }; // struct intersection_capabilities

  bool operator &(const intersection_capabilities& c1, intersection_capabilities::capabilities c2);
  bool operator &(intersection_capabilities::capabilities c1, const intersection_capabilities& c2);
  intersection_capabilities operator &(const intersection_capabilities& c1, const intersection_capabilities& c2);
  intersection_capabilities operator |(const intersection_capabilities& c1, const intersection_capabilities& c2);
  intersection_capabilities operator ~(const intersection_capabilities::capabilities& c);
  intersection_capabilities operator ~(const intersection_capabilities& c);
  
  bool operator==(const intersection_capabilities& i1,
                  const intersection_capabilities& i2);
  bool operator!=(const intersection_capabilities& i1,
                  const intersection_capabilities& i2);
  
  struct object_capabilities
  {
    enum capabilities
    {
        NONE=0,
        INFINITE=1,
        BOUNDABLE=2,
        MOVABLE=4,
        POLYGONIZATION=8,
        CONTAINER=16,
        IMPLICIT=32,
        SIMPLE=64 // A simple shape with fast intersection test (sphere, plane, etc).
    };
    capabilities my_capabilities;

    object_capabilities() : my_capabilities(NONE) { }
    object_capabilities(capabilities c): my_capabilities(c) { }
    std::string to_string() const;
    std::string to_string_short() const;

    object_capabilities& operator |=(const capabilities c);
    object_capabilities& operator |=(const object_capabilities& c);
    object_capabilities& operator &=(const capabilities c);
    object_capabilities& operator &=(const object_capabilities& c);    
  };

  bool operator &(const object_capabilities& c1, object_capabilities::capabilities c2);
  bool operator &(object_capabilities::capabilities c1, const object_capabilities& c2);
  object_capabilities operator &(const object_capabilities& c1, const object_capabilities& c2);
  object_capabilities operator |(const object_capabilities& c1, const object_capabilities& c2);
  object_capabilities operator ~(const object_capabilities::capabilities& c);
  object_capabilities operator ~(const object_capabilities& c);

  bool operator==(const object_capabilities& i1,
                  const object_capabilities& i2);
  bool operator!=(const object_capabilities& i1,
                  const object_capabilities& i2);
  
} // namespace amethyst
#endif /* !defined(AMETHYST__CAPABILITIES_HPP) */
