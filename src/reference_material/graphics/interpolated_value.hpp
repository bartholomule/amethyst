/*
 * $Id: interpolated_value.hpp,v 1.1 2004/06/01 03:53:35 kpharris Exp $
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

#if       !defined(AMETHYST__INTERPOLATED_VALUE_HPP)
#define            AMETHYST__INTERPOLATED_VALUE_HPP

/*
 * This file provides classes/functions for interpolating values using various
 * methods.  For now, this includes interpolation among:
 * # Two values 
 * # A list of values
 * The values and the parameters at which the values are reached can be
 * speficied, as well as endpoint behavior (stop, continue), interpolation
 * methods (block, linear, cubic).
 *
 * In the future, I hope to be able to have a full spline, where smooth curve
 * fitting is performed in each section (as close to the desired values as
 * possible), and the curve is continuous from one section to another (no sharp
 * changes).
 *
 * For simplicity of use, everything shoule be used through a pointer or
 * reference to the base class (to minimize the number of template parameters
 * required), and the interpolation created from a (cloned, if using a pointer)
 * version of something returned from create_interpolation().
 *
 * For example:
 * typedef vector3<double> val_type;
 * // This creates an interpolation object which varies across [0,1] from
 * // VECTOR1 to VECTOR2.  Endpoint behavior and interpolation method are set
 * // at their defaults (ENDPOINT_STOP, and INERP_CUBIC);
 * interpolated_value<double, val_type>* val = create_interpolation<double,val_type>(VECTOR1,VECTOR2).clone_new()
 *
 * SOME_VECTOR = val->interpolate(SOME_VALUE);
 * ...
 * delete val;
 *
 *
 * To minimize the number of required specializations of interpolation types, a
 * single function is used (basic_interpolation).  To properly interpolate
 * types such as matrices, this must be specialized otherwise strange
 * rotations, scalings, or skews may happen in intermediate matrices.
 */


#include <string>
#include "quick_vector.hpp"
#include <algorithm>
#include <ostream>
#include <general/string_format.hpp>

namespace amethyst
{
  /**
   * This simple struct is a point along an interpolation curve.
   */
  template<class parametric_type, class interpolation_type>  
  struct interpolation_point
  {
    parametric_type parameter;
    interpolation_type value;

    interpolation_point(): parameter(), value() { }
    interpolation_point(const parametric_type& p, const interpolation_type& v):
      parameter(p),
      value(v)
    {
    }
  };

  /**
   * 
   * This is a class for interpolated values.  The main use of this class will
   * be for parametric-changing parameters (eg. location, radius, etc.) 
   * 
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.1 $
   * 
   */
  template<class parametric_type, class interpolation_type>
  class interpolated_value
  {
  private:

  protected:

  public:

    /** Default constructor */
    interpolated_value() { }

    /** Destructor */
    virtual ~interpolated_value() { }

    virtual bool valid_parameter(parametric_type parameter) const = 0;
    virtual interpolation_type interpolate(parametric_type parameter) const = 0;
    virtual interpolated_value<parametric_type, interpolation_type>* clone_new() const = 0;

    virtual std::string to_string(const std::string& base_indentation,
                                  const std::string& level_indentation = "  ") const = 0;

    virtual quick_vector<interpolation_point<parametric_type, interpolation_type> > get_interpolation_points() const = 0;
    
    virtual std::string name() const
    {
      return "interpolation_base_class";
    }

    std::string to_string() const
    {
      return to_string("");
    }    
    
  }; // class interpolated_value


  // Comparison operators (all defined in terms of < or ==, to reduce the
  // operations needed).
  template<class parametric_type, class interpolation_type>
  inline bool operator ==(const interpolation_point<parametric_type, interpolation_type>& p1,
                          const interpolation_point<parametric_type, interpolation_type>& p2)
  {
    return p1.parameter == p2.parameter;
  }
  template<class parametric_type, class interpolation_type>
  inline bool operator !=(const interpolation_point<parametric_type, interpolation_type>& p1,
                          const interpolation_point<parametric_type, interpolation_type>& p2)
  {
    return !(p1 == p2);
  }  

  template<class parametric_type, class interpolation_type>
  inline bool operator <(const interpolation_point<parametric_type, interpolation_type>& p1,
                         const interpolation_point<parametric_type, interpolation_type>& p2)
  {
    return p1.parameter < p2.parameter;
  }

