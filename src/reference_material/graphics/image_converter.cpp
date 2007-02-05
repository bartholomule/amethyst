#include <graphics/image_converter.hpp>

#include <cmath>
#include <iostream>

namespace amethyst
{
	// ---------------------------------------------------------------------------
	// doubles

	template<>
	extern rgbcolor<double>
	convert_color<double,unsigned char>(const rgbcolor<unsigned char>& color)
	{

		//    std::cout << __PRETTY_FUNCTION__ << "---1" << std::endl;
		const double conversion_factor = 1.0 / double((unsigned char)(-1));

		// Just perform the conversion (no clipping).
		return rgbcolor<double>(color.r() * conversion_factor,
			color.g() * conversion_factor,
			color.b() * conversion_factor);
	}

	template<>
	extern rgbcolor<unsigned char>
	convert_color<unsigned char,double>(const rgbcolor<double>& color)
	{
		//    std::cout << __PRETTY_FUNCTION__ << "---2" << std::endl;
		const unsigned char cf = (unsigned char)(-1);

		// Convert/clip it.
		return rgbcolor<unsigned char>((unsigned char)(my_min<double>(color.r(), 1.0) * cf),
			(unsigned char)(my_min<double>(color.g(), 1.0) * cf),
			(unsigned char)(my_min<double>(color.b(), 1.0) * cf));
	}

	template<>
	extern rgbcolor<double>
	gamma_convert_color<double,double>(const rgbcolor<double>& color, double gamma)
	{
		double foo = 1/gamma;
		return rgbcolor<double>(std::pow(color.r(), foo),
			std::pow(color.g(), foo),
			std::pow(color.b(), foo));
	}


	// ---------------------------------------------------------------------------
	// Floats

	template<>
	extern rgbcolor<float>
	convert_color<float,unsigned char>(const rgbcolor<unsigned char>& color)
	{

		//    std::cout << __PRETTY_FUNCTION__ << "---1" << std::endl;
		const float conversion_factor = 1.0 / float((unsigned char)(-1));

		// Just perform the conversion (no clipping).
		return rgbcolor<float>(color.r() * conversion_factor,
			color.g() * conversion_factor,
			color.b() * conversion_factor);
	}

	template<>
	extern rgbcolor<unsigned char>
	convert_color<unsigned char,float>(const rgbcolor<float>& color)
	{
		//    std::cout << __PRETTY_FUNCTION__ << "---2" << std::endl;
		const unsigned char cf = (unsigned char)(-1);

		// Convert/clip it.
		return rgbcolor<unsigned char>((unsigned char)(my_min<float>(color.r(), 1.0) * cf),
			(unsigned char)(my_min<float>(color.g(), 1.0) * cf),
			(unsigned char)(my_min<float>(color.b(), 1.0) * cf));
	}

	template<>
	extern rgbcolor<float>
	gamma_convert_color<float,float>(const rgbcolor<float>& color, double gamma)
	{
		float foo = 1/gamma;
		return rgbcolor<float>(std::pow(color.r(), foo),
			std::pow(color.g(), foo),
			std::pow(color.b(), foo));
	}


} // namespace amethyst
