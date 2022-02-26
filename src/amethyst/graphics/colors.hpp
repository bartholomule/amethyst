#pragma once

#include "rgbcolor.hpp"
#include "image_converter.hpp"

namespace amethyst
{
    template <typename color_type>
    struct colors
    {
        static constexpr color_type html_rgb(int r, int g, int b)
        {
            static_assert(!std::is_integral<color_type>::value, "You are misusing this colors structure");
            static_assert(!std::is_floating_point<color_type>::value, "You are misusing this colors structure");

            if constexpr(std::is_same<color_type, rgbcolor<double>>::value || std::is_same<color_type, rgbcolor<float>>::value)
            {
                return { r / 255.0, g / 255.0, b / 255.0 };
            }
            else if constexpr(std::is_same<color_type, rgbcolor<uint8_t>>::value)
            {
                return { uint8_t(r), uint8_t(g), uint8_t(b) };
            }
            else
            {
                return convert_color<typename color_type::number_type>(rgbcolor<double>(r / 255.0, g / 255.0, b / 255.0));
            }
        }
        static constexpr color_type html_rgb(int c) { return html_rgb((c & 0xff0000) >> 16, (c & 0x00ff00) >> 8, c & 0x0000ff); }

        // Not a real HTML color name, but something I want.
        static constexpr color_type superbright = html_rgb(250, 250, 250);

