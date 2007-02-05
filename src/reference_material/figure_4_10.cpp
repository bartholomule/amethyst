// g++ -g figure_4_10.cpp graphics/capabilities.cpp graphics/requirements.cpp -o figure_4_10 -lm -I. -Igraphics -Igeneral

/*
 * This is figure 4.10 from Peter Shirley's "Realistic Raytracing" book, second
 * edition.
 */

#include <math/frame.hpp>
#include <graphics/sphere.hpp>
#include <graphics/dynamic_sphere.hpp>
#include <graphics/triangle.hpp>
#include <graphics/aggregate.hpp>
#include <graphics/raster.hpp>
#include <graphics/tga_io.h>
#include <graphics/rgbcolor.hpp>
#include <math/coord2.hpp>
#include <cmath>
#include <graphics/pinhole_camera.hpp>

#include <graphics/random.hpp>
#include <graphics/samplegen2d.hpp>
#include <graphics/samplegen1d.hpp>

#include <iostream>

using namespace amethyst;

typedef double float_type;
//typedef float float_type;

typedef point3<float_type> point;
typedef vector3<float_type> vec;
typedef rgbcolor<float_type> color;
typedef coord2<float_type> coord;

using std::cout;
using std::endl;

#define WIDTH 300
#define HEIGHT 300
#define TIME_MAG 1024

int main(int argc, char** argv)
{
	(void) argc;
	(void) argv;
	aggregate<float_type> sl;

	point e(0,0,24);
	vec vup(0,1,0);
	vec g(0,0,-2);
	float_type vw=1;
	float_type vh=1;
	float_type dist=2;
	int nx = WIDTH;
	int ny = HEIGHT;

	pinhole_camera<float_type> camera(e, g, vup, vw, vh, dist, nx, ny, interval<float_type>(0,1));

	rc_pointer<shape<float_type> > sph_ptr1(new sphere<float_type>(point(-2,-2,0), sqrt(2)));
	sl.add(sph_ptr1);
	rc_pointer<shape<float_type> > sph_ptr2(new dynamic_sphere<float_type>(point(2,-2,0), 0.25,
															 point(3,-3,0), 1,
															 sqrt(2), 0,
															 sqrt(2), 1));
	sl.add(sph_ptr2);
	rc_pointer<shape<float_type> > sph_ptr3(new dynamic_sphere<float_type>(point(2,2,0), 0.5,
															 point(3,3,0), 1,
															 sqrt(2), 0,
															 sqrt(2), 1));
	sl.add(sph_ptr3);
	rc_pointer<shape<float_type> > sph_ptr4(new dynamic_sphere<float_type>(point(-2,2,0), 0.66,
															 point(-3,3,0), 1,
															 sqrt(2), 0,
															 sqrt(2), 1));
	sl.add(sph_ptr4);

	color light(0.7,0.7,0.7), dark(0.5,0.5,0.5), black(0,0,0);

	raster<color> image(WIDTH, HEIGHT);
	intersection_info<float_type> stuff;

	cout << "objects=" << sl << endl;
	cout << "camera=" << camera << endl;

	intersection_requirements requirements;
	requirements.force_normal(true);

	sample_generator_2d<float_type> *gen_2d = new jitter_sample_2d<float_type>();
	sample_generator_1d<float_type> *gen_1d = new jitter_sample_1d<float_type>();

	for( int y = 0; y < HEIGHT; ++y )
	{
		for( int x = 0; x < WIDTH; ++x )
		{
			color current_color = black;
			quick_vector<coord2<float_type> > samples = gen_2d->get_samples(TIME_MAG);
			quick_vector<float_type> time_samples = gen_1d->get_samples(TIME_MAG);

			for( int time_sample = 0; time_sample < TIME_MAG; ++time_sample )
			{
				float_type a = x + samples[time_sample].x();
				float_type b = y + samples[time_sample].y();
				float_type c = time_samples[time_sample];
				ray_parameters<float_type> r = camera.get_ray(a, b, c);
				//	cout<< "a,b,c=" << a << "," << b << "," << c << endl;

				//	float_type a = (x + 0.5);
				//	float_type b = (y + 0.5);
				//	ray_parameters<float_type> r = camera.get_ray(a,b, time_sample / float_type(TIME_MAG));

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

				if( !sl.intersects_ray(r, stuff, requirements) )
				{
					current_color += black;
				}
				else
				{
					vec normal = stuff.get_normal();

					// Assign a color which is just a cosine between the normal and
					float_type f = 0.1 + 0.9 * std::max<float_type>(dotprod(normal, vec(0,1,0)), 0);
					current_color += color(f,f,f);
				}
			}
			current_color *= 1.0/float_type(TIME_MAG);
			image(x,y) = current_color;
		}
	}

	tga_io<float_type> output;
	output.output("figure_4_10.tga", image);
	return 0;
}

