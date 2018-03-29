#define auto_unit_test_main main
#include "test_framework/unit_test_auto.hpp"
#include <cmath>
#include <iostream>

#include "amethyst/graphics/alpha_triangle_2d.hpp"
#include "amethyst/graphics/image.hpp"
#include "amethyst/graphics/tga_io.hpp"

template <class T>
void clear_image(amethyst::image<T>& img, const amethyst::rgbcolor<T>& color)
{
    for (size_t y = 0; y < img.get_height(); ++y)
    {
        for (size_t x = 0; x < img.get_width(); ++x)
        {
            img(x, y) = color;
        }
    }
}

template <class T>
bool colors_equal(const amethyst::rgbcolor<T>& c1, const amethyst::rgbcolor<T>& c2)
{
    amethyst::rgbcolor<T> diff = c1 - c2;
    const T epsilon = 0.02;

    std::cout << "Comparing colors: " << c1 << " and " << c2 << std::endl;

    return (fabs(diff.r()) < epsilon) && (fabs(diff.g()) < epsilon) && (fabs(diff.b()) < epsilon);
}

AUTO_UNIT_TEST(test_opaque1)
{
    typedef amethyst::image<double> image;
    typedef amethyst::alpha_triangle_2d<double> triangle;
    typedef amethyst::rgbcolor<double> color;
    using namespace amethyst;

    image output(100, 100);


    tga_io<double> io;

    color black(0, 0, 0);
    color white(1, 1, 1);
    double transparent = 1;
    double opaque = 0;

    {
        clear_image(output, black);
        triangle t;
        // |\     .
        // | >
        // |/
        // This ordering ensures sorting is required.
        t.v1.xy.set(0, 0);
        t.v2.xy.set(0, 99);
        t.v3.xy.set(99, 50);
        t.v1.rgb = t.v2.rgb = t.v3.rgb = white;
        t.v1.a = t.v2.a = t.v3.a = opaque;

        dda_rasterize_triangle(output, t);

        io.output("test_triangle_opaque1-1.tga", output);

        TEST_BOOLEAN(colors_equal(output(0, 0), white));
        TEST_BOOLEAN(colors_equal(output(0, 99), white));
        TEST_BOOLEAN(colors_equal(output(50, 50), white));
        TEST_BOOLEAN(colors_equal(output(99, 0), black));
        TEST_BOOLEAN(colors_equal(output(99, 49), black));
        TEST_BOOLEAN(colors_equal(output(99, 50), white));
        TEST_BOOLEAN(colors_equal(output(99, 51), black));
        TEST_BOOLEAN(colors_equal(output(99, 99), black));
    }
    {
        clear_image(output, black);
        triangle t;
        //  /|
        // < |
        //  \|
        t.v1.xy.set(99, 0);
        t.v2.xy.set(99, 99);
        t.v3.xy.set(0, 50);
        t.v1.rgb = t.v2.rgb = t.v3.rgb = white;
        t.v1.a = t.v2.a = t.v3.a = opaque;

        dda_rasterize_triangle(output, t);

        io.output("test_triangle_opaque1-2.tga", output);

        TEST_BOOLEAN(colors_equal(output(0, 0), black));
        TEST_BOOLEAN(colors_equal(output(0, 49), black));
        TEST_BOOLEAN(colors_equal(output(0, 50), white));
        TEST_BOOLEAN(colors_equal(output(0, 51), black));
        TEST_BOOLEAN(colors_equal(output(0, 99), black));
        TEST_BOOLEAN(colors_equal(output(50, 50), white));
        TEST_BOOLEAN(colors_equal(output(98, 1), white));
        TEST_BOOLEAN(colors_equal(output(98, 98), white));
        TEST_BOOLEAN(colors_equal(output(99, 0), white));
        TEST_BOOLEAN(colors_equal(output(99, 99), white));
    }
    {
        clear_image(output, black);
        triangle t;
        // ---
        // \ /
        //  V
        t.v1.xy.set(0, 0);
        t.v2.xy.set(50, 99);
        t.v3.xy.set(99, 0);
        t.v1.rgb = t.v2.rgb = t.v3.rgb = white;
        t.v1.a = t.v2.a = t.v3.a = opaque;

        dda_rasterize_triangle(output, t);

        io.output("test_triangle_opaque1-3.tga", output);

        TEST_BOOLEAN(colors_equal(output(0, 0), white));
        TEST_BOOLEAN(colors_equal(output(0, 99), black));
        TEST_BOOLEAN(colors_equal(output(50, 50), white));
        TEST_BOOLEAN(colors_equal(output(49, 99), black));
        TEST_BOOLEAN(colors_equal(output(50, 99), white));
        TEST_BOOLEAN(colors_equal(output(51, 99), black));
        TEST_BOOLEAN(colors_equal(output(99, 99), black));
        TEST_BOOLEAN(colors_equal(output(99, 0), white));
    }
    {
        clear_image(output, black);
        triangle t;
        //  ^
        // / \ .
        // ---
        t.v1.xy.set(0, 99);
        t.v2.xy.set(50, 0);
        t.v3.xy.set(99, 99);
        t.v1.rgb = t.v2.rgb = t.v3.rgb = white;
        t.v1.a = t.v2.a = t.v3.a = opaque;

        dda_rasterize_triangle(output, t);

        io.output("test_triangle_opaque1-4.tga", output);

        TEST_BOOLEAN(colors_equal(output(0, 0), black));
        TEST_BOOLEAN(colors_equal(output(0, 99), white));
        TEST_BOOLEAN(colors_equal(output(50, 50), white));
        TEST_BOOLEAN(colors_equal(output(49, 0), black));
        TEST_BOOLEAN(colors_equal(output(50, 0), white));
        TEST_BOOLEAN(colors_equal(output(51, 0), black));
        TEST_BOOLEAN(colors_equal(output(99, 0), black));
        TEST_BOOLEAN(colors_equal(output(99, 99), white));
    }

    {
        clear_image(output, black);
        triangle t;
        // |\  .
        // | \ .
        // ---
        t.v1.xy.set(0, 99);
        t.v2.xy.set(0, 0);
        t.v3.xy.set(99, 99);
        t.v1.rgb = t.v2.rgb = t.v3.rgb = white;
        t.v1.a = t.v2.a = t.v3.a = opaque;

        dda_rasterize_triangle(output, t);

        io.output("test_triangle_opaque1-5.tga", output);

        TEST_BOOLEAN(colors_equal(output(0, 0), white));
        TEST_BOOLEAN(colors_equal(output(0, 99), white));
        TEST_BOOLEAN(colors_equal(output(50, 50), white));
        TEST_BOOLEAN(colors_equal(output(1, 0), black));
        TEST_BOOLEAN(colors_equal(output(99, 0), black));
        TEST_BOOLEAN(colors_equal(output(99, 98), black));
        TEST_BOOLEAN(colors_equal(output(99, 99), white));
    }

    {
        clear_image(output, black);
        triangle t;
        //  /|
        // / |
        // ---
        t.v1.xy.set(0, 99);
        t.v2.xy.set(99, 0);
        t.v3.xy.set(99, 99);
        t.v1.rgb = t.v2.rgb = t.v3.rgb = white;
        t.v1.a = t.v2.a = t.v3.a = opaque;

        dda_rasterize_triangle(output, t);

        io.output("test_triangle_opaque1-6.tga", output);

        TEST_BOOLEAN(colors_equal(output(0, 0), black));
        TEST_BOOLEAN(colors_equal(output(0, 98), black));
        TEST_BOOLEAN(colors_equal(output(0, 99), white));
        TEST_BOOLEAN(colors_equal(output(50, 50), white));
        TEST_BOOLEAN(colors_equal(output(98, 0), black));
        TEST_BOOLEAN(colors_equal(output(99, 0), white));
        TEST_BOOLEAN(colors_equal(output(99, 99), white));
    }

    {
        clear_image(output, black);
        triangle t;
        // ---
        // | /
        // |/
        t.v1.xy.set(0, 99);
        t.v2.xy.set(99, 0);
        t.v3.xy.set(0, 0);
        t.v1.rgb = t.v2.rgb = t.v3.rgb = white;
        t.v1.a = t.v2.a = t.v3.a = opaque;

        dda_rasterize_triangle(output, t);

        io.output("test_triangle_opaque1-7.tga", output);

        TEST_BOOLEAN(colors_equal(output(0, 0), white));
        TEST_BOOLEAN(colors_equal(output(0, 99), white));
        TEST_BOOLEAN(colors_equal(output(1, 99), black));
        TEST_BOOLEAN(colors_equal(output(49, 49), white));
        TEST_BOOLEAN(colors_equal(output(99, 0), white));
        TEST_BOOLEAN(colors_equal(output(99, 1), black));
        TEST_BOOLEAN(colors_equal(output(99, 99), black));
    }

    {
        clear_image(output, black);
        triangle t;
        // ---
        // \ |
        //  \|
        t.v1.xy.set(99, 99);
        t.v2.xy.set(99, 0);
        t.v3.xy.set(0, 0);
        t.v1.rgb = t.v2.rgb = t.v3.rgb = white;
        t.v1.a = t.v2.a = t.v3.a = opaque;

        dda_rasterize_triangle(output, t);

        io.output("test_triangle_opaque1-8.tga", output);

        TEST_BOOLEAN(colors_equal(output(0, 0), white));
        TEST_BOOLEAN(colors_equal(output(0, 1), black));
        TEST_BOOLEAN(colors_equal(output(0, 99), black));
        TEST_BOOLEAN(colors_equal(output(50, 50), white));
        TEST_BOOLEAN(colors_equal(output(99, 0), white));
        TEST_BOOLEAN(colors_equal(output(98, 99), black));
        TEST_BOOLEAN(colors_equal(output(99, 99), white));
    }
}


