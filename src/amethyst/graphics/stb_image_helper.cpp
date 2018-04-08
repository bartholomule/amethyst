#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "stb_image_helper.hpp"
#include <memory>

namespace amethyst
{
    raster<rgbcolor<uint8_t>> load_image_with_stb(const std::string& filename)
    {
        int width, height, bytes_per_pixel;
        std::shared_ptr<unsigned char> data(stbi_load(filename.c_str(), &width, &height, &bytes_per_pixel, STBI_rgb), stbi_image_free);
        rgbcolor<uint8_t>* reinterpreted = reinterpret_cast<rgbcolor<uint8_t>*>(data.get());
        raster<rgbcolor<uint8_t>> result(width, height);
        for (size_t j = 0; j < size_t(height); ++j)
        {
            for (size_t i = 0; i < size_t(width); ++i)
            {
                result(i, j) = reinterpreted[j * width + i];
            }
        }

        return result;
    }
}