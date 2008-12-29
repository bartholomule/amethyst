#if    !defined(KH_TGA_OUT_H)
#define         KH_TGA_OUT_H

/*
  tga_out.h

  An extension of the image_output class (in imageout.h) to output to a targa file.
*/
/*
  Revision history:
  19jan1999 Created this file.
*/

#include <graphics/image_io.h>
#include <iostream>
#include <fstream>

namespace amethyst
{
	template <class T>
	class tga_io: public image_io<T>
	{
	public:
		tga_io():image_io<T>() { }
		virtual ~tga_io() {}

		inline bool output(const std::string& filename, const image<T>& source) const
		{
			std::filebuf f;
			f.open(filename.c_str(), std::ios_base::out);
			return output(f, source);
		}
		inline bool output(const std::string& filename, const image<T>& source, T gamma) const
		{
			std::filebuf f;
			f.open(filename.c_str(), std::ios_base::out);
			return output(f, source, gamma);
		}
		inline rc_pointer<image<T> > input(const std::string& filename) const
		{
			std::ifstream i(filename.c_str());
			return input(i);
		}
		virtual bool output(std::ostream& o, const image<T>& source) const;
		virtual bool output(std::streambuf& stream, const image<T>& source) const;
		virtual bool output(std::ostream& o, const image<T>& source, T gamma) const;
		virtual bool output(std::streambuf& stream, const image<T>& source, T gamma) const;

		template <class S> bool basic_output(S& o, const image<T>& source) const;
		template <class S> bool basic_output(S& o, const image<T>& source, T gamma) const;

		virtual rc_pointer<image<T> > input(std::istream& i) const;

	private:
		typedef unsigned char byte;
		typedef unsigned short word;
	};

	template <class T>
	template <class S>
	bool tga_io<T>::basic_output(S& o, const image<T>& source) const
	{
		int x, y;
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
		int width = source.get_width();
		int height = source.get_height();
		// Write the main header.
		for( size_t j = 0; j < sizeof(main_header) / sizeof(*main_header); ++j )
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
		for(y = 0; y < height; ++y)
		{
			for(x = 0; x < width; ++x)
			{
				rgbcolor<unsigned char> c = convert_color<unsigned char>(source(x,y));
				//				std::cerr << "color conversion of " << source(x,y) << " returned (" << int(c.r()) <<"," << int(c.g()) <<"," << int(c.b()) << ")" << std::endl;
				output_byte(o, c.b());
				output_byte(o, c.g());
				output_byte(o, c.r());
			}
		}
		// FIXME! Use proper state checking...
		return true;
	}

	template <class T>
	template <class S>
	bool tga_io<T>::basic_output(S& o, const image<T>& source, T gamma) const
	{
		image<T> foo = convert_image<T>(source, gamma);
		return basic_output(o, foo);
	}

	/*
	  bool tga_io<T>::output(std::streambuf& stream, image<T> const& source) const

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
	template <class T>
	bool tga_io<T>::output(std::streambuf& stream, image<T> const& source) const
	{
		return basic_output(stream, source);
	}

	template <class T>
	bool tga_io<T>::output(std::ostream& o, const image<T>& source) const
	{
		return basic_output(o, source);
	}

	template <class T>
	bool tga_io<T>::output(std::ostream& o, const image<T>& source, T gamma) const
	{
		return basic_output(o, source, gamma);
	}

	template <class T>
	bool tga_io<T>::output(std::streambuf& stream, const image<T>& source, T gamma) const
	{
		return basic_output(stream, source, gamma);
	}

	template <class T>
	rc_pointer<image<T> > tga_io<T>::input(std::istream& i) const
	{
		// Statics with constructors are bad.  Fix this.
		static rc_pointer<image<T> > trash_image(new image<T>(1,1));
		byte version;
		byte b1, b2;
		byte r, g, b;
		int width, height;

		i.ignore(2);
		i.get((char&)version);
		if(version != 2)
		{
			std::cout << "Incorrect version (" << int(version) << ")" << std::endl;
			return trash_image;
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
		if((b1 != 24) || ((b2 != 32) && (b2 != 0)))
		{
			std::cout << "Invalid constants..." << std::endl;
			return trash_image;
		}
		bool flip_y = false;
		if( b2 == 0 )
		{
			flip_y = true;
		}

		rc_pointer<image<T> > dest_ptr(new image<T>(width,height));

		image<T>& dest = *dest_ptr;

		for(int y = 0; y < height; ++y)
		{
			for(int x = 0; x < width; ++x)
			{
				i.get((char&)b).get((char&)g).get((char&)r);

				int y_pos = y;
				if( flip_y )
				{
					y_pos = (height-1) - y;
				}

				dest(x,y_pos).set(word(r)/256.0,
					word(g)/256.0,
					word(b)/256.0);
			}
		}
		std::cout << "Loaded image of size " << width << "x" << height << std::endl;

		return dest_ptr;
	}

} // namespace amethyst

#endif /* !defined(KH_TGA_OUT_H) */


