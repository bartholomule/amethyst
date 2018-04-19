// This the the basic gradient + sphere +normal from Peter Shirley's Ray Tracing In One Weekend.

#include "math/unit_line3.hpp"
#include "graphics/rgbcolor.hpp"
#include "graphics/image_loader.hpp"
#include "graphics/image.hpp"
#include "graphics/renderer.hpp"
#include "graphics/shapes/aggregate.hpp"
#include "graphics/shapes/sphere.hpp"
#include "graphics/pinhole_camera.hpp"
#include "graphics/interpolated_value.hpp"
#include "graphics/samplegen3d.hpp"
#include "graphics/texture/lambertian.hpp"
#include "graphics/texture/metal.hpp"


using namespace amethyst;
using Point = point3<double>;
using Color = rgbcolor<double>;
using Vec = vector3<double>;
using Image = image<double>;
using Line = unit_line3<double>;
using Info = intersection_info<double,Color>;
using Lambertian = lambertian<double, Color>;
using Metal = metal<double, Color>;


namespace
{
    sphere_sample_3d<double> g_sampler;
}


void progress(double percentage)
{
    std::cout << "\r" << percentage << "% complete" << std::flush;
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

struct trivial_camera : public base_camera<double,Color>
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

    ray_parameters<double,Color> get_ray(const coord2<double>& sample, double time = 0) const override
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
    constexpr size_t spp = 4;
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
    auto scene_texture = std::make_shared<Lambertian>(Color{ 0.8, 0.6, 0.2 });
    auto scene = std::make_shared<aggregate<double, Color>>();
    scene->add(std::make_shared<sphere<double, Color>>(Point(0, 0, -1), 0.5, std::make_shared<Lambertian>(Color{ 0.8,0.3,0.3 })));
    scene->add(std::make_shared<sphere<double, Color>>(Point(0, -100.5, -1), 100, std::make_shared<Lambertian>(Color{ 0.8,0.8,0.0 })));
    scene->add(std::make_shared<sphere<double, Color>>(Point(1, 0, -1), 0.5, std::make_shared<Metal>(Color{0.8,0.6,0.2}, 1.0)));
    scene->add(std::make_shared<sphere<double, Color>>(Point(-1, 0, -1), 0.5, std::make_shared<Metal>(Color{ 0.8,0.8,0.8 }, 0.3)));
    auto sampler = std::make_shared<regular_sample_2d<double>>();

    std::cout << "Scene: " << scene << std::endl;

    auto img = render<double, Color>(
        camera, scene, scene_texture, nx, ny,
        requirements, lighting, background_color, spp,
        sampler, progress
        );

    save_image("rtiow_06_metal.png", img);

    return 0;
}