AUTO_UNIT_TEST(test_opaque2)
{
    typedef amethyst::image<double> image;
    typedef amethyst::alpha_triangle_2d<double> triangle;
    typedef amethyst::rgbcolor<double> color;
    using namespace amethyst;

    image output(100, 100);


    tga_io<double> io;

    color black(0, 0, 0);
    color white(1, 1, 1);
    color red(1, 0, 0);
    color green(0, 1, 0);
    color blue(0, 0, 1);
    double transparent = 1;
    double opaque = 0;

    {
        clear_image(output, black);
        triangle t;
        t.v1.xy.set(50, 0);
        t.v2.xy.set(50 + 43.3, 50 + 25);
        t.v3.xy.set(50 - 43.3, 50 + 25);
        t.v1.rgb = red;
        t.v2.rgb = green;
        t.v3.rgb = blue;
        t.v1.a = t.v2.a = t.v3.a = opaque;

        dda_rasterize_triangle(output, t);

        io.output("test_triangle_opaque2-1.tga", output);


        TEST_BOOLEAN(colors_equal(output(
                                      size_t((t.v1.xy + t.v2.xy + t.v3.xy).x() / 3 + 0.5),
                                      size_t((t.v1.xy + t.v2.xy + t.v3.xy).y() / 3 + 0.5)),
                                  (t.v1.rgb + t.v2.rgb + t.v3.rgb) * (1 / 3.0)));

        TEST_BOOLEAN(colors_equal(output(
                                      size_t((t.v1.xy + t.v2.xy).x() / 2 + 0.5),
                                      size_t((t.v1.xy + t.v2.xy).y() / 2 + 0.5)),
                                  (t.v1.rgb + t.v2.rgb) * (1 / 2.0)));

        TEST_BOOLEAN(colors_equal(output(
                                      size_t((t.v1.xy + t.v3.xy).x() / 2 + 0.5),
                                      size_t((t.v1.xy + t.v3.xy).y() / 2 + 0.5)),
                                  (t.v1.rgb + t.v3.rgb) * (1 / 2.0)));

        TEST_BOOLEAN(colors_equal(output(
                                      size_t((t.v2.xy + t.v3.xy).x() / 2 + 0.5),
                                      size_t((t.v2.xy + t.v3.xy).y() / 2 + 0.5)),
                                  (t.v2.rgb + t.v3.rgb) * (1 / 2.0)));
    }
}

