
/*
 * Program 3 - Kevin Harris - 20070207, 20070210
 * Peter Shirley's CS5600 course, Spring 2007
 * ---------------------------------------------------------------------------
 * No.  I'm not in the course, but he invited people to complete the
 * assignments.  I thought this may be a way to get back into improving
 * amethyst and/or panorama.
 *
 * ---------------------------------------------------------------------------
 *
 *   Create a web page that has the output of a program that raterizes
 * triangles. This program will take as input a file of triangles with RGB
 * colors defined at each vertex. The file will first have a list of vertices
 * and then will have a list of triangles in screen coordinates (see program
 * 2). The file will have no comments in it, so ignore the coments in this
 * example:
 * cs5600 // magic header
 * 200 100 // a 200 wide by 100 high image should be output
 * 2.2 // the gamma value for the image
 * 16 // 16 jittered samples per pixel-- ignore this for now
 * 4 2 // 4 vertices and 2 triangles
 * 2.2 4.4 0.3 1.0 0.0 0.0 // the first vertex (x,y,z) and then (r,g,b)
 * 180.2 10.3 0.2 0.0 1.0 0.5 // second vertex, ignore z
 * 180.2 80.3 0.2 1.0 1.0 0.5 // third vertex
 * 10.2 90.3 0.4 1.0 0.0 0.5 // fourth vertex
 * 0 1 2 // first triangle, first vertex is vertex 0
 * 0 2 3 // second triangle
 *
 * You should take one sample at the center of each pixel. Run this on this
 * file
 *
 * Your program should rasterize triangles in order, so overlapping triangles
 * will have later triangles in the file "win".
 *
 * Extra Credit:
 *   Do antialiasing by maintaining a set of samples for each pixel, e.g., for
 * pixel 7, 9, you might maintain a set of sixteen jittered sample locations
 * within the pixel and each triangle would be rasterized with respect to those
 * points.
 */

/*
 * What I've found when writing this:
 * * The number of samples per pixel could be more closely matched for most
     sampling methods.  These have been modified to favor the width over
     height.  For many sampling methods 32spp will now be treated as 6x5
     samples instead of 5x5.
 */

#include <graphics/image.hpp>
#include <graphics/tga_io.h>
#include <graphics/rgbcolor.hpp>
#include <graphics/samplegen2d.hpp>
#include <math/point3.hpp>
#include <general/quick_vector.hpp>
#include <general/string_format.hpp>
#include <string>

#include <iostream>
#include <fstream>

using namespace amethyst;

//typedef double number_type;
typedef float number_type;
typedef rgbcolor<number_type> color;
typedef sample_generator_2d<number_type> generator;

struct point_entry
{
	point_entry() { }
	point_entry(point3<number_type>& p, rgbcolor<number_type>& c) : location(p), color(c) { }
	point3<number_type> location;
	rgbcolor<number_type> color;
};

struct vert_index_triplet
{
	vert_index_triplet() : first(-1), second(-1), third(-1) { }
	vert_index_triplet(int one, int two, int three) : first(one), second(two), third(three) { }
	int first;
	int second;
	int third;
};

struct file_data
{
	std::string header;
	int width;
	int height;
	number_type gamma;
	int samples_per_pixel;
	size_t num_verts;
	size_t num_triangles;
	quick_vector<point_entry> vertices;
	quick_vector<vert_index_triplet> triangles;
};

bool parse_file_data(const std::string& name, file_data& data)
{
	std::ifstream file(name.c_str());

	if( !file )
	{
		return false;
	}

	file >> data.header;
	std::cout << "Read header: " << data.header << std::endl;
	if( data.header != "cs5600" )
	{
		return false;
	}
	file >> data.width;
	file >> data.height;
	std::cout << string_format("width x height = %1 x %2", data.width, data.height) << std::endl;
	file >> data.gamma;
	std::cout << "gamma=" << data.gamma << std::endl;
	file >> data.samples_per_pixel;
	std::cout << "spp=" << data.samples_per_pixel << std::endl;
	file >> data.num_verts;
	std::cout << "verts=" << data.num_verts << std::endl;
	file >> data.num_triangles;
	std::cout << "tris=" << data.num_triangles << std::endl;

	for( size_t v = 0; v < data.num_verts; ++v )
	{
		point_entry p;

		file >> p.location[0];
		file >> p.location[1];
		file >> p.location[2];

		file >> p.color[0];
		file >> p.color[1];
		file >> p.color[2];

		data.vertices.push_back(p);
	}
	for( size_t t = 0; t < data.num_triangles; ++t )
	{
		vert_index_triplet v;
		file >> v.first;
		file >> v.second;
		file >> v.third;

		data.triangles.push_back(v);
	}
	return true;
}

