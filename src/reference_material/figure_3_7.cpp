// g++ -g figure_3_7.cpp -o figure_3_7 -lm -I. -Igraphics -Igeneral -DDO_NOT_ICE

/*
 * This is figure 3.7 from Peter Shirley's "Realistic Raytracing" book, or at
 * least an early draft of it.
 */

#include <sphere.hpp>
#include <triangle.hpp>
#include <aggregate.hpp>
#include <raster.hpp>
#include <tga_io.h>
#include <rgbcolor.hpp>

#include <iostream>

using namespace amethyst;

typedef point3<double> point;
typedef vector3<double> vec;
typedef rgbcolor<double> color;

using std::cout;
using std::endl;

#define WIDTH 500
#define HEIGHT 500

int main(int argc, char** argv)
{
  aggregate<double> sl;


  rc_pointer<shape<double> > sph_ptr(new sphere<double>(point(0,0,1-WIDTH-50), WIDTH-0.9));
  sl.add(sph_ptr);

#if !defined(DO_NOT_ICE)
  rc_pointer<shape<double> > tri_ptr(new triangle<double>(point(0,0,-WIDTH),
							  point(WIDTH+0.1,0,-WIDTH),
							  point(WIDTH/2.0,WIDTH+0.1,-WIDTH)));

  sl.add(tri_ptr);
#endif

  color light(0.7,0.7,0.7), dark(0.5,0.5,0.5), black(0,0,0);

  raster<color> image(WIDTH, HEIGHT);

  intersection_info<double> stuff;

  cout << "objects=" << sl << endl;
  
  for(int y = 0; y < HEIGHT; ++y)
  {
    for(int x = 0; x < WIDTH; ++x)
    {
      unit_line3<double> r(point3<double>(x,HEIGHT-y,0),
			   vector3<double>(0,0,-1));

      //      cout << "Line=" << r << endl;
      
      if(!sl.intersects_line(r, stuff))
      {
	image(x,y) = black;
      }
      else
      {
	//	cout << "Hit at (" << x << "," << y << ")" << endl;
	const shape<double>* hit_shape = stuff.get_shape();

	if( dynamic_cast<const sphere<double>*>(hit_shape) != NULL )
	{
	  image(x,y) = light;
	}
	else if( dynamic_cast<const triangle<double>*>(hit_shape) != NULL )
	{
	  image(x,y) = dark;
	}
      }
    }
  }
  tga_io<color> output;

  output.output("figure_3_7.tga", image);
  return 0; 
}
