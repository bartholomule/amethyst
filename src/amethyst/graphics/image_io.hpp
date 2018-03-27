#if    !defined(KH_IMAGEOUT_H)
#define         KH_IMAGEOUT_H
/*
  imageout.h
  This is intended as a way to convert a raster to an image that can be output
  to a file or whatever.

*/
/*
  Revision history:
  19jan1999 Created this file.
*/

#include <string>
#include <fstream>

#include "graphics/image.hpp"
#include "graphics/image_converter.hpp"
#include <memory>

namespace amethyst
{

	template <class T, typename ColorType = rgbcolor<T>>
	class image_io
	{
	public:
		image_io() = default;
		virtual ~image_io() = default;

		virtual std::string default_extension() const = 0;

		bool output(const std::string& filename, const raster<ColorType>& source) const
		{
			std::filebuf f;
			f.open(filename.c_str(), std::ios_base::out | std::ios_base::binary);
			return output(f, source);
		}

		bool output(const std::string& filename, const raster<ColorType>& source, T gamma) const
		{
			raster<ColorType> foo = convert_image<T>(source, gamma);

			std::filebuf f;
			f.open(filename.c_str(), std::ios_base::out | std::ios_base::binary);
			return output(f, foo);
		}

		virtual bool output(std::ostream& o, const raster<ColorType>& source) const = 0;
		virtual bool output(std::streambuf& o, const raster<ColorType>& source) const = 0;

		void output_byte(std::streambuf& stream, unsigned char data) const
		{
			stream.sputc(std::streambuf::char_type(data));
		}

		void output_byte(std::ostream& stream, unsigned char data) const
		{
			stream.put(data);
		}

		void output_string(std::streambuf& stream, const std::string& data) const
		{
			stream.sputn(data.c_str(), data.size());
		}

		void output_string(std::ostream& stream, const std::string& data) const
		{
			stream.write(data.c_str(), data.size());
		}

		bool input_byte(std::istream& i, char& c) const
		{
			i.get(c);
			return bool(i);
		}
		void output_word_be(std::streambuf& o, int16_t c) const
		{
			o.sputc(char(c >> 8));
			o.sputc(char(c & 0xff));
		}
		void output_word_be(std::ostream& o, int16_t c) const
		{
			o.put(char(c >> 8));
			o.put(char(c & 0xff));
		}
		bool input_word_be(std::istream& i, int16_t& c) const
		{
			char low, high;
			i.get(high).get(low);
			c = (int16_t(high) << 8) + (int16_t(low) & 0xff);
			std::cout << "low=" << (int(low) & 0xff) << " high=" << (int(high) & 0xff) << " c=" << c << std::endl;
			return bool(i);
		}

		raster<ColorType> input(const std::string& filename) const
		{
			std::ifstream i(filename.c_str(), std::ios::binary);
			return input(i);
		}
		virtual raster<ColorType> input(std::istream& i) const = 0;
	protected:
	};
} // namespace amethyst

#endif /* !defined(KH_IMAGEOUT_H) */


