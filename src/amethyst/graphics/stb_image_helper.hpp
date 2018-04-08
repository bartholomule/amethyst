#pragma once

#include "raster.hpp"
#include "rgbcolor.hpp"
#include <string>

namespace amethyst
{
    raster<rgbcolor<uint8_t>> load_image_with_stb(const std::string& filename);
}