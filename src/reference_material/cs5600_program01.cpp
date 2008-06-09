
/*
 * Program 1 - Kevin Harris - 20070203
 * Peter Shirley's CS5600 course, Spring 2007
 *
 * Requirements:
 * Create a web page that has a 24-bit RGB  image that you create by a
 * program. It should contain a number of 100 pixel by 100 pixel squares in RGB
 * space. These should have all combinations of the number 0, 127, and
 * 255. This means 27 squares total.
 *
 * Extra credit: In addition, do a cool 2D fractal image.
 *
 * ---------------------------------------------------------------------------
 * No.  I'm not in the course, but he invited people to complete the
 * assignments.  I thought this may be a way to get back into improving
 * amethyst and/or panorama.
 */


/*
 * What I've found when writing this:
 *  * The image output interface is not flexible enough.  It was written with
 *    floating point numbers in mind.  This makes the use of integer values
 *    (eg. 127) slightly more difficult.  For proper image creation with static
 *    constants (with no potential for floating point error), the image output
 *    would need to support some form of color conversion.
 *  * There was a bug in the interpolation class where it was trying to use a
 *    formula for bicubic filtering, instead of for bicubic interpolation.
 */


#include <graphics/raster.hpp>
#include <graphics/tga_io.h>
#include <graphics/rgbcolor.hpp>
#include <string>

#include <iostream>

using namespace amethyst;

typedef double number_type;
//typedef float number_type;
typedef rgbcolor<number_type> color;

#define SQUARE_SIZE 100

void program1()
{
	unsigned char values[3] = { 0, 127, 255 };
	// I'll spit these 27 squares out in 3 rows of 9.
	raster<rgbcolor<unsigned char> > image(SQUARE_SIZE * 9, SQUARE_SIZE * 3);
	for( int r = 0; r < 3; ++r )
	{
		for( int pix_y = 0; pix_y < SQUARE_SIZE; ++pix_y )
		{
			int y_offset = pix_y + r * SQUARE_SIZE;

			for( int g = 0; g < 3; ++g )
			{
				int x_offset1 = g * 3 * SQUARE_SIZE;

				for( int b = 0; b < 3; ++b )
				{
					int x_offset2 = b * SQUARE_SIZE;

					for( int pix_x = 0; pix_x < SQUARE_SIZE; ++pix_x )
					{
						int x_offset = pix_x + x_offset1 + x_offset2;
						image(x_offset, y_offset).set( values[r], values[g], values[b] );
					}
				}
			}
		}
	}
	tga_io<unsigned char> output;
	output.output("cs5600_program01.tga", image);
}


#include <math/quaternion.hpp>
#include <graphics/interpolated_value.hpp>

// These don't need to be quaternions, but I have them handy.
typedef quaternion<number_type> complex;
typedef interpolated_value<number_type, color> color_interp;

bool bailout_circle(complex c, number_type bail_value)
{
	return dotprod(c,c) > bail_value;
}

bool bailout_rectangle(complex c, number_type bail_value)
{
	return (c.getReal() > bail_value) || (length(c.getImag()) > bail_value);
}

template <class bailout>
number_type mandelbrot_escape(const complex& z, const complex& c, bailout bail, int max_iterations = 1000)
{
	complex current = z;
	int iteration;
	for( iteration = 0; iteration < max_iterations; ++iteration )
	{
		if( bail(current) )
		{
			break;
		}
		current = current * current + c;
	}
	return iteration / number_type(max_iterations);
}

rc_pointer<color_interp> get_colors()
{
	typedef interpolation_point<number_type, color> interp_type;
	quick_vector<interp_type> interp;

	interp.push_back(interp_type(0.00, color(0,0,0))); // (values inside the set)
	interp.push_back(interp_type(0.01, color(1,1,1))); // white
	interp.push_back(interp_type(0.95, color(1,1,0))); // orange
	interp.push_back(interp_type(0.97, color(1,0,0))); // red
	interp.push_back(interp_type(1.00, color(1,0,0))); // red

	return rc_pointer<color_interp>(create_interpolation<number_type,color>(interp).clone_new());
}

void program1_extra()
{
  	int width = 720;
	int height = 480;
	rc_pointer<color_interp> colors = get_colors();

	number_type x_min = -2;
	number_type x_max = 1;
	number_type y_min = -1;
	number_type y_max = 1;
	int escape_max_iterations = 256;
	number_type escape_max_value = 3;

	image<number_type> image(width, height);

	for( int y = 0; y < height; ++y )
	{
		number_type y_pos = y_min + (y * (y_max - y_min)) / height;

		std::cerr << "calculating line " << y << std::endl;

		for( int x = 0; x < width; ++x )
		{
			number_type x_pos = x_min + (x * (x_max - x_min)) / width;

			complex q(x_pos, complex::coord_type(y_pos, 0, 0));

			//			std::cerr << "pix(" << x_pos << ", " << y_pos << ") -->(" << x << ", " << y << ")" << std::endl;
			number_type escape = mandelbrot_escape(q, q,
				std::bind2nd(std::ptr_fun(&bailout_circle), escape_max_value),
				escape_max_iterations);

			color c = colors->interpolate(1 - escape);

			image(x,y) = c;
		}
	}

	tga_io<number_type> output;
	output.output("cs5600_program01_extra.tga", image);
}

int main(int argc, const char** argv)
{
	program1();
	program1_extra();
	return 0;
}

