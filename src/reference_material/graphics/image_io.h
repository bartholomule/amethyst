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

#include "raster.h"
#include <string>
#include <fstream>
#include <magic_pointer.h>

template <class T>
class image_io
{
public:
  image_io() { }
  virtual ~image_io() { }

  bool output(const std::string& filename, const raster<T>& source) const
  {
    std::ofstream o(filename.c_str());
    return output(o, source);    
  }
  bool output(const std::string& filename, const raster<T>& source, T gamma) const
  {
    std::ofstream o(filename.c_str());
    return output(o, source, gamma);    
  }
  virtual bool output(std::ostream& o, const raster<T>& source, T gamma) const = 0;
  virtual bool output(std::ostream& o, const raster<T>& source) const = 0;
  magic_pointer<raster<T> > input(const std::string& filename) const
  {
    std::ifstream i(filename.c_str());
    return input(i);
  }  
  virtual magic_pointer<raster<T> > input(std::istream& i) const = 0;
protected:
};



#endif /* !defined(KH_IMAGEOUT_H) */


