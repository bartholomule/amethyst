/*
 * $Id: capabilities.cpp,v 1.2 2004/06/01 03:59:31 kpharris Exp $
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

#include "capabilities.hpp"

namespace amethyst
{ 

  namespace // anonymous
  {
    void add_to_string_list(std::string& s, const std::string& item)
    {
      if( !s.empty() )
      {
        s += ", ";
      }
      s += item;
    }
  } // end anonymous namespace

  std::string intersection_capabilities::to_string() const
  {
    std::string retval;
    
    if( *this & intersection_capabilities::HIT_FIRST )
    {
      add_to_string_list(retval, "First Object Hit");
    }
    if( *this & intersection_capabilities::HIT_ALL )
    {
      add_to_string_list(retval, "All Objects Hit");
    }
    if( *this & intersection_capabilities::HIT_CONTAINERS )
    {
      add_to_string_list(retval, "Containers Hit");
    }
    if( *this & intersection_capabilities::NORMAL_CALCULATION )
    {
      add_to_string_list(retval, "Normal Calculation");
    }
    if( *this & intersection_capabilities::UV_CALCULATION )
    {
      add_to_string_list(retval, "UV Calculation");
    }
    if( *this & intersection_capabilities::LOCAL_SYSTEM_CALCULATION )
    {
      add_to_string_list(retval, "Local Coord System Calculation");
    }
    if( *this & intersection_capabilities::TIME_SAMPLING )
    {
      add_to_string_list(retval, "Time Sampling");
    }

    if( retval.empty() )
    {
      retval = "NONE";
    }
  
    return "{" + retval + "}";  
  }


  std::string intersection_capabilities::to_string_short() const
  {
    std::string retval;
    
    if( *this & intersection_capabilities::HIT_FIRST )
    {
      retval += "F";
    }
    if( *this & intersection_capabilities::HIT_ALL )
    {
      retval += "A";
    }
    if( *this & intersection_capabilities::HIT_CONTAINERS )
    {
      retval += "C";
    }
    if( *this & intersection_capabilities::NORMAL_CALCULATION )
    {
      retval += "N";
    }
    if( *this & intersection_capabilities::UV_CALCULATION )
    {
      retval += "U";
    }
    if( *this & intersection_capabilities::LOCAL_SYSTEM_CALCULATION )
    {
      retval += "L";
    }
    if( *this & intersection_capabilities::TIME_SAMPLING )
    {
      retval += "T";
    }    
  
    return retval;
  }


  intersection_capabilities& intersection_capabilities::operator |=(const intersection_capabilities::capabilities c)
  {
    my_capabilities = (intersection_capabilities::capabilities)(unsigned(c) | unsigned(my_capabilities));
  }
  intersection_capabilities& intersection_capabilities::operator |=(const intersection_capabilities& c)
  {
    my_capabilities = (intersection_capabilities::capabilities)(unsigned(c.my_capabilities) | unsigned(my_capabilities));  
  }

  intersection_capabilities& intersection_capabilities::operator &=(const intersection_capabilities::capabilities c)
  {
    my_capabilities = (intersection_capabilities::capabilities)(unsigned(c) & unsigned(my_capabilities));
  }
  intersection_capabilities& intersection_capabilities::operator &=(const intersection_capabilities& c)
  {
    my_capabilities = (intersection_capabilities::capabilities)(unsigned(c.my_capabilities) & unsigned(my_capabilities));  
  }

  bool operator &(const intersection_capabilities& c1, intersection_capabilities::capabilities c2)
  {
    return unsigned(c1.my_capabilities) & unsigned(c2);
  }
  bool operator &(intersection_capabilities::capabilities c1, const intersection_capabilities& c2)
  {
    return unsigned(c1) & unsigned(c2.my_capabilities);
  }
  intersection_capabilities operator &(const intersection_capabilities& c1, const intersection_capabilities& c2)
  {
    return intersection_capabilities(intersection_capabilities::capabilities(unsigned(c1.my_capabilities) & unsigned(c2.my_capabilities)));
  }
  intersection_capabilities operator |(const intersection_capabilities& c1, const intersection_capabilities& c2)
  {
    return intersection_capabilities(intersection_capabilities::capabilities(unsigned(c1.my_capabilities) | unsigned(c2.my_capabilities)));
  }  
  intersection_capabilities operator ~(const intersection_capabilities::capabilities& c)
  {
    return intersection_capabilities(intersection_capabilities::capabilities(~unsigned(c)));
  }

  intersection_capabilities operator ~(const intersection_capabilities& c)
  {
    return intersection_capabilities(intersection_capabilities::capabilities(~unsigned(c.my_capabilities)));
  }

  bool operator==(const intersection_capabilities& i1,
                  const intersection_capabilities& i2)
  {
    return unsigned(i1.my_capabilities) == unsigned(i2.my_capabilities);
  }
  bool operator!=(const intersection_capabilities& i1,
                  const intersection_capabilities& i2)
  {
    return unsigned(i1.my_capabilities) != unsigned(i2.my_capabilities);
  }

  std::string object_capabilities::to_string() const
  {
    std::string retval;
    
    if( *this & object_capabilities::INFINITE )
    {
      add_to_string_list(retval, "Infinite");
    }
    if( *this & object_capabilities::BOUNDABLE )
    {
      add_to_string_list(retval, "Boundable");
    }
    if( *this & object_capabilities::MOVABLE )
    {
      add_to_string_list(retval, "Movable");
    }
    if( *this & object_capabilities::POLYGONIZATION )
    {
      add_to_string_list(retval, "Polygonization");
    }
    if( *this & object_capabilities::CONTAINER)
    {
      add_to_string_list(retval, "Container");
    }
    if( *this & object_capabilities::IMPLICIT )
    {
      add_to_string_list(retval, "Implicit Surface");
    }    

    if( retval.empty() )
    {
      retval = "NONE";
    }
  
    return "{" + retval + "}";  
  }


  std::string object_capabilities::to_string_short() const
  {
    std::string retval;

    if( *this & object_capabilities::INFINITE )
    {
      retval += "I";
    }
    if( *this & object_capabilities::BOUNDABLE )
    {
      retval += "B";
    }
    if( *this & object_capabilities::MOVABLE )
    {
      retval += "M";
    }
    if( *this & object_capabilities::POLYGONIZATION )
    {
      retval += "P";
    }
    if( *this & object_capabilities::CONTAINER)
    {
      retval += "C";
    }
    if( *this & object_capabilities::IMPLICIT )
    {
      retval += "F";
    }
    return retval;
  }


  object_capabilities& object_capabilities::operator |=(const object_capabilities::capabilities c)
  {
    my_capabilities = (object_capabilities::capabilities)(unsigned(c) | unsigned(my_capabilities));
  }
  object_capabilities& object_capabilities::operator |=(const object_capabilities& c)
  {
    my_capabilities = (object_capabilities::capabilities)(unsigned(c.my_capabilities) | unsigned(my_capabilities));  
  }

  object_capabilities& object_capabilities::operator &=(const object_capabilities::capabilities c)
  {
    my_capabilities = (object_capabilities::capabilities)(unsigned(c) & unsigned(my_capabilities));
  }
  object_capabilities& object_capabilities::operator &=(const object_capabilities& c)
  {
    my_capabilities = (object_capabilities::capabilities)(unsigned(c.my_capabilities) & unsigned(my_capabilities));  
  }

  bool operator &(const object_capabilities& c1, object_capabilities::capabilities c2)
  {
    return unsigned(c1.my_capabilities) & unsigned(c2);
  }
  bool operator &(object_capabilities::capabilities c1, const object_capabilities& c2)
  {
    return unsigned(c1) & unsigned(c2.my_capabilities);
  }
  object_capabilities operator &(const object_capabilities& c1, const object_capabilities& c2)
  {
    return object_capabilities(object_capabilities::capabilities(unsigned(c1.my_capabilities) & unsigned(c2.my_capabilities)));
  }
  object_capabilities operator |(const object_capabilities& c1, const object_capabilities& c2)
  {
    return object_capabilities(object_capabilities::capabilities(unsigned(c1.my_capabilities) | unsigned(c2.my_capabilities)));
  }  

  object_capabilities operator ~(const object_capabilities::capabilities& c)
  {
    return object_capabilities(object_capabilities::capabilities(~unsigned(c)));
  }

  object_capabilities operator ~(const object_capabilities& c)
  {
    return object_capabilities(object_capabilities::capabilities(~unsigned(c.my_capabilities)));
  }

  bool operator==(const object_capabilities& i1,
                  const object_capabilities& i2)
  {
    return unsigned(i1.my_capabilities) == unsigned(i2.my_capabilities);
  }
  bool operator!=(const object_capabilities& i1,
                  const object_capabilities& i2)
  {
    return unsigned(i1.my_capabilities) != unsigned(i2.my_capabilities);
  }
  
} // namespace amethyst
