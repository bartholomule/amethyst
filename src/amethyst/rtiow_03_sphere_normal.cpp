// This the the basic gradient + sphere +normal from Peter Shirley's Ray Tracing In One Weekend.

#include "math/unit_line3.hpp"
#include "graphics/rgbcolor.hpp"
#include "graphics/image_loader.hpp"
#include "graphics/image.hpp"
#include "graphics/sphere.hpp"

using namespace amethyst;
using Point = point3<double>;
using Color = rgbcolor<double>;
using Vec = vector3<double>;
using Image = image<double>;
using Line = unit_line3<double>;
using Info = intersection_info<double>;

Color color(const unit_line3<double>& l)
{
    sphere<double> s(Point(0, 0, -1), 0.5);
    Info info;
    intersection_requirements requirements;
    requirements.force_first_only(true);
    requirements.force_normal(true);
    if( s.intersects_line(l, info, requirements))
    {
        auto n = info.get_normal();
        return 0.5 * Color(n.x() + 1, n.y() + 1, n.z() + 1);
    }

    double t = 0.5 * (l.direction().y() + 1);
    return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}

int main(int argc, const char** argv)
{
    int nx = 200;
    int ny = 100;

    Point lower_left_corner(-2, -1, -1);
    Vec horizontal(4, 0, 0);
    Vec vertical(0, 2, 0);
    Point origin(0, 0, 0);

    Image img(nx, ny);
    for (int j = 0; j < ny; ++j)
    {
        for (int i = 0; i < nx; ++i)
        {
            double u = double(i) / double(nx);
            double v = double(j) / double(ny);
            Line l(origin, lower_left_corner + u * horizontal + v * vertical);
            img(i, ny - j - 1) = color(l);
        }
    }

    save_image("rtiow_03_sphere_normal.png", img);

    return 0;
}