void rasterize_triangle(const point_entry& p1, const point_entry& p2, const point_entry& p3,
	image<number_type>& image,
	const raster<quick_vector<coord2<number_type> > >& fixed_samples)
{
	// We're doing a simple projection (dropping the 'z'), so we can do all
	// sorts of shortcuts.

	// Stick the vertices in a array to simplify the searching, slope
	// calculation, etc.
	coord2<number_type> verts[3];
	verts[0].set(p1.location.x(), p1.location.y());
	verts[1].set(p2.location.x(), p2.location.y());
	verts[2].set(p3.location.x(), p3.location.y());

	// Precalculate the vectors defining the two sides of the triangle that
	// include the first point (common corner).
	coord2<number_type> v1(verts[1] - verts[0]);
	coord2<number_type> v2(verts[2] - verts[0]);

	// precalculate some constant values...
	number_type v1xv2y = v1.x() * v2.y();
	number_type v2xv1y = v2.x() * v1.y();

	// Precalculate some color differences (for interpolation later)
	color c1 = p1.color;
	color c2 = p2.color - p1.color;
	color c3 = p3.color - p1.color;

	// See the comment below about why this is #ifdefd out.
#if defined(speedup_with_artifacts)
	// The next vertex (cyclical).  THis is used for simplification of the code below.
	int next[3] = { 1, 2, 0 };

	// Precalculate the slope of each projected line segment.
	number_type slopes[3];
	for( int i = 0; i < 3; ++i )
 	{
		slopes[i] = (verts[next[i]].y() - verts[i].y()) / (verts[next[i]].x() - verts[i].x());
	}
#endif


	// Find a simple bounding value for y that contains the entire triangle
	// (within the limits of the viewing area).
	number_type y_min = std::min(std::min(verts[0].y(), verts[1].y()), verts[2].y());
	number_type y_max = std::max(std::max(verts[0].y(), verts[1].y()), verts[2].y());

	int y1 = std::max<int>(int(y_min - 1), 0);
	int y2 = std::min<int>(int(y_max + 1), image.get_height());

#if !defined(speedup_with_artifacts)
	number_type x_min = std::min(std::min(verts[0].x(), verts[1].x()), verts[2].x());
	number_type x_max = std::max(std::max(verts[0].x(), verts[1].x()), verts[2].x());
	int x1 = std::max<int>(int(x_min - 1), 0);
	int x2 = std::min<int>(int(x_max + 1), image.get_width());
#endif

	//	std::cout << string_format("y range=[%1,%2]", y1, y2) << std::endl;

	// Rasterize each potential line that intersects the projected triangle.
	for( int y = y1; y < y2; ++y )
	{
		// This section here is a speed improvement over the box bounding method.
		// Unfortunately, it has sampling artifacts that I have not yet figured
		// out.  The speedup showed about 15% improvement over the box.  At some
		// point, I should find out why this leaves horizontal stripes over some
		// small portions of the image.
#if defined(speedup_with_artifacts)
		// We know that everything must be within [0,width)
		number_type x_min = int(image.get_width()) * number_type(10);
		number_type x_max = -int(image.get_width()) * number_type(10);
		//		std::cout << string_format("x_min=%1, x_max=%2", x_min, x_max) << std::endl;

		// This loop serves to find the bounding min and max values for X along
		// the line.
		for( int i = 0; i < 3; ++i )
		{
			number_type px = (y - verts[i].y()) / slopes[i] + verts[i].x();
			//			std::cout << string_format("y=%1, verts[%2].y=%3, slope[%2]=%4, verts[%2].x=%5", y, i, verts[i].y(), slopes[i], verts[i].x()) << std::endl;
			if( ((px >  verts[i].x()) &&  (px <= verts[next[i]].x())) ||
				((px <= verts[i].x()) &&  (px >  verts[next[i]].x())) )
			{
				x_min = std::min(px, x_min);
				x_max = std::max(px, x_max);
			}
			//			std::cout << string_format("loop=%1, x_min=%2, x_max=%3, px=%4", i, x_min, x_max, px) << std::endl;
		}

		int x1 = std::max<int>(0, int(x_min - 1));
		int x2 = std::min<int>(int(x_max + 1), image.get_width());
#endif
		//		std::cout << string_format("x range=[%1,%2]", x1, x2) << std::endl;

		// This loop does the actual rasterization for the current line (y),
		// between the calculated min and max x values.
		for( int x = x1; x < x2; ++x )
		{
			// Take some number of samples and check each to see if it is inside
			// the triangle.  If so, interpolate the colors given at the three
			// vertices.
			color c(number_type(0));
			int samples_inside=0;

			const quick_vector<coord2<number_type> >& samples = fixed_samples(x,y);

			for( size_t i = 0; i < samples.size(); ++i )
			{
				// The vector between the sample point and the primary vertex.
				coord2<number_type> v3(coord2<number_type>(
												  samples[i].x() + x,
												  samples[i].y() + y) -
					verts[0]);

				// Check if (x+sx, x+sy) is inside the triangle.
				number_type beta = (
					((v3.x() * v2.y()) - (v3.y() * v2.x())) /
					((v1.x() * v2.y()) - (v1.y() * v2.x()))
				);
				number_type gamma = (v3.y() - beta * v1.y()) / v2.y();

				if( (beta > 0) &&
					(gamma > 0) &&
					((beta + gamma) < 1) )
				{
					// We're inside the triangle... Call it a hit.
					++samples_inside;

					// Interpolate the colors at the corners...
					c += c1 + beta * c2 + gamma * c3;
				}
			}
			if( samples_inside > 0 )
			{
				c = c * (1 / number_type(samples_inside));
				int inverted_y = image.get_height() - 1 - y;

				// Store the color value.  Again, this flips the image vertically.
				image(x,inverted_y) = c;
				// If we were doing a zbufer, we'd compare it and store the z as well.
			}
		}
	}
}

