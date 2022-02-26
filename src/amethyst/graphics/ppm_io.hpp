#pragma once

#include "graphics/image_io.hpp"
#include "general/string_format.hpp"
#include <iostream>
#include <fstream>
#include <functional>

namespace amethyst
{
    template <typename T, typename ColorType = rgbcolor<T>>
    class ppm_io : public image_io<T, ColorType>
    {
    public:
        using parent = image_io<T, ColorType>;

        ppm_io() = default;
        ~ppm_io() = default;

        std::string default_extension() const override {
            return "ppm";
        }

        bool output(std::ostream& o, const raster<ColorType>& source) const override;
        bool output(std::streambuf& stream, const raster<ColorType>& source) const override;

        template <class S> bool basic_output(S& o, const raster<ColorType>& source) const;

        void eat_whitespace_and_comments(std::istream& input) const;
        raster<ColorType> input(std::istream& i) const override;
    };

// FIXME! P6 16-bit has a bug in either output (doesn't seem likely because they load
// properly in gimp) or the input.  Values are scaled way too high, and in the middle
// of an image, pixel values get shifted slightly.
#define USE_PPM_P6
#undef USE_PPM_16BIT

#if defined(USE_PPM_P6)
#if !defined(USE_PPM_16BIT)
    template <typename T, typename ColorType>
    template <typename S>
    bool ppm_io<T, ColorType>::basic_output(S& o, const raster<ColorType>& source) const
    {
        std::string header = string_format("P6 %1 %2 255\n", int(source.get_width()), int(source.get_height()));

        parent::output_string(o, header);

        for (size_t y = 0; y < source.get_height(); ++y)
        {
            for (size_t x = 0; x < source.get_width(); ++x)
            {
                auto c = convert_color<uint8_t>(source(x, y));
                parent::output_byte(o, c.r());
                parent::output_byte(o, c.g());
                parent::output_byte(o, c.b());
            }
        }
        return true;
    }
#else // #if defined(USE_PPM_16BIT)
    template <typename T, typename ColorType>
    template <typename S>
    bool ppm_io<T, ColorType>::basic_output(S& o, const raster<ColorType>& source) const
    {
        std::string header = string_format(
            "P6\n"    // binary format
            "%1 %2\n" // width, height
            "65535\n" // using 16-bit output
            , int(source.get_width()), int(source.get_height())
            );

        output_string(o, header);

        for (size_t y = 0; y < source.get_height(); ++y)
        {
            for (size_t x = 0; x < source.get_width(); ++x)
            {
                auto c = convert_color<uint16_t>(source(x, y));
                // std::cout << "source(" << x << "," << y << "): " << inspect(source(x, y)) << " c=" << inspect(c) << std::endl;
                output_word_be(o, c.r());
                output_word_be(o, c.g());
                output_word_be(o, c.b());
            }
        }
        return true;
    }
#endif // #if !defined(USE_PPM_16BIT)
#else // !defined(USE_PPM_P6)
    template <typename T, typename ColorType>
    template <typename S>
    bool ppm_io<T, ColorType>::basic_output(S& o, const raster<ColorType>& source) const
    {
        std::string header = string_format(
            "P3\n"    // binary format
            "%1 %2\n" // width, height
            "255\n" // using 8-bit output
            , int(source.get_width()), int(source.get_height())
            );

        write_string(o, header);

        for (size_t y = 0; y < source.get_height(); ++y)
        {
            for (size_t x = 0; x < source.get_width(); ++x)
            {
                if (x != 0)
                {
                    output_byte(o, ' ');
                }

                auto c = convert_color<uint8_t>(source(x, y));

                auto ctext = string_format("%1 %2 %3", unsigned(c.r()), unsigned(c.g()), unsigned(c.b()));
                write_string(o, ctext);
            }
            output_byte(o, '\n');
        }
        return true;
    }
#endif


