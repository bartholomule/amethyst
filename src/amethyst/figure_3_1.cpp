// g++ -g figure_3_1.cpp graphics/capabilities.cpp graphics/requirements.cpp -o figure_3_1 -lm -I.

/*
 * This is figure 3.1 from Peter Shirley's "Realistic Raytracing" book, second
 * edition.
 */

#include "graphics/shapes/sphere.hpp"
#include "graphics/shapes/triangle.hpp"
#include "graphics/shapes/aggregate.hpp"
#include "graphics/raster.hpp"
#include "graphics/image_loader.hpp"
#include "graphics/rgbcolor.hpp"
#include "graphics/renderer.hpp"

#include <iostream>

using namespace amethyst;

typedef double float_type;
//typedef float float_type;

typedef point3<float_type> point;
using vec = vector3<float_type>;
using color = rgbcolor<float_type>;

using std::cout;
using std::endl;

#define WIDTH 500
#define HEIGHT 500

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    shape_ptr<float_type,color> sph_ptr(new sphere<float_type,color>(point(250, 250, -1000), 150));

    point p1(300, 600, -800);
    point p2(0, 100, -1000);
    point p3(450, 20, -1000);
    shape_ptr<float_type,color> tri_ptr(new triangle<float_type, color>(p1, p2, p3));


    aggregate<float_type,color> sl;
    sl.add(sph_ptr);
    sl.add(tri_ptr);

    color light(0.7, 0.7, 0.7), dark(0.5, 0.5, 0.5), black(0, 0, 0);
    color red(0.7, 0.0, 0.0), blue(0.0, 0.0, 0.7);

    intersection_info<float_type,color> stuff;

    cout << "objects=" << sl << endl;

    std::cerr << "Generating image..." << std::endl;

    auto image = render<float_type, color>(WIDTH, HEIGHT,
        [&](float_type x, float_type y)
        {
            unit_line3<float_type> r(
                point3<float_type>(
                    500.0 * x / float_type(WIDTH),
                    500.0 * (HEIGHT - y) / float_type(HEIGHT),
                    0),
                vector3<float_type>(0, 0, -1)
            );

            if (!sl.intersects_line(r, stuff))
                return dark;

            const shape<float_type,color>* hit_shape = stuff.get_shape();

            if (dynamic_cast<const sphere<float_type,color>*>(hit_shape) != NULL)
                return blue;
            else if (dynamic_cast<const triangle<float_type,color>*>(hit_shape) != NULL)
                return red;
            else
                return black;
        }
    );

    std::cerr << "Saving image as figure_3_1.png..." << std::endl;
    save_image("figure_3_1.png", image);
    return 0;
}
