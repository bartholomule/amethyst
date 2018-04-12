/*
 * This is figure 5.7 from Peter Shirley's "Realistic Raytracing" book, second
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
#include "amethyst/graphics/texture/marble_texture.hpp"


#include <iostream>

using namespace amethyst;

typedef double float_type;
//typedef float float_type;

typedef point3<float_type> point;
using vec = vector3<float_type>;
using color = rgbcolor<float_type>;
using coord = coord2<float_type>;
typedef marble_texture<float_type, color> texture_type;

using std::cout;
using std::endl;

#define WIDTH 800
#define HEIGHT 800
#define SAMPLES_PER_PIXEL 1

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    aggregate<float_type> sl;

    point e(0, 0, 42);
    vec vup(0, 1, 0);
    vec g(0, 0, -2);
    float_type vw = 1;
    float_type vh = 1;
    float_type dist = 2;
    int nx = WIDTH;
    int ny = HEIGHT;

    pinhole_camera<float_type> camera(e, g, vup, vw, vh, dist, nx, ny, interval<float_type>(0, 1));

    std::shared_ptr<shape<float_type>> sph_ptr1(new sphere<float_type>(point(0, 0, 0), 10));
    sl.add(sph_ptr1);

    color light(0.7, 0.7, 0.7), dark(0.5, 0.5, 0.5), black(0, 0, 0);

    raster<color> image(WIDTH, HEIGHT);
    intersection_info<float_type> stuff;

    //	texture_type texture(6.67, 5); // I calculate in line width.  Pete does it in stripes/area  1/0.15 = 6.67
    texture_type texture(6.67, 10); // I calculate in line width.  Pete does it in stripes/area  1/0.15 = 6.67
    vec light_vector = unit(vec(1, -1, -1));
    float_type ambience(0.05);

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
            std::vector<coord2<float_type>> samples = gen_2d->get_samples(SAMPLES_PER_PIXEL);

            for (int pixel_sample = 0; pixel_sample < SAMPLES_PER_PIXEL; ++pixel_sample)
            {
                float_type a = x + samples[pixel_sample].x();
                float_type b = y + samples[pixel_sample].y();
                ray_parameters<float_type> r = camera.get_ray(a, b);

                if (!sl.intersects_ray(r, stuff, requirements))
                {
                    current_color += black;
                }
                else
                {
                    vec normal = stuff.get_normal();

                    // Assign a color which is just a cosine between the normal and the light vector.
                    float_type f = ambience + (1 - ambience) * std::max<float_type>(dotprod(normal, -light_vector) - AMETHYST_EPSILON, 0);

                    current_color += f * texture.get_color(stuff.get_first_point(), stuff.get_uv(), normal);
                }
            }
            current_color *= 1.0 / float_type(SAMPLES_PER_PIXEL);
            image(x, y) = current_color;
        }
    }

    save_image("figure_5_7.png", image);
    return 0;
}

