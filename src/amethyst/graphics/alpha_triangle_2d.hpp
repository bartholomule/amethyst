/*
 * $Id: alpha_triangle_2d.hpp,v 1.1 2008/12/29 17:28:21 kpharris Exp $
 *
 * Part of "Amethyst" -- A playground for graphics development.
 * Copyright (C) 2008 Kevin Harris
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#if       !defined(AMETHYST__ALPHA_TRIANGLE_2D_HPP)
#define            AMETHYST__ALPHA_TRIANGLE_2D_HPP

#include "amethyst/math/coord2.hpp"
#include "amethyst/graphics/rgbcolor.hpp"
#include "amethyst/graphics/image.hpp"

namespace amethyst
{
    template <class T>
    struct alpha_vertex_2d
    {
        coord2<T> xy;
        rgbcolor<T> rgb;
        T a;
    };

    template <class T>
    struct alpha_triangle_2d
    {
        alpha_vertex_2d<T> v1;
        alpha_vertex_2d<T> v2;
        alpha_vertex_2d<T> v3;
    };

    enum alpha_vertex_fields
    {
        E_VERTEX2_X = 0, E_VERTEX2_Y,
        E_VERTEX2_R, E_VERTEX2_G, E_VERTEX2_B,
        E_VERTEX2_A,
    };

    template <class T>
    void compare_and_swap(alpha_vertex_2d<T>& p1, alpha_vertex_2d<T>& p2, alpha_vertex_fields field)
    {
        bool less = false;
        switch (field)
        {
        case E_VERTEX2_X:
            less = p2.xy.x() < p1.xy.x();
            break;
        case E_VERTEX2_Y:
            less = p2.xy.y() < p1.xy.y();
            break;
        case E_VERTEX2_R:
        case E_VERTEX2_G:
        case E_VERTEX2_B:
            less = p2.xy[field - E_VERTEX2_R] < p1.xy[field - E_VERTEX2_R];
            break;
        case E_VERTEX2_A:
            less = p2.a < p1.a;
            break;
        }

        if (less)
        {
            std::swap(p2, p1);
        }
    }

    /**
     * Draw an alpha pixel without any form of bounds checking (for speed).
     * @param pixels The raw pixel data from an image, raster or other array.
     * @param width The width of the image or raster.
     * @param height The height of the image or raster.
     * @param x The x location to draw the pixel.
     * @param y The y location to draw the pixel.
     * @param c The color to draw.
     * @param alpha The transparency of the color.  The pixel would be fully
     *   opaque with an alpha of 0 and would be fully transparent with an alpha
     *   of 1.
     */
    template <class T>
    void put_alpha_pixel_unchecked(rgbcolor<T>* pixels, size_t width, size_t height, size_t x, size_t y, const rgbcolor<T>& c, T alpha)
    {
        rgbcolor<T>& existing = pixels[y * width + x];
        T a1 = 1 - alpha;
        existing.set(
            existing.r() * alpha + c.r() * a1,
            existing.g() * alpha + c.g() * a1,
            existing.b() * alpha + c.b() * a1);
    }

    /**
     * An inner loop of DDA (Bresenhem's algo) to draw a single horizontal line.
     * @param pixels The raw pixel data from an image, raster or other array.
     * @param width The width of the image or raster.
     * @param height The height of the image or raster.
     * @param x1 The starting x location to draw the line.
     * @param x2 The ending x location to draw the line.
     * @param y The y location to draw the line.
     * @param c1 The color to draw at x1.
     * @param c2 The color to draw at x2.
     * @param a1 The alpha value at x1.
     * @param a2 The alpha value at x2.
     * @param swap_x_and_y A flag to allow vertical lines to be drawn by
     *   swapping x and y.
     */
    template <class T>
    void draw_horizontal_line(
        rgbcolor<T>* pixels,
        size_t width, size_t height,
        T x1, T x2, T y,
        rgbcolor<T> c1, rgbcolor<T> c2,
        T a1, T a2,
        bool swap_x_and_y = false)
    {
        if (!swap_x_and_y)
        {
            if ((y < 0) || (size_t(y) >= height))
            {
                return;
            }
        }
        else
        {
            if ((y < 0) || (size_t(y) >= width))
            {
                return;
            }
        }

        // Go between x1 and x2, a1 and a2, c1 and c2
        T dx = x2 - x1;

        if (dx < 0)
        {
            std::swap(x1, x2);
            std::swap(c1, c2);
            std::swap(a1, a2);
            dx = x2 - x1;
        }

        T a_dx = (a2 - a1) / dx;
        rgbcolor<T> cdx = (c2 - c1) * (1 / dx);

        T a = a1;
        rgbcolor<T> c = c1;

        // Do the bounds checking here so it doesn't need to be done for each pixel.
        if (x1 < 0)
        {
            a = a1 - x1 * a_dx;
            c = c1 - x1 * cdx;
            x1 = 0;
        }
        if ((x2 + 0.5) >= width)
        {
            x2 = T(width - 1);
        }

        size_t endx = size_t(x2 + 0.5);
        for (size_t x = size_t(x1 + 0.5); x <= endx; x += 1)
        {
            if (swap_x_and_y)
            {
                put_alpha_pixel_unchecked(pixels, width, height, size_t(y), x, c, a);
            }
            else
            {
                put_alpha_pixel_unchecked(pixels, width, height, x, size_t(y), c, a);
            }
            a += a_dx;
            c += cdx;
        }
    }

    /**
     *
     * An algorithm similar to Bresenems's line drawing, but for rasterizing an
     * alpha-blended triangle (one sample per pixel).
     *
     * At some point, this could be modified to draw the body of the triangle
     * with 1 sample per pixel (no need for more) and draw smooth edges using
     * more samples.
     */
    template <class T>
    void dda_rasterize_triangle(
        rgbcolor<T>* pixels,
        size_t width, size_t height,
        alpha_vertex_2d<T> p1, alpha_vertex_2d<T> p2, alpha_vertex_2d<T> p3)
    {

        // Do a sort of the 3 points by y coord.  This is done with 3 compare/swap operations.
        compare_and_swap(p1, p2, E_VERTEX2_Y);
        compare_and_swap(p2, p3, E_VERTEX2_Y);
        compare_and_swap(p1, p2, E_VERTEX2_Y);

        // This needs to be fixed to avoid the problem when ((p2.y - p1.y) < 1).
        // Note that I didn't say "or (p3.x - p1.x) < 1", since that would be a
        // degenerate triangle.
        //
        // The way around the problem is to swap x and y for p1, p2, and p3, and
        // swap them again when storing in the image.
        bool swap_x_and_y = false;
        if (fabs(p2.xy.y() - p1.xy.y()) < 1)
        {
            swap_x_and_y = true;
            std::swap(p1.xy.x(), p1.xy.y());
            std::swap(p2.xy.x(), p2.xy.y());
            std::swap(p3.xy.x(), p3.xy.y());

            // Resort them.
            compare_and_swap(p1, p2, E_VERTEX2_Y);
            compare_and_swap(p2, p3, E_VERTEX2_Y);
            compare_and_swap(p1, p2, E_VERTEX2_Y);
        }

        coord2<T> dp1 = p2.xy - p1.xy;
        coord2<T> dp2 = p3.xy - p1.xy;
        T da1 = p2.a - p1.a;
        T da2 = p3.a - p1.a;

        rgbcolor<T> dc1 = p2.rgb - p1.rgb;
        rgbcolor<T> dc2 = p3.rgb - p1.rgb;

        // Rasterization needs to be done in 2 segments:
        // 1. p1.y <= y <= p2.y
        //    p1.x <= x1 <= p2.x   or   p2.x <= x1 <= p1.x
        //    p1.x <= x2 <= p3.x   or   p3.x <= x2 <= p1.x
        // 2. p2.y <= y <= p3.y
        //    p2.x <= x1 <= p3.x   or   p3.x <= x1 <= p2.x
        //    p1.x <= x2 <= p3.x   or   p3.x <= x2 <= p1.x

        coord2<T> dp1_dy = dp1 / dp1.y();
        coord2<T> dp2_dy = dp2 / dp2.y();
        rgbcolor<T> dc1_dy = dc1 * (1 / dp1.y());
        rgbcolor<T> dc2_dy = dc2 * (1 / dp2.y());
        T da1_dy = da1 * (1 / dp1.y());
        T da2_dy = da2 * (1 / dp2.y());

        // Segment 1.
        T y = p1.xy.y();
        T x1 = p1.xy.x();
        T x2 = x1;
        T a1 = p1.a;
        T a2 = a1;
        rgbcolor<T> c1 = p1.rgb;
        rgbcolor<T> c2 = c1;
        if (y < p2.xy.y())
        {
            for (; y <= p2.xy.y(); y += 1)
            {
                draw_horizontal_line(pixels, width, height, x1, x2, y, c1, c2, a1, a2, swap_x_and_y);

                x1 += dp1_dy.x();
                x2 += dp2_dy.x();
                a1 += da1_dy;
                a2 += da2_dy;
                c1 += dc1_dy;
                c2 += dc2_dy;
            }
        }

        if (y < p3.xy.y())
        {
            // Segment 2.
            coord2<T> dp3 = p3.xy - p2.xy;
            rgbcolor<T> dc3 = p3.rgb - p2.rgb;
            T da3 = p3.a - p2.a;
            coord2<T> dp3_dy = dp3 / dp3.y();
            rgbcolor<T> dc3_dy = dc3 * (1 / dp3.y());
            T da3_dy = da3 * (1 / dp3.y());

            x1 = p3.xy.x() + (y - p3.xy.y()) * dp3_dy.x();
            //			a1 = p3.a + (y - p3.a) * da3_dy;

            for (; y <= p3.xy.y(); y += 1)
            {
                draw_horizontal_line(pixels, width, height, x1, x2, y, c1, c2, a1, a2, swap_x_and_y);

                x1 += dp3_dy.x();
                x2 += dp2_dy.x();
                a1 += da3_dy;
                a2 += da2_dy;
                c1 += dc3_dy;
                c2 += dc2_dy;
            }
        }
    }

    template <class T>
    void dda_rasterize_triangle(
        rgbcolor<T>* pixels,
        size_t width, size_t height,
        const alpha_triangle_2d<T>& triangle)
    {
        dda_rasterize_triangle(pixels, width, height, triangle.v1, triangle.v2, triangle.v3);
    }

    template <class T>
    void dda_rasterize_triangle(
        image<T>& img,
        const alpha_triangle_2d<T>& triangle)
    {
        rgbcolor<T>* pixels = img.template reinterpret<rgbcolor<T>*>();
        dda_rasterize_triangle(
            pixels,
            img.get_width(), img.get_height(),
            triangle.v1, triangle.v2, triangle.v3);
    }
} // namespace amethyst


#endif /* !defined(AMETHYST__ALPHA_TRIANGLE_2D_HPP) */

