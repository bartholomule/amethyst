// g++ -g mobius.cpp graphics/image_converter.cpp -o mobius -lm -I. -Igraphics -Igeneral -Imath

#include <image.hpp>
#include <tga_io.h>
#include <rgbcolor.hpp>
#include <coord2.hpp>
#include <interval.hpp>
#include <string_format.hpp>
#include <complex>
#include <iostream>

using namespace amethyst;
using std::complex;
using std::cerr;
using std::endl;

template <class T>
complex<T> mobius_transform(const complex<T>& z
	, const complex<T>& a, const complex<T>& b
	, const complex<T>& c, const complex<T>& d)
{
	return (a * z + b) / (c * z + d);
}

template <class T>
rc_pointer<image<T> > mobius_transform(
	const rc_pointer<image<T> >& input, const complex<T>& a, const complex<T>& b,
	const complex<T>& c, const complex<T>& d, const coord2<T>& upperLeft, const coord2<T>& lowerRight,
	const rgbcolor<T>& background = rgbcolor<T>(0,0,0)
)
{
	// First, calculate a few constants...
	size_t nx = input->get_width();
	size_t ny = input->get_height();
	T xstep = (lowerRight.x() - upperLeft.x()) / nx;
	T ystep = (lowerRight.y() - upperLeft.y()) / ny;
	interval<T> xrange(0, nx);
	interval<T> yrange(0, ny);

	rc_pointer<image<T> > output(new image<T>(nx, ny));

	for( size_t y = 0; y < ny; ++y )
	{
		for( size_t x = 0; x < nx; ++x )
		{
			complex<T> z = complex<T>(upperLeft.x() + x * xstep
				, upperLeft.y() + y * ystep);

			complex<T> foz = mobius_transform(z, a, b, c, d);

			T mx = (foz.real() - upperLeft.x()) / xstep;
			T my = (foz.imag() - upperLeft.y()) / ystep;

			// cerr << string_format("transform of (%1,%2)-->(%3,%4)", x,y, mx, my) << endl;

			if( yrange.outside(my) || xrange.outside(mx) )
			{
				(*output)(x, y) = background;
			}
			else
			{
				// FIXME! Do color interpolation instead of block scaling (this should be ugly).
				(*output)(x,y) = (*input)(mx, my);
			}
		}
	}

	return output;
}

int main(int argc, const char** argv)
{
	typedef double float_type;

	// FIXME! Customize
	const char* const input_file = "input.tga";
	const char* const output_file = "output.tga";

	// If my math (in my head) works properly, this should be a 1:1 mapping of input to output.
	// I'm too tired right now (20080328) to figure out proper constants for rotation.
	// Simple scaling can be done by a = (1/scale, 0)
	const complex<float_type> a(1,0);
	const complex<float_type> b(0,0);
	const complex<float_type> c(0,0);
	const complex<float_type> d(1,0);

	const coord2<float_type> upperLeft(100,100);
	const coord2<float_type> lowerRight(-100,-100);

	tga_io<float_type> tga;

	rc_pointer<image<float_type> > input = tga.input(input_file);
	rc_pointer<image<float_type> > output = mobius_transform(
		input, a, b, c, d, upperLeft, lowerRight);

	tga.output(output_file, *output);

	return 0;
}