  template<class parametric_type, class interpolation_type>
  inline bool operator >(const interpolation_point<parametric_type, interpolation_type>& p1,
                         const interpolation_point<parametric_type, interpolation_type>& p2)
  {
    return !(p1 < p2) && !(p1 == p2);
  }  
  
  template<class parametric_type, class interpolation_type>
  inline bool operator<=(const interpolation_point<parametric_type, interpolation_type>& p1,
                         const interpolation_point<parametric_type, interpolation_type>& p2)
  {
    return (p1 < p2) || (p1 == p2);
  }  

  template<class parametric_type, class interpolation_type>
  inline bool operator>=(const interpolation_point<parametric_type, interpolation_type>& p1,
                         const interpolation_point<parametric_type, interpolation_type>& p2)
  {
    return !(p1 < p2);
  }

  template<class parametric_type, class interpolation_type>  
  std::ostream& operator<<(std::ostream& o,
                           const interpolation_point<parametric_type, interpolation_type>& ip)
  {
    o << "{ " << ip.parameter << ", " << ip.value << " }";
    return o;
  }

  enum endpoint_behavior { ENDPOINT_STOP, ENDPOINT_CONTINUE };

  std::ostream& operator<<(std::ostream& o, endpoint_behavior behavior )
  {
    switch( behavior )
    {
    case ENDPOINT_STOP:
      o << "STOP";
      break;
    case ENDPOINT_CONTINUE:
      o << "CONTINUE";
      break;
    }
    return o;
  }


  // This function exists so that only a single function needs to be
  // specialized in order to make complex types (eg. a matrix), which require
  // more complicated interpolation methods, usable in all of these
  // interpolation functions.
  template <class interpolation_type, class parametric_type>
  inline interpolation_type basic_interpolation(parametric_type adjusted_value,
						const interpolation_type& v1,
						const interpolation_type& v2)
  {
    return ( v1 * (1 - adjusted_value) +
	     v2 * adjusted_value );
  }
						
  
  template <class parametric_type, class interpolation_type>
  inline interpolation_type block_interpolate(parametric_type val,
                                              const interpolation_type& v1,
                                              const interpolation_type& v2,
                                              endpoint_behavior behavior)
  {
    return v1;
  }
  
  template <class parametric_type, class interpolation_type>
  inline interpolation_type linear_interpolate(parametric_type val,
                                               const interpolation_type& v1,
                                               const interpolation_type& v2,
                                               endpoint_behavior behavior)
  {
    if( (val < 0) && (behavior == ENDPOINT_STOP) )
    {
      return v1;
    }
    if( (val > 1) && (behavior == ENDPOINT_STOP) )
    {
      return v2;
    }

    return basic_interpolation(val, v1, v2);
  }


  namespace helper_functions
  {
    // This function was adapted from Peter Shirley's "Realist Raytracing" second
    // edition.  I have no clue why the given constants were chosen.
    template <class T>
    inline T interpolated_cubic_filter_solve(T t)
    {
      T u = t;
      for( int i = 0; i < 5; ++i )
      {
	u = ( ( T(11) * t +
		u * u * (u * (T(8) - u * T(9)) + T(6)) ) /
	      ( T(4) + u * T(12) * (u * (T(1) - u) + T(1)) ) );
      }
      return u;
    }

    // This function was adapted from Peter Shirley's "Realist Raytracing" second
    // edition.  I have no clue why the given constants were chosen.  
    template <class T>
    inline T interpolated_cubic_filter(T t)
    {
      const T t1 = T(1.0 / 24.0);
      const T t2 = T(1) - t1;
      if( t < t1 )
      {
	return T(pow(24.0 * t, 0.25) - 2.0);
      }
      else if( t < T(0.5) )
      {
	return interpolated_cubic_filter_solve(T(24) * (t - t1) / T(11)) - T(1);
      }
      else if( t < t2 )
      {
	return T(1) - interpolated_cubic_filter_solve(T(24) * (t2 - t) / T(11));
      }
      else
      {
	return T(2.0 - pow(24.0 * ( 1.0 - t ), 0.25));
      }
    }
  } // end namespace helper_functions
  
