// This the the basic gradient + sphere +normal from Peter Shirley's Ray Tracing In One Weekend.

#include "math/unit_line3.hpp"
#include "graphics/rgbcolor.hpp"
#include "graphics/image_loader.hpp"
#include "graphics/image.hpp"
#include "graphics/renderer.hpp"
#include "graphics/shapes/aggregate.hpp"
#include "graphics/shapes/sphere.hpp"
#include "graphics/texture/solid_texture.hpp"
#include "graphics/pinhole_camera.hpp"
#include "graphics/interpolated_value.hpp"
#include "graphics/samplegen3d.hpp"

using namespace amethyst;
using Point = point3<double>;
using Color = rgbcolor<double>;
using Vec = vector3<double>;
using Image = image<double>;
using Line = unit_line3<double>;
using Info = intersection_info<double, Color>;

namespace
{
    sphere_sample_3d<double> g_sampler;
}


Vec random_vec_in_sphere()
{
    return Vec(g_sampler.next_sample());
}

Color lighting(const Point& p, const Vec& v)
{
    // No lighting.  Only the background is reflected.
    return { 0, 0, 0 };
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
        return { 0,0,0 };
    }

    bool reflect_ray(const ray_parameters<double, Color>& ray, const intersection_info<double, Color>& intersection, ray_parameters<double, Color>& reflected, Color& attenuation) const override
    {
        if (ray.perfect_reflection(intersection, reflected))
        {
            const auto& p = intersection.get_first_point();
            const auto& n = intersection.get_normal();
            Point target = p + n + random_vec_in_sphere();
            reflected.set_line(Line(p, target - p, reflected.get_line().limits()));
            attenuation = { 0.5, 0.5, 0.5 };
            return true;
        }
        return false;
    }

    std::string internal_members(const std::string& indentation, bool prefix_with_classname) const override
    {
        return {};
    }

    material_capabilities get_material_capabilties() const override
    {
        return material_capabilities::DIFFUSE | material_capabilities::REFLECTIVE;
    }
};

struct trivial_camera : public base_camera<double, Color>
{
public:
    trivial_camera(size_t w, size_t h, Point ll, Vec horiz, Vec vert, Point orig)
        : base_camera<double, Color>(w, h)
        , lower_left_corner(ll)
        , horizontal(horiz)
        , vertical(vert)
        , origin(orig)
    {
    }

    ray_parameters<double, Color> get_ray(const coord2<double>& sample, double time = 0) const override
    {
        ray_parameters<double, Color> result;
        result.set_line(Line(origin, lower_left_corner + sample.x() * horizontal + sample.y() * vertical));
        return result;
    }

    ray_parameters<double, Color> get_ray(const double& px, const double& py, double time = 0) const override
    {
        double u = px / width();
        double v = (double(height() - 1) - py) / double(height()); // flipped because render() does (0,0) as the top left.
        return get_ray(coord2<double>(u, v), time);
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
    constexpr size_t spp = 16;
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
    auto scene = std::make_shared<aggregate<double, Color>>();
    scene->add(std::make_shared<sphere<double, Color>>(Point(0, 0, -1), 0.5));
    scene->add(std::make_shared<sphere<double, Color>>(Point(0, -100.5, -1), 100));

    auto img = render<double, Color>(
        camera, scene, scene_texture, nx, ny,
        requirements, lighting, background_color, spp
        );

    save_image("rtiow_05_diffuse.png", img);

    return 0;
}
