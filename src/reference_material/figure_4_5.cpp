// g++ -g figure_4_5.cpp -o figure_4_5 -lm -I. -Igraphics -Igeneral

/*
 * This is figure 4.5 from Peter Shirley's "Realistic Raytracing" book, second
 * edition. 
 */

#include <math/frame.hpp>
#include <sphere.hpp>
#include <triangle.hpp>
#include <aggregate.hpp>
#include <raster.hpp>
#include <tga_io.h>
#include <rgbcolor.hpp>
#include <math/coord2.hpp>
#include <cmath>
#include <pinhole_camera.hpp>

#include <iostream>

using namespace amethyst;

//typedef double float_type;
typedef float float_type;

typedef point3<float_type> point;
typedef vector3<float_type> vec;
typedef rgbcolor<float_type> color;
typedef coord2<float_type> coord;

using std::cout;
using std::endl;

#define WIDTH 500
#define HEIGHT 500

int main(int argc, char** argv)
{
  (void) argc;
  (void) argv;
  aggregate<float_type> sl;

  point e(0,0,2);
  vec vup(0,1,0);
  vec g(0,0,-2);
  float_type vw=4;
  float_type vh=4;
  float_type dist=2;
  int nx = WIDTH;
  int ny = HEIGHT;
  
  pinhole_camera<float_type> camera(e, g, vup, vw, vh, dist, nx, ny);

  rc_pointer<shape<float_type> > sph_ptr(new sphere<float_type>(point(0,0,0), sqrt(2)));
  sl.add(sph_ptr);

  color light(0.7,0.7,0.7), dark(0.5,0.5,0.5), black(0,0,0);
  
  raster<color> image(WIDTH, HEIGHT);
  intersection_info<float_type> stuff;                                   

  cout << "objects=" << sl << endl;
  cout << "camera=" << camera << endl;
                                         
  for( int y = 0; y < HEIGHT; ++y )
  {
    for( int x = 0; x < WIDTH; ++x )
    {
      float_type a = (x + 0.5);
      float_type b = (y + 0.5);
      unit_line3<float_type> r = camera.get_ray(a,b);
      //      cout<< "a,b=" << a << "," << b << endl;

      /*
        float_type a = (x - 0.5) / float_type(WIDTH - 1);
        float_type b = (y - 0.5) / float_type(HEIGHT - 1);
        coord2<float_type> c(a,b);
        unit_line3<float_type> r = camera.get_ray(c);
        //      cout << "c(" << x << "," << y << ")=" << c << endl;
        */

      /*
        cout << "ray=" << r << endl;
        cout << "ray.point_at(1)=" << r.point_at(float_type(1)) << endl;
        cout << "ray.point_at_scaled(1)=" << r.point_at_scaled(float_type(1)) << endl;
      */
      
      if( !sl.intersects_line(r, stuff) )
      {
        image(x,y) = black;
      }
      else
      {
        // EVIL HACK! This is here because intersections do not yet calculate
        // normals.
        point intersection_point = (stuff.get_distance() * r.v() + r.o());
        vec normal = unit(intersection_point - point(0,0,0));


        // Assign a color which is just a cosine between the normal and 
        float_type f = 0.1 + 0.9 * std::max<float_type>(dotprod(normal, vec(0,1,0)), 0);
        image(x,y) = color(f,f,f);
      }
    }
  }

  tga_io<color> output;
  tga_io<color>::use_streambuf dummy;
  output.output("figure_4_5.tga", image, dummy);
  return 0; 

}

