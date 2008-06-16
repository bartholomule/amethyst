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
#include "general/rc_pointer.hpp"

namespace amethyst
{

	template <class T>
	class image_io
	{
	public:
		image_io()
		{
		}
		virtual ~image_io()
		{
		}

		bool output(const std::string& filename, const image<T>& source) const
		{
			std::ofstream o(filename.c_str());
			return output(o, source);
		}
		bool output(const std::string& filename, const image<T>& source, T gamma) const
		{
			std::ofstream o(filename.c_str());
			return output(o, source, gamma);
		}
		virtual bool output(std::ostream& o, const image<T>& source, T gamma) const = 0;
		virtual bool output(std::ostream& o, const image<T>& source) const = 0;

		void output_byte(std::streambuf& stream, unsigned char data) const
		{
			stream.sputc(std::streambuf::char_type(data));
		}
		void output_byte(std::ostream& stream, unsigned char data) const
		{
			stream.put(data);
		}


		rc_pointer<image<T> > input(const std::string& filename) const
		{
			std::ifstream i(filename.c_str());
			return input(i);
		}
		virtual rc_pointer<image<T> > input(std::istream& i) const = 0;
	protected:
	};

} // namespace amethyst

#endif /* !defined(KH_IMAGEOUT_H) */


