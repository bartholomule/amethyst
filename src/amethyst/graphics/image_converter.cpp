#include <graphics/image_converter.hpp>


#include <iostream>

namespace amethyst
{
    // ---------------------------------------------------------------------------
    // doubles

    template <>
    rgbcolor<double>
    convert_color<double, unsigned char>(const rgbcolor<unsigned char>& color)
    {
        const double conversion_factor = 1.0 / double(uint8_t(-1));

        // Just perform the conversion (no clipping).
        return rgbcolor<double>(color.r() * conversion_factor,
                                color.g() * conversion_factor,
                                color.b() * conversion_factor);
    }

    template <>
    rgbcolor<unsigned char>
    convert_color<unsigned char, double>(const rgbcolor<double>& color)
    {
        const double cf = uint8_t(-1);

        // Convert/clip it.
        return rgbcolor<unsigned char>(
            uint8_t(my_min<double>(color.r(), 1.0) * cf),
            uint8_t(my_min<double>(color.g(), 1.0) * cf),
            uint8_t(my_min<double>(color.b(), 1.0) * cf));
    }

    template <>
    rgbcolor<uint16_t> convert_color<uint16_t, double>(const rgbcolor<double>& color)
    {
        const double cf = (uint16_t)(-1);

        // Convert/clip it.
        return rgbcolor<uint16_t>(
            uint16_t(my_min<double>(color.r(), 1.0) * cf),
            uint16_t(my_min<double>(color.g(), 1.0) * cf),
            uint16_t(my_min<double>(color.b(), 1.0) * cf));
    }

    template <>
    rgbcolor<double> convert_color<double, uint16_t>(const rgbcolor<uint16_t>& color)
    {
        const double conversion_factor = 1.0 / double(uint16_t(-1));

        return rgbcolor<double>(color.r() * conversion_factor,
                                color.g() * conversion_factor,
                                color.b() * conversion_factor);
    }


    template <>
    rgbcolor<double>
    gamma_convert_color<double, double>(const rgbcolor<double>& color, double gamma)
    {
        double foo = 1 / gamma;
        return rgbcolor<double>(std::pow(color.r(), foo),
                                std::pow(color.g(), foo),
                                std::pow(color.b(), foo));
    }


    // ---------------------------------------------------------------------------
    // Floats

    template <>
    rgbcolor<float>
    convert_color<float, unsigned char>(const rgbcolor<unsigned char>& color)
    {
        const float conversion_factor = 1.0f / float(std::numeric_limits<uint8_t>::max());

        // Just perform the conversion (no clipping).
        return rgbcolor<float>(color.r() * conversion_factor,
                               color.g() * conversion_factor,
                               color.b() * conversion_factor);
    }

    template <>
    rgbcolor<unsigned char>
    convert_color<unsigned char, float>(const rgbcolor<float>& color)
    {
        const unsigned char cf = uint8_t(-1);

        // Convert/clip it.
        return rgbcolor<unsigned char>(uint8_t(my_min<float>(color.r(), 1.0) * cf),
                                       uint8_t(my_min<float>(color.g(), 1.0) * cf),
                                       uint8_t(my_min<float>(color.b(), 1.0) * cf));
    }

    template <>
    rgbcolor<float>
    gamma_convert_color<float, float>(const rgbcolor<float>& color, double gamma)
    {
        float foo = float(1 / gamma);
        return rgbcolor<float>(std::pow(color.r(), foo),
                               std::pow(color.g(), foo),
                               std::pow(color.b(), foo));
    }

    template <>
    rgbcolor<uint16_t> convert_color<uint16_t, uint8_t>(const rgbcolor<uint8_t>& color)
    {
        return { uint16_t(color.r() << 8), uint16_t(color.g() << 8), uint16_t(color.b() << 8) };
    }

    template <>
    rgbcolor<uint8_t> convert_color<uint8_t, uint16_t>(const rgbcolor<uint16_t>& color)
    {
        return { uint8_t(color.r() >> 8), uint8_t(color.g() >> 8), uint8_t(color.b() >> 8) };
    }
} // namespace amethyst
