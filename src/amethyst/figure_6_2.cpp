/*
* This is (roughly) figure 6.2 from Peter Shirley's "Realistic Raytracing" book, second
* edition.
*/

#include "amethyst/graphics/raster.hpp"
#include "amethyst/graphics/rgbcolor.hpp"
#include "amethyst/graphics/image_loader.hpp"
#include "amethyst/graphics/image_texture.hpp"
#include "amethyst/graphics/renderer.hpp"
#include <iostream>

using namespace amethyst;

#define WIDTH 1000
#define HEIGHT 700

int main()
try
{
    std::string filename = "earth.ppm";

    image_texture<double, rgbcolor<double>> mytexture(filename, image_mapping_type::repeated);

    // Show it in a 2x2 grid
    constexpr double ycount = 2;
    constexpr double xcount = 2;

    constexpr vector3<double> normal = { 0, 0, -1 };
    constexpr double xfactor = xcount / double(WIDTH);
    constexpr double yfactor = -ycount / double(HEIGHT); // flipped because render() does (0,0) as the top left.
    constexpr double yoffset = double(HEIGHT - 1) / double(HEIGHT);

    auto image = render<double, rgbcolor<double>>(WIDTH, HEIGHT,
        [&](double x, double y) {
            coord2<double> c = { x * xfactor, y * yfactor + yoffset };
            auto pix = mytexture.get_color({}, c, normal);
            return pix;
        }, 16
    );

    save_image("figure_6_2.png", image);
    return 0;
}
catch (const std::exception& e)
{
    std::cerr << "Exception in main: " << e.what() << std::endl;
    return 1;
}
