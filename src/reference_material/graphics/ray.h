#if    !defined(KH_RAY_H)
#define         KH_RAY_H

/*
  ray.h

  A ray class for use in raytracing and other applications.
 */

/*
  revision history:
  21jan1999 Created this file.
  17Apr1999 Added the time stuff.
  08Apr2000 Changed the includes to the point2, point3, and vector3 to be in
            <graph_math/XXX>, as I am reorganizing.
  18May2001 Started changing this for use in a newer tracer, as per "Realistic
            Raytracing" by Peter Shirley, "templatized" the class...
  06Jul2003 Changed the reflect and refract methods in ray to lower the max
            depth instead of increasing the current depth.  This makes itt
            possible to use for path tracing with reflections of lights.
	    Also added a light sampling method to the class...
 */


#include <graph_math/coord3.h>
#include <graph_math/interval.h>
#include "../my_defines.h"

template <class T>
class ray_3d;

#include <shapes/hitrec.h>
#if       !defined(MIN_CONTRIBUTION)
#define            MIN_CONTRIBUTION (1e-6)
#endif /* !defined(MIN_CONTRIBUTION) */

template <class T>
class ray_3d
{
public:
  enum RayLightingMethod { LIGHTING_DIRECT, LIGHTING_RADIANCE };


  inline ray_3d(T min_t = (KH_EPSILON),
		T max_t = KH_MAX_NUMBER,
		T _ior = 1,
		T _time = 0,
		T _contribution = 1,
		long max_depth = KH_DEFAULT_MAX_RECURSE,
		long current_depth = 0,
		RayLightingMethod rlm = LIGHTING_DIRECT):
    range_of_ray(min_t, max_t),
    time_ray_was_fired(_time),
    current_ior(_ior),
    depth_max(max_depth),
    depth(current_depth),
    light_sampling_method(rlm),
    effective_contribution(_contribution)
  {
  }

  // A function to define a full ray, so that any changes will be noticed
  // without default parameters slipping through and taking a bite out of me
  // (as happened when I added the contribution parameter. 
  static ray_3d<T> full_ray(const coord3<T>& _origin,
			    const coord3<T>& _direction,
			    T min_t,
			    T max_t,
			    T _ior,
			    T _time,
			    T _contribution,
			    long max_depth,
			    long current_depth,
			    RayLightingMethod rlm)
  {
    return ray_3d<T>(_origin,_direction,
		     min_t,max_t,
		     _ior,_time,_contribution,
		     max_depth,current_depth,
		     rlm);
  }

  inline ray_3d(const coord3<T>& _origin, const coord3<T>& _direction,
		T min_t = (KH_EPSILON),
		T max_t = KH_MAX_NUMBER,
		T _ior = 1,
		T _time = 0,
		T _contribution = 1,
		long max_depth = KH_DEFAULT_MAX_RECURSE,
		long current_depth = 0,
		RayLightingMethod rlm = LIGHTING_DIRECT):
    origin_of_ray(_origin),
    direction_of_ray(_direction),
    range_of_ray(min_t, max_t),
    time_ray_was_fired(_time),
    current_ior(_ior),
    depth_max(max_depth),
    depth(current_depth),
    light_sampling_method(rlm),
    effective_contribution(_contribution)
  {
  }

  friend std::ostream& operator << <>(std::ostream& o, const ray_3d<T>& r);

  inline coord3<T>& direction() { return(direction_of_ray); }
  inline const coord3<T>& direction() const { return(direction_of_ray); }
  
  inline coord3<T>& origin() { return(origin_of_ray); }
  inline const coord3<T>& origin() const { return(origin_of_ray); }

  inline T time() const { return(time_ray_was_fired); }
  inline void set_time(T time) { time_ray_was_fired = time; use_time = true; }

  inline bool time_used() const { return use_time; }
  inline void set_time_used(bool b) { use_time = b; }

  inline T ior() const { return(current_ior); }
  inline void set_ior(T _ior) { current_ior = _ior; }  

  inline const interval<T>& range() const { return(range_of_ray); }

  inline void set_range(T min, T max)
  {
    range_of_ray.set(min, max);
  }

