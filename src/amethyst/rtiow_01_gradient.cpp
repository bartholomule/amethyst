// This the the basic gradient from Peter Shirley's Ray Tracing In One Weekend.

#include "math/unit_line3.hpp"
#include "graphics/rgbcolor.hpp"
#include "graphics/image_loader.hpp"
#include "graphics/image.hpp"
#include "graphics/renderer.hpp"

using namespace amethyst;
using Point = point3<double>;
using Color = rgbcolor<double>;
using Vec = vector3<double>;
using Image = image<double>;
using Line = unit_line3<double>;

Color color(const unit_line3<double>& l)
{
    double t = 0.5 * (l.direction().y() + 1);
    return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}

int main(int argc, const char** argv)
{
    const int nx = 400;
    const int ny = 200;
    const int spp = 1;

    const Point lower_left_corner(-2, -1, -1);
    const Vec horizontal(4, 0, 0);
    const Vec vertical(0, 2, 0);
    const Point origin(0, 0, 0);

    auto img = render<double, Color>(nx, ny,
        [&](double x, double y) {
            double u = double(x) / double(nx);
            double v = (double(ny - 1) - y) / double(ny); // flipped because render() does (0,0) as the top left.
            Line l(origin, lower_left_corner + u * horizontal + v * vertical);
            return color(l);
        },
        spp
    );

    save_image("rtiow_01_gradient.png", img);

    return 0;
}