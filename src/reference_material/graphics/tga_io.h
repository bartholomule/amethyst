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

#include "image_io.h"
#include <iostream>

namespace amethyst {

template <class T>
class tga_io: public image_io<T>
{
public:
  tga_io():image_io<T>() { }
  virtual ~tga_io() {}


  inline bool output(const std::string& filename, const raster<T>& source) const
  {
    std::ofstream o(filename.c_str());
    return output(o,source);
  }
  inline bool output(const std::string& filename, const raster<T>& source, T gamma) const
  {
    std::ofstream o(filename.c_str());    
    return output(o, source, gamma);
  }  
  inline rc_pointer<raster<T> > input(const std::string& filename) const
  {
    std::ifstream i(filename.c_str());
    return input(i);
  }
  virtual bool output(std::ostream& o,      const raster<T>& source) const;
  virtual bool output(std::ostream& o,      const raster<T>& source, T gamma) const;  
  virtual rc_pointer<raster<T> > input(std::istream& i) const;  

private:
  typedef unsigned char byte;
  typedef unsigned short  word;  

};

template <class T>
bool tga_io<T>::output(std::ostream& o, const raster<T>& source) const
{
  byte r, g, b;
  int x, y;
  static byte main_header[12] = {0,0,2,0,0,0,0,0,0,0,0,0};
  int width = source.get_width();
  int height = source.get_height();
  // Write the main header
  for(int i = 0; i < 12; ++i)
    o << main_header[i];
  // Write the width and height
  o << byte(width  & 0xff) << byte((width >>8) & 0xff);
  o << byte(height & 0xff) << byte((height>>8) & 0xff);
  // Write the end of the header
  o << byte(24) << byte(32);

  for(y = 0; y < height; ++y)
  {
    for(x = 0; x < width; ++x)
    {
      const T& p = source(x,y);

      r = byte(std::min<int>(std::max<int>(int(p.r() * 256), 0), 255) & 0xFF);
      g = byte(std::min<int>(std::max<int>(int(p.g() * 256), 0), 255) & 0xFF);
      b = byte(std::min<int>(std::max<int>(int(p.b() * 256), 0), 255) & 0xFF);

      o << b << g << r;
    }
  }
  return(o.good());
}

template <class T>
bool tga_io<T>::output(std::ostream& o, const raster<T>& source, T gamma) const
{
  // FIXME!
  return output(o, source);
  /*
  byte r, g, b;
  int x, y;
  static byte main_header[12] = {0,0,2,0,0,0,0,0,0,0,0,0};
  int width = source.get_width();
  int height = source.get_height();
  // Write the main header
  for(int i = 0; i < 12; ++i)
    o << main_header[i];
  // Write the width and height
  o << byte(width  & 0xff) << byte((width >>8) & 0xff);
  o << byte(height & 0xff) << byte((height>>8) & 0xff);
  // Write the end of the header
  o << byte(24) << byte(32);

  for(y = 0; y < height; ++y)
  {
    for(x = 0; x < width; ++x)
    {
      source(x,y).gamma_clipped_int_triplet(r,g,b, gamma);
      o << b << g << r;
    }
  }
  */
  return(o.good());
}


template <class T>
rc_pointer<raster<T> > tga_io<T>::input(std::istream& i) const
{
  static rc_pointer<raster<T> > trash_raster(new raster<T>(1,1));
  byte version;
  byte b1, b2;
  byte r, g, b;
  int width, height;  

  i.ignore(2);
  i.get((char&)version);
  if(version != 2)
  {
    std::cout << "Incorrect version (" << int(version) << ")" << std::endl;
    return trash_raster;
  }
  else
  {
    std::cout << "Version is " << (int)version << std::endl;
  }
  i.ignore(9);

  i.get((char&)b1).get((char&)b2);
  width  = word(b1) + (word(b2) << 8);

  i.get((char&)b1).get((char&)b2);  
  height = word(b1) + (word(b2) << 8);

  i.get((char&)b1).get((char&)b2);    
  if((b1 != 24) && (b2 != 32))
  {
    std::cout << "Invalid constants..." << std::endl;    
    return trash_raster;
  }

  rc_pointer<raster<T> > dest_ptr(new raster<T>(width,height));

  raster<T>& dest = *dest_ptr;
  
  for(int y = 0; y < height; ++y)
  {
    for(int x = 0; x < width; ++x)
    {
      i.get((char&)b).get((char&)g).get((char&)r);
      dest(x,y).set(word(r)/256.0,
		    word(g)/256.0,
		    word(b)/256.0);
    }
  }
  std::cout << "Loaded image of size " << width << "x" << height << std::endl;

  return dest_ptr;
}

} // namespace amethyst {

#endif /* !defined(KH_TGA_OUT_H) */


