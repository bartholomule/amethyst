// g++ -g figure_4_10.cpp graphics/capabilities.cpp graphics/requirements.cpp -o figure_4_10 -lm -I. -Igraphics -Igeneral

/*
 * This is figure 4.10 from Peter Shirley's "Realistic Raytracing" book, second
 * edition.
 */

#include "amethyst/math/frame.hpp"
#include "amethyst/graphics/sphere.hpp"
#include "amethyst/graphics/dynamic_sphere.hpp"
#include "amethyst/graphics/triangle.hpp"
#include "amethyst/graphics/aggregate.hpp"
#include "amethyst/graphics/raster.hpp"
#include "amethyst/graphics/rgbcolor.hpp"
#include "amethyst/graphics/image_loader.hpp"
#include "amethyst/math/coord2.hpp"
#include "amethyst/graphics/pinhole_camera.hpp"

#include "amethyst/general/random.hpp"
#include "amethyst/graphics/samplegen2d.hpp"
#include "amethyst/graphics/samplegen1d.hpp"
#include "amethyst/graphics/simple_texture.hpp"
#include "amethyst/graphics/noise_texture.hpp"


#include <iostream>

using namespace amethyst;

typedef double float_type;
//typedef float float_type;

typedef point3<float_type> point;
using vec = vector3<float_type>;
using color = rgbcolor<float_type>;
using coord = coord2<float_type>;
typedef simple_texture<float_type, color> simple_texture_type;
typedef noise_texture<float_type, color> noisy_texture_type;

using std::cout;
using std::endl;

#define WIDTH 600
#define HEIGHT 600
//#define TIME_MAG 1024
#define TIME_MAG 9

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    aggregate<float_type> sl;

    point e(0, 0, 24);
    vec vup(0, 1, 0);
    vec g(0, 0, -2);
    float_type vw = 1;
    float_type vh = 1;
    float_type dist = 2;
    int nx = WIDTH;
    int ny = HEIGHT;

    pinhole_camera<float_type> camera(e, g, vup, vw, vh, dist, nx, ny, interval<float_type>(0, 1));

    std::shared_ptr<shape<float_type>> sph_ptr1(new sphere<float_type>(point(-2, -2, 0), sqrt(2)));
    sl.add(sph_ptr1);
    std::shared_ptr<shape<float_type>> sph_ptr2(new dynamic_sphere<float_type>(point(2, -2, 0), 0.25,
                                                                               point(3, -3, 0), 1,
                                                                               sqrt(2), 0,
                                                                               sqrt(2), 1));
    sl.add(sph_ptr2);
    std::shared_ptr<shape<float_type>> sph_ptr3(new dynamic_sphere<float_type>(point(2, 2, 0), 0.5,
                                                                               point(3, 3, 0), 1,
                                                                               sqrt(2), 0,
                                                                               sqrt(2), 1));
    sl.add(sph_ptr3);
    std::shared_ptr<shape<float_type>> sph_ptr4(new dynamic_sphere<float_type>(point(-2, 2, 0), 0.66,
                                                                               point(-3, 3, 0), 1,
                                                                               sqrt(2), 0,
                                                                               sqrt(2), 1));
    sl.add(sph_ptr4);

    color light(0.7, 0.7, 0.7), dark(0.5, 0.5, 0.5), black(0, 0, 0);

    raster<color> image(WIDTH, HEIGHT);
    intersection_info<float_type> stuff;

    simple_texture_type texture = simple_texture_type(color(0.9, 0.1, 0.1));
    //  noisy_texture_type texture;

    cout << "objects=" << sl << endl;
    cout << "camera=" << camera << endl;
    cout << "texture=" << texture << endl;

    intersection_requirements requirements;
    requirements.force_normal(true);
    requirements.force_uv(true);
    requirements.force_first_only(true);

    sample_generator_2d<float_type> *gen_2d = new jitter_sample_2d<float_type>();
    sample_generator_1d<float_type> *gen_1d = new jitter_sample_1d<float_type>();

    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            color current_color = black;
            std::vector<coord2<float_type>> samples = gen_2d->get_samples(TIME_MAG);
            std::vector<float_type> time_samples = gen_1d->get_samples(TIME_MAG);

            for (int time_sample = 0; time_sample < TIME_MAG; ++time_sample)
            {
                float_type a = x + samples[time_sample].x();
                float_type b = y + samples[time_sample].y();
                float_type c = time_samples[time_sample];
                ray_parameters<float_type> r = camera.get_ray(a, b, c);

                if (!sl.intersects_ray(r, stuff, requirements))
                {
                    current_color += black;
                }
                else
                {
                    vec normal = stuff.get_normal();

                    // std::cout << string_format("x=%1 y=%2 s=%3 n=%4", x, y, c, normal) << std::endl;

                    // Assign a color which is just a cosine between the normal and the light vector (y).
                    float_type f = 0.1 + 0.9 * std::max<float_type>(dotprod(normal, vec(0, 1, 0)), 0);

                    current_color += f * texture.get_color(stuff.get_first_point(), stuff.get_uv());
                }
            }
            current_color *= 1.0 / float_type(TIME_MAG);
            image(x, y) = current_color;
        }
    }

    std::cerr << "Saving image as figure_4_10.png ..." << std::endl;
    save_image("figure_4_10.png", image);
    return 0;
}

