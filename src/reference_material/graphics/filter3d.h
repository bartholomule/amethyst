#if    !defined(KH_FILTER3D_H)
#define         KH_FILTER3D_H

/*
  filter3d.h

  A set of filters that will take a set of points, and warp them to fit in the
  density pattern of the given filter (so that the image creation function can
  be sampled and averaged at these points to generate a filtered pixel value).
  They may have other applications later. 
 */

/*
  Revision history:
  20jan1999 Created this file.
  08Apr2000 Changed the include of coord2 to be in <graph_math>
  25May2001 Changed this class to be templated, moving the members back in here
            from the filter3d.cpp file.
	    Fixed the separated triangle filter to use the width, which was
	    previously obeing ignored. 
 */
#include <vector>
#include <graph_math/coord2.h>

using std::vector;

template <class T>
class filter_3d
{
public:
  inline filter_3d() {}
  virtual ~filter_3d() {}

  virtual void warp_points(vector<coord2<T> >& points_to_warp) const
  {
    /* The base filter does no warping. */
  }
};

template <class T>
class box_filter_3d: public filter_3d<T>
{
public:
  inline box_filter_3d(T width = 2):
    filter_3d<T>(),
    box_width(width)
  {
  }
  virtual ~box_filter_3d() {}

  virtual void warp_points(vector<coord2<T> >& points_to_warp) const;
  
private:
  T box_width;
};

template <class T>
class separated_triangle_filter_3d: public filter_3d<T>
{
public:
  
  inline separated_triangle_filter_3d(T width = 2):
    filter_3d<T>(),
    triangle_width(width) {}
  virtual ~separated_triangle_filter_3d() {}
  
  virtual void warp_points(vector<coord2<T> >& points_to_warp) const;
  
private:
  T triangle_width;
};


template <class T>
void box_filter_3d<T>::warp_points(vector<coord2<T> >& points_to_warp) const
{
  T half_width = box_width/2;
  typename std::vector<coord2<T> >::iterator vi;

  for(vi = points_to_warp.begin(); vi != points_to_warp.end(); ++vi)
  {
    vi->x() = (vi->x()*box_width) - half_width;
    vi->y() = (vi->y()*box_width) - half_width;
  }
}


template <class T>
void separated_triangle_filter_3d<T>::warp_points(std::vector<coord2<T> >& points_to_warp) const
{
  typename std::vector<coord2<T> >::iterator vi;
  T half_width = triangle_width / T(2);

  for(vi = points_to_warp.begin(); vi != points_to_warp.end(); ++vi)
  {
    if(vi->x() < 0.5)
      vi->x() = (sqrt(2*vi->x()) - 1) * half_width;
    else
      vi->x() = (1-sqrt(2-2*vi->x())) * half_width;
    
    if(vi->y() < 0.5)
      vi->y() = (sqrt(2*vi->y()) - 1) * half_width;
    else
      vi->y() = (1-sqrt(2-2*vi->y())) * half_width;
  }
}


#endif /* !defined(KH_FILTER3D_H) */
