#pragma once

#include "graphics/image_io.hpp"
#include <iostream>
#include <fstream>

namespace amethyst
{
	template <typename T>
	class ppm_io : public image_io<T>
	{
	public:
		ppm_io() = default;
		~ppm_io() = default;

		using image_io<T>::output;
		using image_io<T>::input;

		bool output(std::ostream& o, const image<T>& source) const override;
		bool output(std::streambuf& stream, const image<T>& source) const override;

		template <class S> bool basic_output(S& o, const image<T>& source) const;

		std::shared_ptr<image<T>> input(std::istream& i) const override;
	};

	namespace
	{
		void write_string(std::ostream& o, const std::string& s)
		{
			o << s;
		}

		void write_string(std::streambuf& o, const std::string& s)
		{
			o.sputn(s.c_str(), s.size());
		}
	}

//#define USE_PPM_P6

#if defined(USE_PPM_P6)
	template <typename T>
	template <typename S>
	bool ppm_io<T>::basic_output(S& o, const image<T>& source) const
	{
		std::string header = string_format("P6 %1 %2 255\n", int(source.get_width()), int(source.get_height())
		);

		write_string(o, header);

		for (size_t y = 0; y < source.get_height(); ++y)
		{
			for (size_t x = 0; x < source.get_width(); ++x)
			{
				auto c = convert_color<uint8_t>(source(x, y));		
				output_byte(o, c.r() & 0xff);
				output_byte(o, c.g() & 0xff);
				output_byte(o, c.b() & 0xff);
			}
		}
		return true;
	}
	
	/*
	template <typename T>
	template <typename S>
	bool ppm_io<T>::basic_output(S& o, const image<T>& source) const
	{
		std::string header = string_format(
			"P6\n"    // binary format
			"%1 %2\n" // width, height
			"65535\n" // using 16-bit output
			, int(source.get_width()), int(source.get_height())
		);

		write_string(o, header);

		for (size_t y = 0; y < source.get_height(); ++y)
		{
			for (size_t x = 0; x < source.get_width(); ++x)
			{
				auto c = convert_color<uint16_t>(source(x, y));
				output_byte(o, c.r() >> 8);
				output_byte(o, c.r() & 0xff);
				output_byte(o, c.g() >> 8);
				output_byte(o, c.g() & 0xff);
				output_byte(o, c.b() >> 8);
				output_byte(o, c.b() & 0xff);
			}
		}
		return true;
	}
	*/
#else
	template <typename T>
	template <typename S>
	bool ppm_io<T>::basic_output(S& o, const image<T>& source) const
	{
		std::string header = string_format(
			"P3\n"    // binary format
			"%1 %2\n" // width, height
			"255\n" // using 8-bit output
			, int(source.get_width()), int(source.get_height())
		);

		write_string(o, header);

		for (size_t y = 0; y < source.get_height(); ++y)
		{
			for (size_t x = 0; x < source.get_width(); ++x)
			{
				if (x != 0) 
				{
					output_byte(o, ' ');
				}

				auto c = convert_color<uint8_t>(source(x, y));

				auto ctext = string_format("%1 %2 %3", unsigned(c.r()), unsigned(c.g()), unsigned(c.b()));
				write_string(o, ctext);
			}
			output_byte(o, '\n');
		}
		return true;
	}
#endif


	template <typename T>
	bool ppm_io<T>::output(std::ostream& o, const image<T>& source) const
	{
		return basic_output(o, source);
	}

	template <typename T>
	bool ppm_io<T>::output(std::streambuf& stream, const image<T>& source) const
	{
		return basic_output(stream, source);
	}


	template <typename T>
	std::shared_ptr<image<T>> ppm_io<T>::input(std::istream& i) const
	{
		throw "not yet implemented";
	}
}