  template <class parametric_type, class interpolation_type>
  inline interpolation_type cubic_interpolate(parametric_type val,
                                              const interpolation_type& v1,
                                              const interpolation_type& v2,
                                              endpoint_behavior behavior)
  {
    if( (val < 0) && (behavior == ENDPOINT_STOP) )
    {
      return v1;
    }
    if( (val > 1) && (behavior == ENDPOINT_STOP) )
    {
      return v2;
    }

    // This is how I've done cubic filtering before:
    //    parametric_type val2 = ( 3 * val * val - 2 * val * val * val );
    //
    // This is how Peter Shirley's second edition says to do a cubic filter:
    parametric_type val2 = helper_functions::interpolated_cubic_filter<parametric_type>(val);

    // NOTE: I haven't done much testing on the Pete's new version.

    return basic_interpolation(val2, v1, v2);    
  }  
  

  template <class parametric_type, class interpolation_type>
  struct interpolation_base
  {
    std::string name() const { return "interpolation_base"; }

    interpolation_type operator()(const parametric_type& param,
                                  const parametric_type& pmin,
                                  const parametric_type& pmax,
                                  const interpolation_type& vmin,
                                  const interpolation_type& vmax,
				  endpoint_behavior b) const
    {
      return block_interpolate(param, vmin, vmax, b);
    }
  };

  template <class parametric_type, class interpolation_type>
  struct linear_interpolation : public interpolation_base<parametric_type, interpolation_type>
  {
    std::string name() const { return "linear_interpolation"; }

    interpolation_type operator()(const parametric_type& param,
                                  const parametric_type& pmin,
                                  const parametric_type& pmax,
                                  const interpolation_type& vmin,
                                  const interpolation_type& vmax,
				  endpoint_behavior b) const
    {
      parametric_type t = (param - pmin) / (pmax - pmin);
      return linear_interpolate(t, vmin, vmax, b);
    }
  };

  template <class parametric_type, class interpolation_type>
  struct cubic_interpolation : public interpolation_base<parametric_type, interpolation_type>
  {
    std::string name() const { return "cubic_interpolation"; }

    interpolation_type operator()(const parametric_type& param,
                                  const parametric_type& pmin,
                                  const parametric_type& pmax,
                                  const interpolation_type& vmin,
                                  const interpolation_type& vmax,
				  endpoint_behavior b) const
    {
      parametric_type t = (param - pmin) / (pmax - pmin);
      return cubic_interpolate(t, vmin, vmax, b);
    }    
  };

  template <class parametric_type, class interpolation_type, class interpolation_function>
  struct user_interpolation : public interpolation_base<parametric_type, interpolation_type>
  {
    user_interpolation(interpolation_function f, std::string fn_name) : fun(f), user_function_name(fn_name) { }

    std::string name() const { return "user_interpolation_function<" + user_function_name + ">"; }

    interpolation_type operator()(const parametric_type& param,
                                  const parametric_type& pmin,
                                  const parametric_type& pmax,
                                  const interpolation_type& vmin,
                                  const interpolation_type& vmax,
				  endpoint_behavior b) const
    {
      parametric_type t = (param - pmin) / (pmax - pmin);
      return fun(t, vmin, vmax, b);
    }

  private:
    user_interpolation() { }
    interpolation_function fun;
    std::string user_function_name;
  };  

  
  template<class parametric_type, class interpolation_type, class interpolation_function>
  class interpolated_pair : public interpolated_value<parametric_type, interpolation_type>
  {
  public:
    typedef interpolated_pair<parametric_type, interpolation_type, interpolation_function> self_type;
    typedef amethyst::interpolation_point<parametric_type, interpolation_type> interpolation_point;
  private:
    interpolation_point ip1, ip2;
    interpolation_function fun;
    endpoint_behavior behavior;
  public:
    interpolated_pair(const interpolation_type& v1,
		      const interpolation_type& v2,
                      interpolation_function f = interpolation_function(),
		      endpoint_behavior b = ENDPOINT_STOP):
      ip1(0,v1),
      ip2(1,v2),
      fun(f),
      behavior(b)
    {
    }

    interpolated_pair(const interpolation_point& p1, const interpolation_point& p2,
                      interpolation_function f = interpolation_function(),
		      endpoint_behavior b = ENDPOINT_STOP):
      ip1(p1),
      ip1(p2),
      fun(f),
      behavior(b)
    {
    }
    // NOTE: copy constructor/assign operator use compiler default.
    
