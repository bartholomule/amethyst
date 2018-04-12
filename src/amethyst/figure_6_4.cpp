/*
* This is figure 6.4 from Peter Shirley's "Realistic Raytracing" book, second
* edition.
*/

#include "amethyst/graphics/renderer.hpp"
#include "amethyst/graphics/image_loader.hpp"
#include "amethyst/graphics/texture/image_texture.hpp"
#include "amethyst/graphics/pinhole_camera.hpp"
#include "amethyst/graphics/shapes/aggregate.hpp"
#include "amethyst/graphics/shapes/sphere.hpp"

#include <iostream>

using namespace amethyst;

typedef double float_type;
//typedef float float_type;

typedef point3<float_type> point;
using vec = vector3<float_type>;
using color = rgbcolor<float_type>;
using coord = coord2<float_type>;

#define WIDTH 800
#define HEIGHT 800
#define SAMPLES_PER_PIXEL 1

int main()
try
{
    constexpr point e(0, 0, 42);
    constexpr vec vup(0, 1, 0);
    constexpr vec g(0, 0, -2);
    constexpr float_type vw = 1;
    constexpr float_type vh = 1;
    constexpr float_type dist = 2;
    constexpr size_t nx = WIDTH;
    constexpr size_t ny = HEIGHT;
    constexpr size_t spp = SAMPLES_PER_PIXEL;

    auto camera = std::make_shared<pinhole_camera<float_type,color>>(e, g, vup, vw, vh, dist, nx, ny, interval<float_type>(0, 1));
    auto scene = std::make_shared<aggregate<float_type, color>>();
    scene->add(std::make_shared<sphere<float_type, color>>(point(0, 0, 0), 10));

    std::cout << "objects=" << scene << std::endl;
    std::cout << "camera=" << camera << std::endl;

    intersection_requirements requirements;
    requirements.force_normal(true);
    requirements.force_uv(true);
    requirements.force_first_only(true);

    {
        auto light_fn = [](const point3<float_type>& p, const vector3<float_type>& n) -> rgbcolor<float_type>
        {
            return { 1, 1, 1 };
        };

        const std::string filename = "earth.ppm";
        auto scene_texture = std::make_shared<image_texture<float_type, color>>(filename, image_mapping_type::once);
        auto image = render<float_type, color>(camera, scene, scene_texture, WIDTH, HEIGHT, requirements, light_fn, nullptr, spp);

        save_image("figure_6_1.png", image);
    }
    {
        auto light_fn = [](const point3<float_type>& p, const vector3<float_type>& n) -> rgbcolor<float_type>
        {
            const vec light_vector = unit(vec(1, -1, -1));
            const float_type ambience(0.05);

            // Assign a color which is just a cosine between the normal and the light vector.
            float_type f = ambience + (1 - ambience) * std::max<float_type>(dotprod(n, -light_vector) - AMETHYST_EPSILON, 0);

            return rgbcolor<float_type>(f);
        };

        const std::string filename = "uv_grid_400x400.ppm";
        auto scene_texture = std::make_shared<image_texture<float_type, color>>(filename, image_mapping_type::once);
        auto image = render<float_type, color>(camera, scene, scene_texture, WIDTH, HEIGHT, requirements, light_fn, nullptr, spp);

        save_image("figure_6_4.png", image);
    }

    return 0;
}
catch (const std::exception& e)
{
    std::cerr << "Exception in main: " << e.what() << std::endl;
    return 1;
}
catch (...)
{
    std::cerr << "Unknown exception in main." << std::endl;
    return 1;
}
