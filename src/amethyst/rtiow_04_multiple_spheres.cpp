// This the the basic gradient + sphere +normal from Peter Shirley's Ray Tracing In One Weekend.

#include "math/unit_line3.hpp"
#include "graphics/rgbcolor.hpp"
#include "graphics/image_loader.hpp"
#include "graphics/image.hpp"
#include "graphics/sphere.hpp"
#include "graphics/renderer.hpp"
#include "graphics/aggregate.hpp"
#include "graphics/texture/solid_texture.hpp"
#include "graphics/pinhole_camera.hpp"
#include "graphics/aggregate.hpp"
#include "graphics/interpolated_value.hpp"

#include "math/ct_math.hpp"

using namespace amethyst;
using Point = point3<double>;
using Color = rgbcolor<double>;
using Vec = vector3<double>;
using Image = image<double>;
using Line = unit_line3<double>;
using Info = intersection_info<double>;

Color lighting(const Point& p, const Vec& v)
{
    // Uniform lighting everywhere.
    return { 1, 1, 1 };
}

Color background_color(double x, double y, const Line& l)
{
    // Fade from a light blue at the top to a white at the bottom.
    // y = [-1,1] --> t = [0,1]
    double t = 0.5 * (l.direction().y() + 1);
    return linear_interpolate(t, Color(1.0, 1.0, 1.0), Color(0.5, 0.7, 1.0));
}

class normal_scene_texture : public solid_texture<double, Color>
{
public:
    Color get_color_at_point(const Point& location, const Vec& normal) const override
    {
        // This just shifts the normal, which can be in [-1,1]^3 into a color from [0,1]^3
        return 0.5 * Color(normal.x() + 1, normal.y() + 1, normal.z() + 1);
    }
    std::string internal_members(const std::string& indentation, bool prefix_with_classname) const override
    {
        return { };
    }
};

struct trivial_camera : public base_camera<double>
{
public:
    trivial_camera(size_t w, size_t h, Point ll, Vec horiz, Vec vert, Point orig)
        : base_camera<double>(w, h)
        , lower_left_corner(ll)
        , horizontal(horiz)
        , vertical(vert)
        , origin(orig)
    {
    }

    ray_parameters<double> get_ray(const coord2<double>& sample, double time = 0) const override
    {
        ray_parameters<double> result;
        result.set_line(Line(origin, lower_left_corner + sample.x() * horizontal + sample.y() * vertical));
        return result;
    }

    ray_parameters<double> get_ray(const double& px, const double& py, double time = 0) const override
    {
        double u = px / width();
        double v = (double(height() - 1) - py) / double(height()); // flipped because render() does (0,0) as the top left.
        return get_ray(coord2<double>(u,v), time);
    }
    Point lower_left_corner;
    Vec horizontal;
    Vec vertical;
    Point origin;
};

int main(int argc, const char** argv)
{
    constexpr size_t nx = 400;
    constexpr size_t ny = 200;
    constexpr size_t spp = 1;
    // These are the constants in Pete's example.
    constexpr Point lower_left_corner(-2, -1, -1);
    constexpr Vec horizontal(4, 0, 0);
    constexpr Vec vertical(0, 2, 0);
    constexpr Point origin(0, 0, 0);

    intersection_requirements requirements;
    requirements.force_first_only(true);
    requirements.force_normal(true);
    requirements.force_uv(true);

    auto camera = std::make_shared<trivial_camera>(nx, ny, lower_left_corner, horizontal, vertical, origin);
    auto scene_texture = std::make_shared<normal_scene_texture>();
    auto scene = std::make_shared<aggregate<double>>();
    scene->add(std::make_shared<sphere<double>>(Point(0, 0, -1), 0.5));
    scene->add(std::make_shared<sphere<double>>(Point(0, -100.5, -1), 100));

    auto img = render<double, Color>(
        camera, scene, scene_texture, nx, ny,
        requirements, lighting, background_color, spp
    );

    save_image("rtiow_04_multiple_spheres.png", img);

    return 0;
}
