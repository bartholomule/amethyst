// g++ -g mobius.cpp graphics/image_converter.cpp -o mobius -lm -I. -Igraphics -Igeneral -Imath

#include "graphics/image.hpp"
#include "graphics/image_loader.hpp"
#include "math/coord2.hpp"
#include "math/interval.hpp"
#include "general/string_format.hpp"
#include <complex>
#include <iostream>

using namespace amethyst;

template <class T>
std::complex<T> mobius_transform(const std::complex<T>& z
    , const std::complex<T>& a, const std::complex<T>& b
    , const std::complex<T>& c, const std::complex<T>& d)
{
    return (a * z + b) / (c * z + d);
}

template <class T>
image<T> mobius_transform(
    const image<T>& input, const std::complex<T>& a, const std::complex<T>& b,
    const std::complex<T>& c, const std::complex<T>& d, const coord2<T>& upperLeft, const coord2<T>& lowerRight,
    const rgbcolor<T>& background = rgbcolor<T>(0, 0, 0)
    )
{
    // First, calculate a few constants...
    size_t nx = input.get_width();
    size_t ny = input.get_height();
    T xstep = (lowerRight.x() - upperLeft.x()) / nx;
    T ystep = (lowerRight.y() - upperLeft.y()) / ny;
    interval<T> xrange(0, T(nx));
    interval<T> yrange(0, T(ny));

    image<T> output(nx, ny);

    for (size_t y = 0; y < ny; ++y)
    {
        for (size_t x = 0; x < nx; ++x)
        {
            std::complex<T> z = std::complex<T>(upperLeft.x() + x * xstep,
                upperLeft.y() + y * ystep);

            std::complex<T> foz = mobius_transform(z, a, b, c, d);

            T mx = (foz.real() - upperLeft.x()) / xstep;
            T my = (foz.imag() - upperLeft.y()) / ystep;

            // cerr << string_format("transform of (%1,%2)-->(%3,%4)", x,y, mx, my) << endl;

            if (yrange.outside(my) || xrange.outside(mx))
            {
                output(x, y) = background;
            }
            else
            {
                // FIXME! Do color interpolation instead of block scaling (this should be ugly).
                output(x, y) = input(size_t(mx), size_t(my));
            }
        }
    }

    return output;
}

int main(int argc, const char** argv)
{
    typedef double float_type;

    std::string input_file = (argc > 1) ? argv[1] : "earth.ppm";
    std::string output_file = (argc > 2) ? argv[2] : "mobius_output.png";

    // If my math (in my head) works properly, this should be a 1:1 mapping of input to output.
    // I'm too tired right now (20080328) to figure out proper constants for rotation.
    // Simple scaling can be done by a = (1/scale, 0)
    const std::complex<float_type> a(1, 0);
    const std::complex<float_type> b(0, 0);
    const std::complex<float_type> c(0, 0);
    const std::complex<float_type> d(1, 0);

    const coord2<float_type> upperLeft(100, 100);
    const coord2<float_type> lowerRight(-100, -100);

    tga_io<float_type> tga;

    auto input = convert_image<float_type>(load_image_with_stb(input_file));
    auto output = mobius_transform(input, a, b, c, d, upperLeft, lowerRight);

    save_image(output_file, output);

    return 0;
}
