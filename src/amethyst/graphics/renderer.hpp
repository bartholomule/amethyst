#pragma once

#include "base_camera.hpp"
#include "colors.hpp"
#include "raster.hpp"
#include "texture/texture.hpp"
#include "samplegen2d.hpp"
#include "graphics/shapes/shape.hpp"
#include "intersection_info.hpp"
#include "requirements.hpp"
#include "samplegen2d.hpp"
#include "ray_parameters.hpp"
#include "general/template_functions.hpp"
#include <functional>

namespace amethyst
{
    template <typename T, typename color_type>
    using lighting_function = std::function<color_type(const point3<T>& p, const vector3<T>& n)>;

    template <typename T, typename color_type>
    using color_function = std::function<color_type(T x, T y)>;

    template <typename T, typename color_type>
    using background_function = std::function<color_type(T x, T y, const unit_line3<T>& l)>;

    template <typename T>
    using progress_function = std::function<void(T percentage)>;

    template <typename T, typename color_type = rgbcolor<T>>
    raster<color_type> render(
        size_t width,
        size_t height,
        color_function<T, color_type> color,
        size_t samples_per_pixel = 1,
        sample_generator_2d_ptr<T> sampler = std::make_shared<regular_sample_2d<T>>(),
        progress_function<T> progress = nullptr
    )
    {
        raster<color_type> result(width, height);

        constexpr color_type black = { 0, 0, 0 };

        uint64_t last_percentage_x10 = -1;
        uint64_t total_pixels = width * height;

        for (size_t y = 0; y < height; ++y)
        {
            for (size_t x = 0; x < width; ++x)
            {
                uint64_t current_percentage_x10 = 1000 * (y * width + x) / total_pixels;
                if (current_percentage_x10 != last_percentage_x10 && progress)
                {
                    progress(current_percentage_x10 / 10.0);
                }
                last_percentage_x10 = current_percentage_x10;

                color_type current_color = black;
                std::vector<coord2<T>> samples = sampler->get_samples(samples_per_pixel);

                for (const auto& sample : samples)
                {
                    T a = x + sample.x();
                    T b = y + sample.y();

                    current_color += color(a, b);
                }

                result(x, y) = current_color / T(samples.size());
            }
        }
        return result;
    }

    namespace impl
    {
        template <typename T, typename color_type>
        bool get_local_color(const intersection_info<T, color_type>& intersection, const texture_ptr<T,color_type>& tex, color_type& color)
        {
            coord2<T> uv{};
            if (intersection.have_uv())
            {
                uv = intersection.get_uv();
            }

            vector3<T> normal{};
            if (intersection.have_normal())
            {
                normal = intersection.get_normal();
            }

            if (!tex->get_color(intersection.get_first_point(), uv, normal, color))
            {
                color = colors<color_type>::black;
                return false;
            }
            return true;
        }
    }

    template <typename T, typename color_type = rgbcolor<T>>
    color_type sample_scene(
        T x, T y,
        const ray_parameters<T,color_type>& ray,
        const shape_ptr<T, color_type>& scene,
        const texture_ptr<T, color_type> scene_texture,
        const intersection_requirements& requirements,
        const lighting_function<T, color_type>& brightness,
        const background_function<T, color_type>& background)
    {
        intersection_info<T,color_type> intersection;
        if (scene->intersects_ray(ray, intersection, requirements))
        {
            color_type light = brightness(intersection.get_first_point(), intersection.get_normal());

            auto tex = intersection.get_shape()->texture();
            if (!tex)
            {
                tex = scene_texture;
            }

            bool scattered = false;

            color_type local_color;
            bool local = impl::get_local_color(intersection, tex, local_color);

            color_type scattered_color = colors<color_type>::black;
            ray_parameters<T,color_type> scattered_ray;
            color_type attenuation;
            if (tex->scatter_ray(ray, intersection, scattered_ray, attenuation))
            {
                scattered_ray.set_contribution(attenuation * scattered_ray.get_contribution());
                // Only send another ray if the contribution large enough to do something.
                if (scattered_ray.get_scalar_contribution() > AMETHYST_EPSILON)
                {
                    scattered_color = attenuation * sample_scene(x, y, scattered_ray, scene, scene_texture, requirements, brightness, background);
                    scattered = true;
                }
            }

            if (!scattered && !local)
            {
                // A potential point to hit a debugger.  Could mean total internal reflection, border case, etc. that will be fixed by supersampling.

                // std::cout << "scattered nothing, refracted nothing, local texture has no value." << std::endl;
            }

            return clamp_visible(light * local_color + scattered_color);
        }
        else
        {
            return background(x, y, ray.get_line());
        }
    }

    // This will change a whole bunch as we progress, but there is way too much
    // duplicated code in the samples that are being written.
    template <typename T, typename color_type = rgbcolor<T>>
    raster<color_type> render(
        camera_ptr<T, color_type> camera,
        shape_ptr<T, color_type> scene,
        texture_ptr<T, color_type> scene_texture, // Not sufficient for general use.
        size_t width,
        size_t height,
        intersection_requirements requirements,
        lighting_function<T, color_type> brightness = [](const point3<T>&, const vector3<T>&) { return color_type{ 1,1,1 }},
        background_function<T, color_type> background = nullptr,
        size_t samples_per_pixel = 1,
        sample_generator_2d_ptr<T> sampler = std::make_shared<regular_sample_2d<T>>(),
        progress_function<T> progress = nullptr
    )
    {
        // If no background function was give, apply a gradient.
        if (background == nullptr)
        {
            background = [=](T x, T y, const unit_line3<T>& l)
            {
                T t = 0.5 * (l.direction().y() + 1);
                return (1.0 - t) * color_type(1.0, 1.0, 1.0) + t * color_type(0.5, 0.7, 1.0);
            };
        }

        auto color = [&](T x, T y)
        {
            ray_parameters<T,color_type> r = camera->get_ray(x, y);
            return sample_scene(x, y, r, scene, scene_texture, requirements, brightness, background);
        };

        return render<T, color_type>(width, height, color, samples_per_pixel, sampler, progress);
    }
}