  inline T get_contribution() const
  {
    return(effective_contribution);
  }
  inline void set_contribution(T contribution)
  {
    effective_contribution = contribution;
  }

  inline void set_max_depth(long depth) { depth_max = depth; }
  inline long max_depth() const { return(depth_max); }
  inline long current_depth() const { return(depth); }
  inline bool depth_ok() const { return(depth < depth_max); }
  inline bool primary_ray() const { return (depth==0); }

  template <class color_type>
  ray_3d<T> reflect(const hitrec<T,color_type>& hr) const;
  template <class color_type>
  ray_3d<T> refract(const hitrec<T,color_type>& hr, T ior, bool& ray_ok) const;  

  RayLightingMethod get_lighting_method() const { return light_sampling_method; }
  void set_lighting_method(RayLightingMethod rlm) { light_sampling_method = rlm; }  
  
private:  
  coord3<T> origin_of_ray;
  coord3<T> direction_of_ray;
  interval<T> range_of_ray;
  T time_ray_was_fired;
  T current_ior;
  long depth_max;
  long depth;
  bool use_time;
  RayLightingMethod light_sampling_method;
  T effective_contribution;
};

template <class T>
std::ostream& operator << (std::ostream& o, const ray_3d<T>& r)
{
  o << "{"
    << "O=" << r.origin_of_ray << ";"
    << "V=" << r.direction_of_ray
    << "->" << r.range_of_ray << "<-"
    << "; IOR=" << r.current_ior
    << "; d" << r.depth << "(" << r.depth_max << ")"
    << "}";
  return(o);
}

template <class T>
template <class color_type>
ray_3d<T> ray_3d<T>::reflect(const hitrec<T,color_type>& hr) const
{
  T cos = dotprod(direction_of_ray, hr.get_normal());
  // If the ray is going the same direction as the normal for the object which
  // was just hit, flip the cosine, so that the reflected ray will not travel
  // through the object, rather, it should be reflected backwards. 
  if( cos > 0 )
  {
    cos = -cos;
  }
  coord3<T> new_direction = unit(direction_of_ray - 2 * cos * hr.get_normal());

  return ray_3d<T>(hr.get_point(), new_direction,
		   //		   range_of_ray.begin(), range_of_ray.end(),
		   KH_EPSILON, KH_MAX_NUMBER,
		   current_ior, time_ray_was_fired,
		   effective_contribution,
		   (depth_max > depth)?(depth_max - (depth+1)):0,
		   0,
		   light_sampling_method);
}

template <class T>
template <class color_type>
ray_3d<T> ray_3d<T>::refract(const hitrec<T,color_type>& hr, T ior, bool& ray_ok) const
{
  T cos = dotprod(direction(), hr.get_normal());
  T ratio = current_ior / ior;
  T rad = 1 - (ratio * ratio) * (1 - cos * cos);

  GOM.debug(3) << "ray::refract(...): cos=" << cos << " ratio=" << ratio << " radical=" << rad << std::endl;
  
  if(rad >= 0)
  {
    coord3<T> new_direction = unit(ratio * (direction() - cos * hr.get_normal()) -
				   (hr.get_normal() * sqrt(rad)));

    ray_ok = true;
    
    return ray_3d<T>(hr.get_point(), new_direction,
		     //		     range_of_ray.begin(), range_of_ray.end(),
		     KH_EPSILON, KH_MAX_NUMBER,		     
		     ior, time_ray_was_fired,
		     effective_contribution,		     
		     (depth_max > depth)?(depth_max - (depth+1)):0,
		     0,		     
		     light_sampling_method);
  }
  else
  {
    GOM.debug(2) << "refract is NOT ok" << std::endl;
    ray_ok = false;
    /* Create a ray which should prevent its own use by use of strictly
       limiting initial values.  This is just in case the bonehead programmer
       (me in this case) forgets to use the returned boolean value (ray_ok).
    */ 
    return ray_3d<T>(hr.get_point(), this->direction(),
		     KH_EPSILON,KH_EPSILON, /* range which prevents it from going anywhere... */
		     ior, time_ray_was_fired,
		     0, /* Effective contribution of 0... */
		     0, /* Max depth of 0 */
		     0,
		     light_sampling_method);
  }
}

#endif /* !defined(KH_RAY_H) */
