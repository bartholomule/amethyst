#include "png_io.hpp"
#include "image_converter.hpp"
#include "stb_image.h"
#include "stb_image_write.h"

namespace amethyst
{
    bool write_png(const std::string& filename, const raster<rgbcolor<uint8_t>>& data)
    {
        return stbi_write_png(filename.c_str(), int(data.get_width()), int(data.get_height()), 3, data.reinterpret<const unsigned char*>(), 0);
    }

    bool write_png(const std::string& filename, const raster<rgbcolor<double>>& data)
    {
        return write_png(filename, convert_image<unsigned char>(data));
    }

    bool write_png(const std::string& filename, const raster<rgbcolor<float>>& data)
    {
        return write_png(filename, convert_image<unsigned char>(data));
    }

    namespace
    {
        void write_to_streambuf(void* context, void* data, int size)
        {
            std::streambuf* sb = reinterpret_cast<std::streambuf*>(context);
            sb->sputn((const char*)data, size);
        }
    }

    bool write_png(std::streambuf& output, const raster<rgbcolor<uint8_t>>& data)
    {
        return stbi_write_png_to_func(write_to_streambuf, &output, int(data.get_width()), int(data.get_height()), 3, data.reinterpret<unsigned char*>(), 0);
    }
    bool write_png(std::streambuf& output, const raster<rgbcolor<double>>& data)
    {
        return write_png(output, convert_image<uint8_t>(data));
    }
    bool write_png(std::streambuf& output, const raster<rgbcolor<float>>& data)
    {
        return write_png(output, convert_image<uint8_t>(data));
    }
}