    virtual ~interpolated_pair() { };
    virtual self_type* clone_new() const { return new self_type(*this); }

    virtual std::string name() const
    {
      return "interpolated_pair<" + fun.name() + ">";
    }
    
    virtual std::string to_string(const std::string& base_indentation,
                                  const std::string& level_indentation = "  ") const
    {
      std::string my_name = name();
      std::string indent = base_indentation + level_indentation;
      return ( base_indentation + my_name + "\n" +
	       base_indentation + "{\n" +
	       indent + string_format("p1=%1\n", ip1) +
	       indent + string_format("p2=%1\n", ip2) +
	       indent + string_format("endpoint=%1\n", behavior) +
	       base_indentation + "}" );
    }

    virtual bool valid_parameter(parametric_type parameter) const
    {
      return (parameter >= ip1.parameter) && (parameter <= ip2.parameter);
    }
    
    virtual interpolation_type interpolate(parametric_type parameter) const
    {
      return fun((parameter - ip1.parameter) / (ip2.parameter - ip1.parameter),
		 ip1.parameter, ip2.parameter,
		 ip1.value, ip2.value, behavior);
    }
    
    virtual quick_vector<interpolation_point> get_interpolation_points() const
    {
      quick_vector<interpolation_point> retval;
      retval.push_back(ip1);
      retval.push_back(ip2);
      return retval;
    }
  };



  template<class parametric_type, class interpolation_type, class interpolation_function>
  class interpolated_vector : public interpolated_value<parametric_type, interpolation_type>
  {
  public:
    typedef interpolated_vector<parametric_type, interpolation_type, interpolation_function> self_type;
    typedef amethyst::interpolation_point<parametric_type, interpolation_type> interpolation_point;
  private:
    quick_vector<interpolation_point> iv;
    interpolation_function fun;
    endpoint_behavior behavior;
  public:

    interpolated_vector(const interpolation_type& v1, const interpolation_type& v2,
                        interpolation_function f = interpolation_function(),
                        endpoint_behavior b = ENDPOINT_STOP):
      iv(2),
      fun(f),
      behavior(b)
    {
      iv[0] = interpolation_point(parametric_type(0), v1);
      iv[1] = interpolation_point(parametric_type(1), v2);				  
    }

    interpolated_vector(const interpolation_point& p1, const interpolation_point& p2,
                        interpolation_function f = interpolation_function(),
                        endpoint_behavior b = ENDPOINT_STOP):
      iv(2),
      fun(f),
      behavior(b)
    {
      iv[0] = p1;
      iv[1] = p2;
      // Make sure the points are in order.
      std::sort(iv.begin(), iv.end());      
    }

    interpolated_vector(const quick_vector<interpolation_point>& v,
                        interpolation_function f = interpolation_function(),
                        endpoint_behavior b = ENDPOINT_STOP):
      iv(v),
      fun(f),
      behavior(b)
    {
      // Make sure the points are in order.
      std::sort(iv.begin(), iv.end());
    }    
    
    // NOTE: copy constructor/assign operator use compiler default.
    
    virtual ~interpolated_vector() { };
    virtual self_type* clone_new() const { return new self_type(*this); }

    virtual std::string name() const
    {
      return "interpolated_vector<" + fun.name() + ">";
    }
    
    virtual std::string to_string(const std::string& base_indentation,
                                  const std::string& level_indentation = "  ") const
    {
      std::string my_name = name();
      std::string indent = base_indentation + level_indentation;
      return ( base_indentation + my_name + "\n" +
	       base_indentation + "{\n" +
	       indent + string_format("v=%1\n", iv) +
	       indent + string_format("endpoint=%1\n", behavior) +
	       base_indentation + "}" );
    }

    virtual bool valid_parameter(parametric_type parameter) const
    {
      if( iv.empty() )
      {
        return false;
      }
      parametric_type start = iv.begin()->parameter;
      parametric_type finish = iv.rbegin()->parameter;      
      return (parameter >= start) && (parameter <= finish);
    }
    
