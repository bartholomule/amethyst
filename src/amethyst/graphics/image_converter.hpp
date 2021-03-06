#pragma once
#include "amethyst/graphics/image.hpp"

namespace amethyst
{
    /**
     * Convert from one color type to another.  This function must be specialized
     * for all types where U != T (where any conversion actually occurs).
     */
    template <class U, class T>
    rgbcolor<U> convert_color(const rgbcolor<T>& color)
    {
        // This return will cause a compile-time error if they aren't the same
        // type.  If you see an error here, then this templated function has not
        // been specialized for your particular type.  See below for pre-existing
        // specializations.
        return color;
    }

    /**
     * Convert from one color type to another, performing gamma conversion.  The
     * default version of this function is VERY slow, so specializations would be
     * required for any kind of decent speed.
     */
    template <class U, class T>
    rgbcolor<U> gamma_convert_color(const rgbcolor<T>& color, double gamma)
    {
        rgbcolor<double> temp = convert_color<double>(color);
        temp.set(std::pow(temp.r(), 1 / gamma),
                 std::pow(temp.g(), 1 / gamma),
                 std::pow(temp.b(), 1 / gamma));
        return convert_color<U>(temp);
    }

    /**
     * Generic conversion from one type of image to another.
     */
    template <class U, class T>
    raster<rgbcolor<U>> convert_image(const raster<rgbcolor<T>>& img)
    {
        raster<rgbcolor<U>> result(img.get_width(), img.get_height());

        for (unsigned y = 0; y < result.get_height(); ++y)
        {
            for (unsigned x = 0; x < result.get_width(); ++x)
            {
                result(x, y) = convert_color<U>(img(x, y));
            }
        }
        return result;
    }

    /**
     * Generic conversion from one type of image to another, using a gamma
     * conversion.
     */
    template <class U, class T>
    image<U> convert_image(const image<T>& img, double gamma)
    {
        image<U> result(img.get_width(), img.get_height());

        for (unsigned y = 0; y < result.get_height(); ++y)
        {
            for (unsigned x = 0; x < result.get_width(); ++x)
            {
                result(x, y) = gamma_convert_color<U>(img(x, y), gamma);
            }
        }
        return result;
    }


    // byte [0,255] --> double [0,1.0)
    template <>
    rgbcolor<double>
    convert_color<double, unsigned char>(const rgbcolor<unsigned char>& color);

    // double [0,1.0) --> byte [0,255]
    template <>
    rgbcolor<uint8_t> convert_color<uint8_t, double>(const rgbcolor<double>& color);

    template <>
    rgbcolor<uint16_t> convert_color<uint16_t, double>(const rgbcolor<double>& color);

    template <>
    rgbcolor<double> convert_color<double, uint16_t>(const rgbcolor<uint16_t>& color);

    // double --> double (gamma corrected)
    template <>
    rgbcolor<double>
    gamma_convert_color<double, double>(const rgbcolor<double>& color, double gamma);

    // byte [0,255] --> float [0,1.0)
    template <>
    rgbcolor<float>
    convert_color<float, unsigned char>(const rgbcolor<unsigned char>& color);

    // float [0,1.0) --> byte [0,255]
    template <>
    rgbcolor<unsigned char>
    convert_color<unsigned char, float>(const rgbcolor<float>& color);

    // float --> float (gamma corrected)
    template <>
    rgbcolor<float>
    gamma_convert_color<float, float>(const rgbcolor<float>& color, double gamma);


    // 8 <--> 16 bit
    template <>
    rgbcolor<uint16_t> convert_color<uint16_t, uint8_t>(const rgbcolor<uint8_t>& color);
    template <>
    rgbcolor<uint8_t> convert_color<uint8_t, uint16_t>(const rgbcolor<uint16_t>& color);

}
