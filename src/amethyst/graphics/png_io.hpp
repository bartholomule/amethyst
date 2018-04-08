#pragma once

#include "graphics/image_io.hpp"
#include "general/string_format.hpp"
#include "graphics/stb_image_helper.hpp"
#include <iostream>
#include <fstream>
#include <functional>

namespace amethyst
{
    bool write_png(const std::string& filename, const raster<rgbcolor<uint8_t>>& data);
    bool write_png(const std::string& filename, const raster<rgbcolor<double>>& data);
    bool write_png(const std::string& filename, const raster<rgbcolor<float>>& data);

    bool write_png(std::streambuf& output, const raster<rgbcolor<uint8_t>>& data);
    bool write_png(std::streambuf& output, const raster<rgbcolor<double>>& data);
    bool write_png(std::streambuf& output, const raster<rgbcolor<float>>& data);

    template <typename T, typename ColorType = rgbcolor<T>>
    class png_io : public image_io<T, ColorType>
    {
    public:
        using Parent = image_io<T, ColorType>;

        png_io() = default;
        ~png_io() = default;

        using Parent::output;
        using Parent::input;

        std::string default_extension() const override { return "png"; }

        bool output(std::ostream& o, const raster<ColorType>& source) const override
        {
            return write_png(*o.rdbuf(), source);
        }
        bool output(std::streambuf& stream, const raster<ColorType>& source) const override
        {
            return write_png(stream, source);
        }

        raster<ColorType> input(std::istream& i) const override
        {
            // FIXME! callbacks and such.  This function isn't really needed though, because the other input is overridden.
            return {};
        }

        raster<ColorType> input(const std::string& filename) const override
        {
            return convert_image<ColorType::number_type>(load_image_with_stb(filename));
        }
    };
}
