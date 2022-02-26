#pragma once

/*
   tga_io.hpp

   An extension of the image_output class (in imageout.h) to output to a targa file.
 */
/*
   Revision history:
   19jan1999 Created this file.
 */

#include "graphics/image_io.hpp"
#include <iostream>
#include <fstream>

namespace amethyst
{
    template <typename T, typename ColorType = rgbcolor<T>>
    class tga_io : public image_io<T, ColorType>
    {
    public:
        tga_io() : image_io<T>() {
        }
        virtual ~tga_io() {
        }

        using image_io<T>::output;
        using image_io<T>::output_byte;
        using image_io<T>::input;

        std::string default_extension() const override {
            return "tga";
        }

        bool output(std::ostream& o, const raster<ColorType>& source) const override;
        bool output(std::streambuf& stream, const raster<ColorType>& source) const override;

        template <class S> bool basic_output(S& o, const raster<ColorType>& source) const;

        raster<ColorType> input(std::istream& i) const override;

    private:
        using byte = unsigned char;
        using word = uint16_t;
    };

    template <typename T, typename ColorType>
    template <class S>
    bool tga_io<T, ColorType>::basic_output(S& o, const raster<ColorType>& source) const
    {
        size_t x, y;
        static unsigned char main_header[12] = {
            0, // Length of identifier string
            0, // Color map type (0 = no color map)
            2, // Image type (2 = RGB)
            0, // First color map entry (LSB)
            0, //   "     "    "    "   (MSB)
            0, // Entries in color map (LSB)
            0, //   "     "    "    "  (MSB)
            0, // Size of color map entry
            0, // X origin of image (LSB)
            0, // "    "    "   "   (MSB)
            0, // Y origin of image (LSB)
            0  // "    "    "   "   (MSB)
        };
        size_t width = source.get_width();
        size_t height = source.get_height();
        // Write the main header.
        for (size_t j = 0; j < sizeof(main_header) / sizeof(*main_header); ++j)
        {
            output_byte(o, main_header[j]);
        }
        // Write the width and height.
        output_byte(o, width & 0xff);         // width (lsb)
        output_byte(o, (width >> 8) & 0xff);  // width (msb)
        output_byte(o, height & 0xff);        // height (lsb)
        output_byte(o, (height >> 8) & 0xff); // height (msb)
        // Write the end of the header.
        output_byte(o, 24); // Pixel size (24 bits)
        output_byte(o, 32); // Attributes (0 = origin in lower left)

        // Everything else is raw 24-bit data in b,g,r byte order
        for (y = 0; y < height; ++y)
        {
            for (x = 0; x < width; ++x)
            {
                rgbcolor<unsigned char> c = convert_color<unsigned char>(source(x, y));
                //				std::cerr << "color conversion of " << source(x,y) << " returned (" << int(c.r()) <<"," << int(c.g()) <<"," << int(c.b()) << ")" << std::endl;
                output_byte(o, c.b());
                output_byte(o, c.g());
                output_byte(o, c.r());
            }
        }
        // FIXME! Use proper state checking...
        return true;
    }

    /*
       bool tga_io<T,ColorType>::output(std::streambuf& stream, raster<ColorType> const& source) const

       Write a targa file to a stream buffer.

       ostream, istream, etc, are not really meant for binary input and
       output. They are meant for *formatted* i/o - that is, i/o such
       that standard library formattiing is right.  tga_io, however, does
       its own formatting. So I decided to skip the standard library
       formatting, and write a function that would use streambuffers
       directly.

       This function can output the 500x500 (750K) image made by figure_3_7.cpp
       nearly 3x as fast as the nearly identical function using std::ostream.
       Additionly, I think this function avoids locale transformations.
     */
    template <typename T, typename ColorType>
    bool tga_io<T, ColorType>::output(std::streambuf& stream, raster<ColorType> const& source) const
    {
        return basic_output(stream, source);
    }

    template <typename T, typename ColorType>
    bool tga_io<T, ColorType>::output(std::ostream& o, const raster<ColorType>& source) const
    {
        return basic_output(o, source);
    }

    template <typename T, typename ColorType>
    raster<ColorType> tga_io<T, ColorType>::input(std::istream& i) const
    {
        byte version;
        byte b1, b2;
        byte r, g, b;
        int width, height;

        i.ignore(2);
        i.get((char&)version);
        if (version != 2)
        {
            std::cout << "Incorrect version (" << int(version) << ")" << std::endl;
            return {};
        }
        else
        {
            std::cout << "Version is " << (int)version << std::endl;
        }
        i.ignore(9);

        i.get((char&)b1).get((char&)b2);
        width = word(b1) + (word(b2) << 8);

        i.get((char&)b1).get((char&)b2);
        height = word(b1) + (word(b2) << 8);

        i.get((char&)b1).get((char&)b2);
        if ((b1 != 24) || ((b2 != 32) && (b2 != 0)))
        {
            std::cout << "Invalid constants..." << std::endl;
            return {};
        }
        bool flip_y = false;
        if (b2 == 0)
        {
            flip_y = true;
        }

        raster<ColorType> dest { size_t(width), size_t(height) };

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                i.get((char&)b).get((char&)g).get((char&)r);

                int y_pos = y;
                if (flip_y)
                {
                    y_pos = (height - 1) - y;
                }

                dest(x, y_pos) = convert_color<T>(rgbcolor<uint8_t>(r, g, b));
            }
        }
        std::cout << "Loaded image of size " << width << "x" << height << std::endl;

        return dest;
    }
}
