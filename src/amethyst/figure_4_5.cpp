// g++ -g figure_4_5.cpp graphics/capabilities.cpp graphics/requirements.cpp -o figure_4_5 -lm -I. -Igraphics -Igeneral

/*
 * This is figure 4.5 from Peter Shirley's "Realistic Raytracing" book, second
 * edition.
 */

#include "math/frame.hpp"
#include "graphics/shapes/sphere.hpp"
#include "graphics/shapes/aggregate.hpp"
#include "graphics/raster.hpp"
#include "graphics/rgbcolor.hpp"
#include "graphics/image_loader.hpp"
#include "math/coord2.hpp"
#include "graphics/pinhole_camera.hpp"


#include <iostream>

using namespace amethyst;

using float_type = double;
// using float_type = float;

using point = point3<float_type>;
using vec = vector3<float_type>;
using color = rgbcolor<float_type>;
using coord = coord2<float_type>;

using std::cout;
using std::endl;

#define WIDTH 500
#define HEIGHT 500

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    aggregate<float_type> sl;

    point e(0, 0, 2);
    vec vup(0, 1, 0);
    vec g(0, 0, -2);
    float_type vw = 4;
    float_type vh = 4;
    float_type dist = 2;
    int nx = WIDTH;
    int ny = HEIGHT;

    pinhole_camera<float_type> camera(e, g, vup, vw, vh, dist, nx, ny);

    std::shared_ptr<shape<float_type>> sph_ptr = std::make_shared<sphere<float_type>>(point(0, 0, 0), sqrt(2));
    sl.add(sph_ptr);

    color light(0.7, 0.7, 0.7), dark(0.5, 0.5, 0.5), black(0, 0, 0);

    image<float_type> image(WIDTH, HEIGHT);
    intersection_info<float_type> stuff;

    cout << "objects=" << sl << endl;
    cout << "camera=" << camera << endl;

    intersection_requirements requirements;
    requirements.force_normal(true);

    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            float_type a = (x + 0.5);
            float_type b = (y + 0.5);
            ray_parameters<float_type> r = camera.get_ray(a, b);

            if (!sl.intersects_ray(r, stuff, requirements))
            {
                image(x, y) = black;
            }
            else
            {
                vec normal = stuff.get_normal();

                // Assign a color which is just a cosine between the normal and
                float_type f = 0.1 + 0.9 * std::max<float_type>(dotprod(normal, vec(0, 1, 0)), 0);
                image(x, y) = color(f, f, f);
            }
        }
    }

    save_image("figure_4_5.png", image);
    return 0;
}