    template <typename T, typename ColorType>
    bool ppm_io<T, ColorType>::output(std::ostream& o, const raster<ColorType>& source) const
    {
        return basic_output(o, source);
    }

    template <typename T, typename ColorType>
    bool ppm_io<T, ColorType>::output(std::streambuf& stream, const raster<ColorType>& source) const
    {
        return basic_output(stream, source);
    }


    template <typename T, typename ColorType>
    void ppm_io<T, ColorType>::eat_whitespace_and_comments(std::istream& input) const
    {
        bool in_comment = false;

        for (;;)
        {
            char c;
            if (!input.get(c))
            {
                break;
            }

            if (c == '#')
            {
                in_comment = true;
            }

            if (in_comment)
            {
                if (c == '\n')
                {
                    in_comment = false;
                }
            }
            else if (!isspace(c))
            {
                input.unget();
                break;
            }
        }
    }

    template <typename T, typename ColorType>
    raster<ColorType> ppm_io<T, ColorType>::input(std::istream& input) const
    {
        std::string format;
        size_t width;
        size_t height;
        size_t max_pixel_value;

        eat_whitespace_and_comments(input);
        input >> format;
        eat_whitespace_and_comments(input);
        input >> width;
        eat_whitespace_and_comments(input);
        input >> height;
        eat_whitespace_and_comments(input);
        input >> max_pixel_value;
        eat_whitespace_and_comments(input);

        std::cout << "Format is " << format << "  Width=" << width << " Height=" << height << " max pixel value=" << max_pixel_value << std::endl;

        raster<ColorType> result{ width, height };

        std::function<bool(int& r, int& g, int& b)> read_rgb;

        auto rgb_p3 = [&input](int& r, int& g, int& b)
        {
            input >> r >> g >> b;
            if (!input) {
                std::cerr << "Failed reading p3" << std::endl;
            }
            return bool(input);
        };
        auto rgb_p6_byte = [&input, this](int& r, int& g, int& b) -> bool
        {
            char rv, gv, bv;
            if (parent::input_byte(input, rv) && parent::input_byte(input, gv) && parent::input_byte(input, bv))
            {
                r = int(rv) & 0xff;
                g = int(gv) & 0xff;
                b = int(bv) & 0xff;
                return true;
            }
            return false;
        };
        auto rgb_p6_word = [&input, this](int& r, int& g, int& b) -> bool
        {
            int16_t rv, gv, bv;
            if (parent::input_word_be(input, rv) && parent::input_word_be(input, gv) && parent::input_word_be(input, bv))
            {
                r = rv;
                g = gv;
                b = bv;
                return true;
            }
            return false;
        };

        if (format == "P3")
        {
            read_rgb = rgb_p3;
        }
        else if (format == "P6" && (max_pixel_value > 255))
        {
            read_rgb = rgb_p6_word;
        }
        else if (format == "P6")
        {
            read_rgb = rgb_p6_byte;
        }

        ColorType* data = result.template reinterpret<ColorType*>();

        for (size_t y = 0; y < height; ++y)
        {
            ColorType* line = data + y * width;

            for (size_t x = 0; x < width; ++x)
            {
                int r, g, b;
                if (!read_rgb(r, g, b))
                {
                    throw std::runtime_error(string_format("failed reading pixel value for [%1,%2]", x, y));
                }
                if (max_pixel_value > 255) {
                    rgbcolor<uint16_t> source(r, g, b);
                    line[x] = convert_color<typename ColorType::number_type>(source);
                    //std::cout << string_format("pixel(%1,%2)=%3  (from word source %4)", x, y, inspect(line[x]), inspect(source)) << std::endl;
                }
                else
                {
                    rgbcolor<uint8_t> source(r, g, b);
                    line[x] = convert_color<typename ColorType::number_type>(source);
                    //std::cout << string_format("pixel(%1,%2)=%3  (from byte source %4)", x, y, inspect(line[x]), inspect(source)) << std::endl;
                }
            }
        }
        return result;
    }
}