void rasterize_triangles(const file_data& data, image<number_type>& image,
	const raster<quick_vector<coord2<number_type> > >& samples)
{
	// Blank the image...
	for(int y = 0; y < image.get_height(); ++y)
	{
		for(int x = 0; x <image.get_width(); ++x)
		{
			image(x,y).set(0,0,0);
		}
	}

	int percent_done = -1;

	// Go through each image and rasterize it.
	for( quick_vector<vert_index_triplet>::const_iterator iter = data.triangles.begin();
		iter != data.triangles.end();
		++iter )
	{
		int new_percent = 100 * (iter - data.triangles.begin()) / data.triangles.size();
		if( new_percent != percent_done )
		{
			std::cout << string_format("Rasterizing (%1%%) triangle #%2 of %3", new_percent,
				iter - data.triangles.begin(),
				data.triangles.size()) << std::endl;
			percent_done = new_percent;
		}
		rasterize_triangle(
			data.vertices[iter->first],
			data.vertices[iter->second],
			data.vertices[iter->third],
			image, samples);
	}
}

raster<quick_vector<coord2<number_type> > > get_samples(
	int width, int height,
	int samples_per_pixel,
	rc_pointer<generator> sampler = rc_pointer<generator>())
{
	raster<quick_vector<coord2<number_type> > > samples(width,height);

	for( int y = 0; y < height; ++y )
	{
		for( int x = 0; x < width; ++x )
		{
			if( sampler && samples_per_pixel > 1)
			{
				samples(x,y) = sampler->get_samples(samples_per_pixel);
			}
			else
			{
				samples(x,y) = quick_vector<coord2<number_type> >(1, coord2<number_type>(0.5, 0.5));
			}
		}
	}
	return samples;
}


int main(int argc, const char** argv)
{
	file_data data;
	std::string filename("program3.txt");
	if( parse_file_data(filename, data) )
	{
		std::cout << string_format("Read data from file \"%1\"", filename) << std::endl;
		std::cout << string_format("File contains %1 verts and %2 triangles", data.vertices.size(), data.triangles.size()) << std::endl;

		// Project and rasterize the triangles.
		image<number_type> image(data.width, data.height);
		tga_io<number_type> output;
		raster<quick_vector<coord2<number_type> > > samples =get_samples(data.width, data.height,
			data.samples_per_pixel,
			rc_pointer<generator>(new jitter_sample_2d<number_type>()));

		std::cout << "rasterizing with antialiasing" << std::endl;
		rasterize_triangles(data, image, samples);
		std::cout << "Writing data to file.." << std::endl;
		output.output(string_format("cs5600_program03_antialiased-%1spp-%2.tga", samples(0,0).size(), data.gamma), image, data.gamma);
		output.output(string_format("cs5600_program03_antialiased-%1spp-%2.tga", samples(0,0).size(), 1.7), image, 1.7);

		std::cout << "rasterizing" << std::endl;
		samples = get_samples(data.width, data.height, 1);
		rasterize_triangles(data, image, samples);
		std::cout << "Writing data to file.." << std::endl;
		output.output(string_format("cs5600_program03-%1spp-%2.tga", samples(0,0).size(), data.gamma), image, data.gamma);
		output.output(string_format("cs5600_program03-%1spp-%2.tga", samples(0,0).size(), 1.7), image, 1.7);
	}
	else
	{
		std::cout << string_format("Could not read file \"%1\"", filename) << std::endl;
		return 1;
	}
	return 0;
}
