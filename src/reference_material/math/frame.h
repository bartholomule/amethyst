#if       !defined(KH_FRAME_H)
#define            KH_FRAME_H

/*
  frame.h -- A class to represent a complete coordinate system.
 */

/*
  Revision History:
  18May2001 Created this file.
 */

#include "onb.h"
#include "mat_4x4.h"

template <class T>
class frame
{
public:
  inline frame(const coord3<T>& origin, const coord3<T>& w):
    my_basis(w),
    my_origin(origin)
  {
    make_matrices();
  }
  inline frame(const coord3<T>& origin,
	       const coord3<T>& a, const coord3<T>& b):
    my_basis(a,b),
    my_origin(origin)
  {
    make_matrices();    
  }

  inline frame(const frame<T>& f):
    my_basis(f.my_basis),
    my_origin(f.my_origin),
    my_transform(f.my_transform)
  {
  }
  inline frame<T>& operator=(const frame<T>& f)
  {
    if(&f != this)
    {
      my_basis = f.my_basis;
      my_origin = f.my_origin;
      my_transform = f.my_transform;
    }
    return(*this);
  }

  inline coord3<T> u() const { return my_basis.get_u(); }
  inline coord3<T> v() const { return my_basis.get_v(); }
  inline coord3<T> w() const { return my_basis.get_w(); }

  onb<T>&    basis()  { return my_basis; }
  coord3<T>& origin() { return my_origin; }  
  const onb<T>&    basis()  const { return my_basis; }
  const coord3<T>& origin() const { return my_origin; }  

  inline const matrix_4x4<T>& transform() const
  {
    return my_transform;
  }
  inline const matrix_4x4<T>& inverse_transform() const
  {
    return my_inverse_transform;
  }  

  inline coord3<T> transform_as_point(const coord3<T>& c)
  {
    return my_transform.transform_as_point(c);
  }
  inline coord3<T> transform_as_vector(const coord3<T>& c)
  {
    return my_transform.transform_as_vector(c);    
  }
  inline coord3<T> transform_as_normal(const coord3<T>& c)
  {
    return my_transform.transform_as_normal(c);        
  }

  void make_matrices()
  {
    matrix_4x4<T> rotation = matrix_4x4<T>::make_rotate(u(), v(), w());
    matrix_4x4<T> translation = matrix_4x4<T>::make_translate(origin());
    
    my_transform = rotation * translation;		    
  }
  
private:
  onb<T>    my_basis;
  coord3<T> my_origin;
  
  matrix_4x4<T> my_transform;
};


#endif /* !defined(KH_FRAME_H) */