    virtual interpolation_type interpolate(parametric_type parameter) const
    {
      using std::cout;
      using std::endl;
      if( iv.size() >= 2 )
      {
	size_t i = 0;

	for( ;
	     (i < iv.size()) && (iv[i].parameter < parameter);
	     ++i )
	{
	}

	
	size_t before = 0, after = 0;
	if( (i < iv.size()) && (i >  0) )
	{
	  before = i - 1;
	  after = i;
	}
	else if( i == 0 )
	{
	  before = i;
	  after = i + 1;
	}
	else // if( i >= iv.size() )
	{
	  before = iv.size() - 2;
	  after = before + 1;
	}

	//	cout << "param=" << parameter << " i=" << i << " before=" << before << " after=" << after << endl;

	parametric_type t = parameter; //(parameter - iv[before].parameter) / (iv[after].parameter - iv[before].parameter);
	
	//	cout << "iv[before]=" << iv[before] << "  iv[after]=" << iv[after] << " t=" << t << endl;

	//	return fun((parameter - iv[before].parameter) /
	//	(iv[after].parameter - iv[before].parameter),
	return fun(t,
		   iv[before].parameter, iv[after].parameter,
		   iv[before].value, iv[after].value, behavior);
      }
      else if( !iv.empty() )
      {
	return iv.begin()->value;
      }
      else
      {
	return interpolation_type(0);
      }
    }
    
    virtual quick_vector<interpolation_point> get_interpolation_points() const
    {
      return iv;
    }
  }; // interpolated_vector

  enum interpolation_method { INTERP_BLOCK, INTERP_LINEAR, INTERP_CUBIC };


  // This struct is only here to allow a proper typedef for an interpolation
  // function prototype.
  template <class T, class IT>
  struct it_wrapper
  {
    typedef IT (*user_interpolation_function)(T,const IT&,const IT&,endpoint_behavior);
  };
  
  template <class T, class IT>
  interpolated_pair<T,IT,user_interpolation<T, IT, typename it_wrapper<T,IT>::user_interpolation_function> >
  create_interpolation(const IT& i1,
		       const IT& i2,
		       endpoint_behavior b = ENDPOINT_STOP,
		       interpolation_method method = INTERP_CUBIC) 
  {
    typedef typename it_wrapper<T,IT>::user_interpolation_function fn_ptr;

    fn_ptr fun;
    std::string fn_name;

    switch(method)
    {
    case INTERP_BLOCK:
      fun = &block_interpolate<T,IT>;
      fn_name = "block";
      break;
    case INTERP_LINEAR:
      fun = &linear_interpolate<T,IT>;
      fn_name = "linear";
      break;
    case INTERP_CUBIC:
      fun = &cubic_interpolate<T,IT>;
      fn_name = "cubic";
      break;
    }

    user_interpolation<T,IT,fn_ptr> interpolation(fun, fn_name);
    
    return interpolated_pair<T,IT,user_interpolation<T,IT,fn_ptr> >(i1,i2, interpolation, b);
  }


  template <class T, class IT>
  interpolated_vector<T,IT,user_interpolation<T, IT, typename it_wrapper<T,IT>::user_interpolation_function> >
  create_interpolation(const quick_vector<interpolation_point<T, IT> >& vec,
		       endpoint_behavior b = ENDPOINT_STOP,
		       interpolation_method method = INTERP_CUBIC) 
  {
    typedef typename it_wrapper<T,IT>::user_interpolation_function fn_ptr;

    fn_ptr fun;
    std::string fn_name;

    switch(method)
    {
    case INTERP_BLOCK:
      fun = &block_interpolate<T,IT>;
      fn_name = "block";
      break;
    case INTERP_LINEAR:
      fun = &linear_interpolate<T,IT>;
      fn_name = "linear";
      break;
    case INTERP_CUBIC:
      fun = &cubic_interpolate<T,IT>;
      fn_name = "cubic";
      break;
    }

    user_interpolation<T,IT,fn_ptr> interpolation(fun, fn_name);
    
    return interpolated_vector<T,IT,user_interpolation<T,IT,fn_ptr> >(vec, interpolation, b);
  }
  
  
  template <class T, class V>
  std::ostream& operator<<(std::ostream& o, const interpolated_value<T,V>& iv)
  {
    o << iv.to_string();
    return o;
  }
  
} // namespace amethyst


#endif /* !defined(AMETHYST__INTERPOLATED_VALUE_HPP) */