        // red html color names
        static constexpr color_type indianred = html_rgb(0xcd5c5c);
        static constexpr color_type lightcoral = html_rgb(0xf08080);
        static constexpr color_type salmon = html_rgb(0xfa8072);
        static constexpr color_type darksalmon = html_rgb(0xe9967a);
        static constexpr color_type lightsalmon = html_rgb(0xffa07a);
        static constexpr color_type crimson = html_rgb(0xdc143c);
        static constexpr color_type red = html_rgb(0xff0000);
        static constexpr color_type firebrick = html_rgb(0xb22222);
        static constexpr color_type darkred = html_rgb(0x8b0000);
        // pink html color names
        static constexpr color_type pink = html_rgb(0xffc0cb);
        static constexpr color_type lightpink = html_rgb(0xffb6c1);
        static constexpr color_type hotpink = html_rgb(0xff69b4);
        static constexpr color_type deeppink = html_rgb(0xff1493);
        static constexpr color_type mediumvioletred = html_rgb(0xc71585);
        static constexpr color_type palevioletred = html_rgb(0xdb7093);
        // orange html color names
        static constexpr color_type coral = html_rgb(0xff7f50);
        static constexpr color_type tomato = html_rgb(0xff6347);
        static constexpr color_type orangered = html_rgb(0xff4500);
        static constexpr color_type darkorange = html_rgb(0xff8c00);
        static constexpr color_type orange = html_rgb(0xffa500);
        // yellow html color names
        static constexpr color_type gold = html_rgb(0xffd700);
        static constexpr color_type yellow = html_rgb(0xffff00);
        static constexpr color_type lightyellow = html_rgb(0xffffe0);
        static constexpr color_type lemonchiffon = html_rgb(0xfffacd);
        static constexpr color_type lightgoldenrodyellow = html_rgb(0xfafad2);
        static constexpr color_type papayawhip = html_rgb(0xffefd5);
        static constexpr color_type moccasin = html_rgb(0xffe4b5);
        static constexpr color_type peachpuff = html_rgb(0xffdab9);
        static constexpr color_type palegoldenrod = html_rgb(0xeee8aa);
        static constexpr color_type khaki = html_rgb(0xf0e68c);
        static constexpr color_type darkkhaki = html_rgb(0xbdb76b);
        // purple html color names
        static constexpr color_type lavender = html_rgb(0xe6e6fa);
        static constexpr color_type thistle = html_rgb(0xd8bfd8);
        static constexpr color_type plum = html_rgb(0xdda0dd);
        static constexpr color_type violet = html_rgb(0xee82ee);
        static constexpr color_type orchid = html_rgb(0xda70d6);
        static constexpr color_type fuchsia = html_rgb(0xff00ff);
        static constexpr color_type magenta = html_rgb(0xff00ff);
        static constexpr color_type mediumorchid = html_rgb(0xba55d3);
        static constexpr color_type mediumpurple = html_rgb(0x9370db);
        static constexpr color_type rebeccapurple = html_rgb(0x663399);
        static constexpr color_type blueviolet = html_rgb(0x8a2be2);
        static constexpr color_type darkviolet = html_rgb(0x9400d3);
        static constexpr color_type darkorchid = html_rgb(0x9932cc);
        static constexpr color_type darkmagenta = html_rgb(0x8b008b);
        static constexpr color_type purple = html_rgb(0x800080);
        static constexpr color_type indigo = html_rgb(0x4b0082);
        static constexpr color_type slateblue = html_rgb(0x6a5acd);
        static constexpr color_type darkslateblue = html_rgb(0x483d8b);
        static constexpr color_type mediumslateblue = html_rgb(0x7b68ee);
        // green html color names
        static constexpr color_type greenyellow = html_rgb(0xadff2f);
        static constexpr color_type chartreuse = html_rgb(0x7fff00);
        static constexpr color_type lawngreen = html_rgb(0x7cfc00);
        static constexpr color_type lime = html_rgb(0x00ff00);
        static constexpr color_type limegreen = html_rgb(0x32cd32);
        static constexpr color_type palegreen = html_rgb(0x98fb98);
        static constexpr color_type lightgreen = html_rgb(0x90ee90);
        static constexpr color_type mediumspringgreen = html_rgb(0x00fa9a);
        static constexpr color_type springgreen = html_rgb(0x00ff7f);
        static constexpr color_type mediumseagreen = html_rgb(0x3cb371);
        static constexpr color_type seagreen = html_rgb(0x2e8b57);
        static constexpr color_type forestgreen = html_rgb(0x228b22);
        static constexpr color_type green = html_rgb(0x008000);
        static constexpr color_type darkgreen = html_rgb(0x006400);
        static constexpr color_type yellowgreen = html_rgb(0x9acd32);
        static constexpr color_type olivedrab = html_rgb(0x6b8e23);
        static constexpr color_type olive = html_rgb(0x808000);
        static constexpr color_type darkolivegreen = html_rgb(0x556b2f);
        static constexpr color_type mediumaquamarine = html_rgb(0x66cdaa);
        static constexpr color_type darkseagreen = html_rgb(0x8fbc8b);
        static constexpr color_type lightseagreen = html_rgb(0x20b2aa);
        static constexpr color_type darkcyan = html_rgb(0x008b8b);
        static constexpr color_type teal = html_rgb(0x008080);
        // blue html color names
        static constexpr color_type aqua = html_rgb(0x00ffff);
        static constexpr color_type cyan = html_rgb(0x00ffff);
        static constexpr color_type lightcyan = html_rgb(0xe0ffff);
        static constexpr color_type paleturquoise = html_rgb(0xafeeee);
        static constexpr color_type aquamarine = html_rgb(0x7fffd4);
        static constexpr color_type turquoise = html_rgb(0x40e0d0);
        static constexpr color_type mediumturquoise = html_rgb(0x48d1cc);
        static constexpr color_type darkturquoise = html_rgb(0x00ced1);
        static constexpr color_type cadetblue = html_rgb(0x5f9ea0);
        static constexpr color_type steelblue = html_rgb(0x4682b4);
        static constexpr color_type lightsteelblue = html_rgb(0xb0c4de);
        static constexpr color_type powderblue = html_rgb(0xb0e0e6);
        static constexpr color_type lightblue = html_rgb(0xadd8e6);
        static constexpr color_type skyblue = html_rgb(0x87ceeb);
        static constexpr color_type lightskyblue = html_rgb(0x87cefa);
        static constexpr color_type deepskyblue = html_rgb(0x00bfff);
        static constexpr color_type dodgerblue = html_rgb(0x1e90ff);
        static constexpr color_type cornflowerblue = html_rgb(0x6495ed);
        static constexpr color_type royalblue = html_rgb(0x4169e1);
        static constexpr color_type blue = html_rgb(0x0000ff);
        static constexpr color_type mediumblue = html_rgb(0x0000cd);
        static constexpr color_type darkblue = html_rgb(0x00008b);
        static constexpr color_type navy = html_rgb(0x000080);
        static constexpr color_type midnightblue = html_rgb(0x191970);
        // brown html color names
        static constexpr color_type cornsilk = html_rgb(0xfff8dc);
        static constexpr color_type blanchedalmond = html_rgb(0xffebcd);
        static constexpr color_type bisque = html_rgb(0xffe4c4);
        static constexpr color_type navajowhite = html_rgb(0xffdead);
        static constexpr color_type wheat = html_rgb(0xf5deb3);
        static constexpr color_type burlywood = html_rgb(0xdeb887);
        static constexpr color_type tan = html_rgb(0xd2b48c);
        static constexpr color_type rosybrown = html_rgb(0xbc8f8f);
        static constexpr color_type sandybrown = html_rgb(0xf4a460);
        static constexpr color_type goldenrod = html_rgb(0xdaa520);
        static constexpr color_type darkgoldenrod = html_rgb(0xb8860b);
        static constexpr color_type peru = html_rgb(0xcd853f);
        static constexpr color_type chocolate = html_rgb(0xd2691e);
        static constexpr color_type saddlebrown = html_rgb(0x8b4513);
        static constexpr color_type sienna = html_rgb(0xa0522d);
        static constexpr color_type brown = html_rgb(0xa52a2a);
        static constexpr color_type maroon = html_rgb(0x800000);
        // white html color names
        static constexpr color_type white = html_rgb(0xffffff);
        static constexpr color_type snow = html_rgb(0xfffafa);
        static constexpr color_type honeydew = html_rgb(0xf0fff0);
        static constexpr color_type mintcream = html_rgb(0xf5fffa);
        static constexpr color_type azure = html_rgb(0xf0ffff);
        static constexpr color_type aliceblue = html_rgb(0xf0f8ff);
        static constexpr color_type ghostwhite = html_rgb(0xf8f8ff);
        static constexpr color_type whitesmoke = html_rgb(0xf5f5f5);
        static constexpr color_type seashell = html_rgb(0xfff5ee);
        static constexpr color_type beige = html_rgb(0xf5f5dc);
        static constexpr color_type oldlace = html_rgb(0xfdf5e6);
        static constexpr color_type floralwhite = html_rgb(0xfffaf0);
        static constexpr color_type ivory = html_rgb(0xfffff0);
        static constexpr color_type antiquewhite = html_rgb(0xfaebd7);
        static constexpr color_type linen = html_rgb(0xfaf0e6);
        static constexpr color_type lavenderblush = html_rgb(0xfff0f5);
        static constexpr color_type mistyrose = html_rgb(0xffe4e1);
        // gray html color names
        static constexpr color_type gainsboro = html_rgb(0xdcdcdc);
        static constexpr color_type lightgray = html_rgb(0xd3d3d3);
        static constexpr color_type silver = html_rgb(0xc0c0c0);
        static constexpr color_type darkgray = html_rgb(0xa9a9a9);
        static constexpr color_type gray = html_rgb(0x808080);
        static constexpr color_type dimgray = html_rgb(0x696969);
        static constexpr color_type lightslategray = html_rgb(0x778899);
        static constexpr color_type slategray = html_rgb(0x708090);
        static constexpr color_type darkslategray = html_rgb(0x2f4f4f);
        static constexpr color_type black = html_rgb(0x000000);
    };


    template <typename color_type>
    color_type clamp_visible(const color_type& v)
    {
        return clamp(v, colors<color_type>::black, colors<color_type>::white);
    }
}