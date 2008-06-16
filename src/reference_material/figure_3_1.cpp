// g++ -g figure_3_1.cpp graphics/capabilities.cpp graphics/requirements.cpp -o figure_3_1 -lm -I.

/*
 * This is figure 3.1 from Peter Shirley's "Realistic Raytracing" book, second
 * edition. 
 */

#include "graphics/sphere.hpp"
#include "graphics/triangle.hpp"
#include "graphics/aggregate.hpp"
#include "graphics/raster.hpp"
#include "graphics/tga_io.h"
#include "graphics/rgbcolor.hpp"

#include <iostream>

using namespace amethyst;

typedef double float_type;
//typedef float float_type;

typedef point3<float_type> point;
typedef vector3<float_type> vec;
typedef rgbcolor<float_type> color;

using std::cout;
using std::endl;

#define WIDTH 500
#define HEIGHT 500

int main(int argc, char** argv)
{
  (void) argc;
  (void) argv;
  aggregate<float_type> sl;

  rc_pointer<shape<float_type> > sph_ptr(new sphere<float_type>(point(250,250,-1000),
                                                                150));
  sl.add(sph_ptr);
  point p1(300,600,-800);
  point p2(0,100,-1000);
  point p3(450,20,-1000);
  rc_pointer<shape<float_type> > tri_ptr(new triangle<float_type>(p1,p2,p3));

  sl.add(tri_ptr);

  color light(0.7,0.7,0.7), dark(0.5,0.5,0.5), black(0,0,0);
  color red(0.7,0.0,0.0), blue(0.0,0.0,0.7);

  amethyst::image<float_type> image(WIDTH, HEIGHT);

  intersection_info<float_type> stuff;

  cout << "objects=" << sl << endl;

  std::cerr << "Generating image..." << std::endl;
  for( int y = 0; y < HEIGHT; ++y )
  {
    for( int x = 0; x < WIDTH; ++x )
    {
      unit_line3<float_type> r(point3<float_type>(500.0 * x / float_type(WIDTH),
                                                  500.0 * (HEIGHT-y)/float_type(HEIGHT),
                                                  0),
                               vector3<float_type>(0,0,-1));

      if( !sl.intersects_line(r, stuff) )
      {
        image(x,y) = dark;
      }
      else
      {
        const shape<float_type>* hit_shape = stuff.get_shape();

        if( dynamic_cast<const sphere<float_type>*>(hit_shape) != NULL )
        {
          image(x,y) = blue;
        }
        else if( dynamic_cast<const triangle<float_type>*>(hit_shape) != NULL )
        {
          image(x,y) = red;
        }
      }
    }
  }
  std::cerr << "Saving image..." << std::endl;
  tga_io<float_type> output;
  output.output("figure_3_1.tga", image);
  return 0; 
}
