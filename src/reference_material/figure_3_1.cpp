// g++ -g figure_3_1.cpp -o figure_3_1 -lm -I. -Igraphics -Igeneral

/*
 * This is figure 3.1 from Peter Shirley's "Realistic Raytracing" book, second
 * edition. 
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
  (void) argc;
  (void) argv;
  aggregate<double> sl;

  rc_pointer<shape<double> > sph_ptr(new sphere<double>(point(250,250,-1000),
							150));
  sl.add(sph_ptr);
  point p1(300,600,-800);
  point p2(0,100,-1000);
  point p3(450,20,-1000);
  rc_pointer<shape<double> > tri_ptr(new triangle<double>(p1,p2,p3));

  sl.add(tri_ptr);

  color light(0.7,0.7,0.7), dark(0.5,0.5,0.5), black(0,0,0);
  color red(0.7,0.0,0.0), blue(0.0,0.0,0.7);

  raster<color> image(WIDTH, HEIGHT);

  intersection_info<double> stuff;

  cout << "objects=" << sl << endl;

  for( int y = 0; y < HEIGHT; ++y )
  {
    for( int x = 0; x < WIDTH; ++x )
    {
      unit_line3<double> r(point3<double>(500.0 * x / double(WIDTH),
					  500.0 * (HEIGHT-y)/double(HEIGHT),
					  0),
                           vector3<double>(0,0,-1));

      if( !sl.intersects_line(r, stuff) )
      {
        image(x,y) = dark;
      }
      else
      {
        const shape<double>* hit_shape = stuff.get_shape();

        if( dynamic_cast<const sphere<double>*>(hit_shape) != NULL )
        {
          image(x,y) = blue;
        }
        else if( dynamic_cast<const triangle<double>*>(hit_shape) != NULL )
        {
          image(x,y) = red;
        }
      }
    }
  }
  tga_io<color> output;

  output.output("figure_3_1.tga", image);
  return 0; 
}
