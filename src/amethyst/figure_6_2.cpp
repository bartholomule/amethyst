/*
* This is (roughly) figure 6.2 from Peter Shirley's "Realistic Raytracing" book, second
* edition.
*/

#include "amethyst/graphics/raster.hpp"
#include "amethyst/graphics/rgbcolor.hpp"
#include "amethyst/graphics/image_loader.hpp"
#include "amethyst/graphics/image_texture.hpp"
#include <iostream>

using namespace amethyst;

using float_type = double;
using point = point3<float_type>;
using vec = vector3<float_type>;
using color = rgbcolor<float_type>;
using uv_coord = coord2<float_type>;

#define WIDTH 800
#define HEIGHT 800

int main(int argc, char** argv)
try
{
    (void)argc;
    (void)argv;

    raster<color> image(WIDTH, HEIGHT);
    std::string filename = "earth.ppm";

    image_texture<float_type, color> mytexture(filename, image_mapping_type::repeated);

    for (int y = 0; y < HEIGHT; ++y)
    {
        for (int x = 0; x < WIDTH; ++x)
        {
            // UV coords should be from the bottom left.
            float_type u = x;
            float_type v = HEIGHT - y;

            image(x, y) = mytexture.get_color(point(), uv_coord(u, v));
        }
    }

    save_image("figure_6_2.ppm", image);
    return 0;
}
catch (const std::exception& e)
{
    std::cerr << "Exception in main: " << e.what() << std::endl;
    return 1;
}
