/*
 * $Id: image_converter.hpp,v 1.7 2008/06/21 22:25:10 kpharris Exp $
 *
 * Part of "Amethyst" a playground for graphics development
 * Copyright (C) 2003 Kevin Harris
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

#if       !defined(AMETHYST__IMAGE_CONVERTER_HPP)
#define            AMETHYST__IMAGE_CONVERTER_HPP

#include "amethyst/graphics/image.hpp"
#include <cmath>

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
		temp.set(std::pow(temp.r(), 1/gamma),
			std::pow(temp.g(), 1/gamma),
			std::pow(temp.b(), 1/gamma));
		return convert_color<U>(temp);
	}

	/**
	 * Generic conversion from one type of image to another.
	 */
	template <class U, class T>
	image<U> convert_image(const image<T>& img)
	{
		image<U> result(img.get_width(), img.get_height());

		for(unsigned y = 0; y < result.get_height(); ++y)
		{
			for(unsigned x = 0; x < result.get_width(); ++x)
			{
				result(x,y) = convert_color<U>(img(x,y));
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

		for(unsigned y = 0; y < result.get_height(); ++y)
		{
			for(unsigned x = 0; x < result.get_width(); ++x)
			{
				result(x,y) = gamma_convert_color<U>(img(x,y), gamma);
			}
		}
		return result;
	}


	// byte [0,255] --> double [0,1.0)
	template<>
	extern rgbcolor<double>
	convert_color<double,unsigned char>(const rgbcolor<unsigned char>& color);

	// double [0,1.0) --> byte [0,255]
	template<>
	extern rgbcolor<unsigned char>
	convert_color<unsigned char,double>(const rgbcolor<double>& color);

	// double --> double (gamma corrected)
	template<>
	extern rgbcolor<double>
	gamma_convert_color<double,double>(const rgbcolor<double>& color, double gamma);

	// byte [0,255] --> float [0,1.0)
	template<>
	extern rgbcolor<float>
	convert_color<float,unsigned char>(const rgbcolor<unsigned char>& color);

	// float [0,1.0) --> byte [0,255]
	template<>
	extern rgbcolor<unsigned char>
	convert_color<unsigned char,float>(const rgbcolor<float>& color);

	// float --> float (gamma corrected)
	template<>
	extern rgbcolor<float>
	gamma_convert_color<float,float>(const rgbcolor<float>& color, double gamma);

} // namespace amethyst

#endif /* !defined(AMETHYST__IMAGE_CONVERTER_HPP) */
