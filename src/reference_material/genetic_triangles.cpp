#define KH_USE_TIME_FOR_RANDOM

#include <graphics/image.hpp>
#include <graphics/tga_io.h>
#include <graphics/rgbcolor.hpp>
#include <graphics/samplegen2d.hpp>
#include <math/point3.hpp>
#include <general/quick_vector.hpp>
#include <general/string_format.hpp>
#include "amethyst/general/random.hpp"
#include <string>

#include <iostream>
#include <iomanip>
#include <fstream>

using namespace amethyst;

//typedef double number_type;
typedef float number_type;
typedef rgbcolor<number_type> color;
typedef sample_generator_2d<number_type> generator;

struct vertex
{
	color rgb;
	point3<number_type> xya;
};

void compare_and_swap(vertex& p1, vertex& p2, size_t index)
{
	if( p2.xya[index] < p1.xya[index] )
	{
		std::swap(p2, p1);
	}
}

// Add the term to the current sum, add the dropped bits to the correction
// term, and return the new sum
template <typename T>
inline T corrected_sum(T current_sum, T term, T& correction)
{
  T sum1 = current_sum + term;
  // In an ideal world, this would be zero: (x - (x + t) + t)
  correction += (current_sum - sum1) + term;
  return(sum1);
}

void put_alpha_pixel_unchecked(color* pixels, int width, int height, int x, int y, const color& c, number_type alpha)
{
	color& existing = pixels[y * width + x];
	number_type a1 = 1 - alpha;
	existing.set(
		existing.r() * alpha + c.r() * a1,
		existing.g() * alpha + c.g() * a1,
		existing.b() * alpha + c.b() * a1);
}

void put_alpha_pixel(image<number_type>& img, int x, int y, const color& c, number_type alpha)
{
	if( ( (x >= 0) && (x < img.get_width()) ) &&
		( (y >= 0) && (y < img.get_height()) ) )
	{
		color& existing = img(x,y);
		number_type a1 = 1 - alpha;
		existing.set(
			existing.r() * alpha + c.r() * a1,
			existing.g() * alpha + c.g() * a1,
			existing.b() * alpha + c.b() * a1);
	}
}

// An inner loop of DDA (Bresenhem's algo) to draw a single horizontal line.
void draw_horizontal_line(image<number_type>& img, number_type x1, number_type x2, number_type y, color c1, color c2, number_type a1, number_type a2, bool swap_x_and_y = false)
{
	// Go between x1 and x2, a1 and a2, c1 and c2
	number_type dx = x2 - x1;

	if( dx < 0 )
	{
		std::swap(x1, x2);
		std::swap(c1, c2);
		std::swap(a1, a2);
		dx = x2 - x1;
	}

	color* pixels = img.reinterpret<color*>();
	int width = img.get_width();
	int height = img.get_height();

	number_type a_dx =  (a2 - a1) / dx;
	color cdx = (c2 - c1) * (1 / dx);

	number_type a = a1;
	color c = c1;

	// Do the bounds checking here so it doesn't need to be done for each pixel.
	if( x1 < 0 )
	{
		a = -x1 * a_dx;
		c = -x1 * cdx;
		x1 = 0;
	}
	if( x2 > width )
	{
		x2 = width - 1;
	}

	if( swap_x_and_y )
	{
		for( number_type x = x1; x <= x2; x += 1 )
		{
			put_alpha_pixel_unchecked(pixels, width, height, y, x, c, a);
			a += a_dx;
			c += cdx;
		}
	}
	else
	{
		for( number_type x = x1; x <= x2; x += 1 )
		{
			put_alpha_pixel_unchecked(pixels, width, height, x, y, c, a);
			a += a_dx;
			c += cdx;
		}
	}
}

// An algorithm similar to Bresenems's line drawing, but for rasterizing an
// alpha-blended triangle (one sample per pixel).
//
// At some point, this could be modified to draw the body of the triangle with
// 1 sample per pixel (no need for more) and draw smooth edges using more
// samples.
void dda_rasterize_triangle(vertex p1, vertex p2, vertex p3, image<number_type>& img)
{
	// Do a sort of the 3 points by y coord.  This is done with 3 compare/swap operations.
	compare_and_swap(p1, p2, 1);
	compare_and_swap(p2, p3, 1);
	compare_and_swap(p1, p2, 1);

	// This needs to be fixed to avoid the problem when ((p2.y - p1.y) < 1).
	// Note that I didn't say "or (p3.x - p1.x) < 1", since that would be a
	// degenerate triangle.
	//
	// The way around the problem is to swap x and y for p1, p2, and p3, and
	// swap them again when storing in the image.
	bool swap_x_and_y = false;
	if( fabs(p2.xya.y() - p1.xya.y()) < 1 )
	{
		swap_x_and_y = true;
		std::swap(p1.xya.x(), p1.xya.y());
		std::swap(p2.xya.x(), p2.xya.y());
		std::swap(p3.xya.x(), p3.xya.y());
	}

	vector3<number_type> dp1 = p2.xya - p1.xya;
	vector3<number_type> dp2 = p3.xya - p1.xya;

	color dc1 = p2.rgb - p1.rgb;
	color dc2 = p3.rgb - p1.rgb;


	// FIXME! Handle FP addition errors by using the corrected_sum function.

	// Rasterization needs to be done in 2 segments:
	// 1. p1.y <= y <= p2.y
	//    p1.x <= x1 <= p2.x   or   p2.x <= x1 <= p1.x
	//    p1.x <= x2 <= p3.x   or   p3.x <= x2 <= p1.x
	// 2. p2.y <= y <= p3.y
	//    p2.x <= x1 <= p3.x   or   p3.x <= x1 <= p2.x
	//    p1.x <= x2 <= p3.x   or   p3.x <= x2 <= p1.x

	vector3<number_type> dp1_dy = dp1 / dp1.y();
	vector3<number_type> dp2_dy = dp2 / dp2.y();
	color dc1_dy = dc1 * (1 / dp1.y());
	color dc2_dy = dc2 * (1 / dp2.y());

	// Segment 1.
	number_type y = p1.xya.y();
	number_type x1 = p1.xya.x();
	number_type x2 = x1;
	number_type a1 = p1.xya.z();
	number_type a2 = a1;
	color c1 = p1.rgb;
	color c2 = c1;
	for(; y < p2.xya.y(); y += 1)
	{
		draw_horizontal_line(img, x1, x2, y, c1, c2, a1, a2, swap_x_and_y);

		x1 += dp1_dy.x();
		x2 += dp2_dy.x();
		a1 += dp1_dy.z();
		a2 += dp2_dy.z();
		c1 += dc1_dy;
		c2 += dc2_dy;
	}

	if( y < p3.xya.y() )
	{
		// Segment 2.
		vector3<number_type> dp3 = p3.xya - p2.xya;
		color dc3 = p3.rgb - p2.rgb;
		vector3<number_type> dp3_dy = dp3 / dp3.y();
		color dc3_dy = dc3 * (1 / dp3.y());

		for(; y < p3.xya.y(); y += 1)
		{
			draw_horizontal_line(img, x1, x2, y, c1, c2, a1, a2, swap_x_and_y);

			x1 += dp3_dy.x();
			x2 += dp2_dy.x();
			a1 += dp3_dy.z();
			a2 += dp2_dy.z();
			c1 += dc3_dy;
			c2 += dc2_dy;
		}
	}
}

void rasterize_triangle(const vertex& p1, const vertex& p2, const vertex& p3,
	image<number_type>& image,
	const raster<quick_vector<coord2<number_type> > >& fixed_samples)
{
	// We're doing a simple projection (dropping the 'z'), so we can do all
	// sorts of shortcuts.

	// Stick the vertices in a array to simplify the searching, slope
	// calculation, etc.
	coord2<number_type> verts[3];
	verts[0].set(p1.xya.x(), p1.xya.y());
	verts[1].set(p2.xya.x(), p2.xya.y());
	verts[2].set(p3.xya.x(), p3.xya.y());

	// Precalculate the vectors defining the two sides of the triangle that
	// include the first point (common corner).
	coord2<number_type> v1(verts[1] - verts[0]);
	coord2<number_type> v2(verts[2] - verts[0]);

	// precalculate some constant values...
	number_type v1xv2y = v1.x() * v2.y();
	number_type v2xv1y = v2.x() * v1.y();

	// Precalculate some color differences (for interpolation later)
	color c1 = p1.rgb;
	color c2 = p2.rgb - p1.rgb;
	color c3 = p3.rgb - p1.rgb;
	number_type a1 = p1.xya.z();
	number_type a2 = p2.xya.z() - a1;
	number_type a3 = p3.xya.z() - a1;

	// Find a simple bounding value for y that contains the entire triangle
	// (within the limits of the viewing area).
	number_type y_min = std::min(std::min(verts[0].y(), verts[1].y()), verts[2].y());
	number_type y_max = std::max(std::max(verts[0].y(), verts[1].y()), verts[2].y());

	int y1 = std::max<int>(int(y_min - 1), 0);
	int y2 = std::min<int>(int(y_max + 1), image.get_height());

	number_type x_min = std::min(std::min(verts[0].x(), verts[1].x()), verts[2].x());
	number_type x_max = std::max(std::max(verts[0].x(), verts[1].x()), verts[2].x());
	int x1 = std::max<int>(int(x_min - 1), 0);
	int x2 = std::min<int>(int(x_max + 1), image.get_width());

	// Rasterize each potential line that intersects the projected triangle.
	for( int y = y1; y < y2; ++y )
	{
		//		std::cout << string_format("x range=[%1,%2]", x1, x2) << std::endl;

		// This loop does the actual rasterization for the current line (y),
		// between the calculated min and max x values.
		for( int x = x1; x < x2; ++x )
		{
			// Take some number of samples and check each to see if it is inside
			// the triangle.  If so, interpolate the colors given at the three
			// vertices.
			color c(number_type(0));
			number_type alpha(0);
			size_t samples_inside = 0;

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
					alpha += a1 + beta * a2 + gamma * a3;
					c += c1 + beta * c2 + gamma * c3;
				}
			}
			if( samples_inside > 0 )
			{
				number_type factor = (1 / number_type(samples_inside));
				c *= factor;
				alpha *= factor;

				put_alpha_pixel(image, x, y, c, alpha);
			}
		}
	}
}

struct alpha_triangle
{
	vertex v1;
	vertex v2;
	vertex v3;
};

void rasterize_triangles(quick_vector<alpha_triangle> triangles, image<number_type>& image,
	const raster<quick_vector<coord2<number_type> > >& samples, bool quiet = true)
{
	if( !quiet )
	{
		std::cout << "Rasterizing " << triangles.size() << " triangles." << std::endl;
	}

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
	for( quick_vector<alpha_triangle>::const_iterator iter = triangles.begin();
		iter != triangles.end();
		++iter )
	{
		if( !quiet )
		{
			int new_percent = 100 * (iter - triangles.begin()) / triangles.size();
			if( new_percent != percent_done )
			{
				std::cout << string_format("Rasterizing (%1%%) triangle #%2 of %3", new_percent,
					iter - triangles.begin(),
					triangles.size()) << std::endl;
				percent_done = new_percent;
			}
		}
		//		rasterize_triangle(iter->v1, iter->v2, iter->v3, image, samples);
		dda_rasterize_triangle(iter->v1, iter->v2, iter->v3, image);
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

void generate_random_triangles(quick_vector<alpha_triangle>& triangles, size_t count, int width, int height, Random<number_type>& r)
{
	triangles.clear();
	for( size_t i = 0; i < count; ++i )
	{
		alpha_triangle t;
		t.v1.rgb = color(r.next(), r.next(), r.next());
		t.v1.xya = point3<number_type>(r.next() * width, r.next() * height, r.next());
		t.v2.rgb = color(r.next(), r.next(), r.next());
		t.v2.xya = point3<number_type>(r.next() * width, r.next() * height, r.next());
		t.v3.rgb = color(r.next(), r.next(), r.next());
		t.v3.xya = point3<number_type>(r.next() * width, r.next() * height, r.next());

		triangles.push_back(t);
	}
}

number_type calculate_error(const image<number_type>& ref, const image<number_type>& img)
{
	number_type error(0);

	size_t width = ref.get_width();
	size_t height = ref.get_height();

	const color* ref_pixels = ref.raw_data();
	const color* img_pixels = img.raw_data();

	size_t max_pixel = width * height;
	for( size_t i = 0; i < max_pixel; ++i )
	{
		color difference = (ref_pixels[i] - img_pixels[i]) * 256;
		error += difference.r() * difference.r() + difference.g() * difference.g() + difference.b() * difference.b();
	}
	return error;
}

struct pleb
{
	number_type error;

	struct pleb_data_entry
	{
		union
		{
			struct
			{
				number_type x;
				number_type y;
				number_type r;
				number_type g;
				number_type b;
				number_type a;
			} pleb_verts[3];

			number_type raw_data[18];
		};

		pleb_data_entry()
		{
			for(size_t i = 0; i < 18; ++i)
			{
				raw_data[i] = 0;
			}
		}

		pleb_data_entry(const pleb_data_entry& e)
		{
			*this = e;
		}

		pleb_data_entry& operator=(const pleb_data_entry& e)
		{
			for(size_t i = 0; i < 18; ++i)
			{
				raw_data[i] = e.raw_data[i];
			}
		}
	};

	quick_vector<pleb_data_entry> pleb_data;

	pleb()
		: error(std::numeric_limits<number_type>::max())
		, pleb_data()
	{
	}

	pleb(size_t triangles)
		: error(std::numeric_limits<number_type>::max())
		, pleb_data(triangles)
	{
	}
};

typedef quick_vector<pleb> population;


std::ostream& operator<<(std::ostream& o, const pleb& p)
{
	o << p.pleb_data.size() << " " << p.error;
	for( size_t i = 0; i < p.pleb_data.size(); ++i )
	{
		for( size_t j = 0; j < 18; ++j )
		{
			o << " " << std::setprecision(6) << p.pleb_data[i].raw_data[j];
		}
	}
	return o;
}

std::istream& operator>>(std::istream& input, pleb& p)
{
	size_t triangles;
	input >> triangles;
	p.pleb_data.resize(triangles);
	input >> p.error;

	for( size_t i = 0; i < p.pleb_data.size(); ++i )
	{
		for( size_t j = 0; j < 18; ++j )
		{
			input >> p.pleb_data[i].raw_data[j];
		}
	}
	return input;
}


void convert_to_triangles(pleb& p, quick_vector<alpha_triangle>& output)
{
	size_t triangles = p.pleb_data.size();
	output.resize(triangles);
	for( size_t i = 0; i < triangles; ++i )
	{
		output[i].v1.rgb.set( p.pleb_data[i].pleb_verts[0].r, p.pleb_data[i].pleb_verts[0].g, p.pleb_data[i].pleb_verts[0].b) ;
		output[i].v2.rgb.set( p.pleb_data[i].pleb_verts[1].r, p.pleb_data[i].pleb_verts[1].g, p.pleb_data[i].pleb_verts[1].b) ;
		output[i].v3.rgb.set( p.pleb_data[i].pleb_verts[2].r, p.pleb_data[i].pleb_verts[2].g, p.pleb_data[i].pleb_verts[2].b) ;
		output[i].v1.xya.set( p.pleb_data[i].pleb_verts[0].x, p.pleb_data[i].pleb_verts[0].y, p.pleb_data[i].pleb_verts[0].a) ;
		output[i].v2.xya.set( p.pleb_data[i].pleb_verts[1].x, p.pleb_data[i].pleb_verts[1].y, p.pleb_data[i].pleb_verts[1].a) ;
		output[i].v3.xya.set(p.pleb_data[i].pleb_verts[2].x, p.pleb_data[i].pleb_verts[2].y, p.pleb_data[i].pleb_verts[2].a) ;
	}
}


void shuffle_range(quick_vector<pleb>& data, size_t index1, size_t index2, Random<number_type>& random)
{
	if( !data.empty() )
	{
		//		std::cout << "Shuffling between " << index1 << " and " << index2 << std::endl;

		size_t current = data.size() - 1;
		while( current > 1 )
		{
			size_t swap_number = size_t(random.next_int(current));

			for( int index = index1; index < index2; ++index )
			{
				std::swap(data[current].pleb_data[index / 18].raw_data[index % 18], data[swap_number].pleb_data[index / 18].raw_data[index % 18]);
			}
			--current;
		}
	}
}

void unique_random_list(quick_vector<size_t>& output, size_t min_value, size_t max_value, size_t count, Random<number_type>& random)
{
	output.clear();
	output.reserve(count);

	if( max_value < count )
	{
		throw std::runtime_error("output size is too large for the max value");
	}

	const size_t range = max_value - min_value;

	while( output.size() < count )
	{
		size_t next_loc = min_value + random.next_int(range);
		if( std::find(output.begin(), output.end(), next_loc) == output.end() )
		{
			output.push_back(next_loc);
		}
	}
}

void random_cross(const pleb& p1, const pleb& p2, int points, size_t count, quick_vector<pleb>& output, Random<number_type>& random)
{
	//	std::cout << "Generating random cross..." << std::endl;
	output.clear();
	output.resize(count);

	// Generate some random crossover points, all unique.
	quick_vector<size_t> point_locations;
	point_locations.reserve(points);
	const size_t num_data_points = p1.pleb_data.size() * 18;
	//	std::cout << "Generating " << points << " points between 1 and " << num_data_points << std::endl;

	unique_random_list(point_locations, 1, num_data_points, points, random);
	std::sort(point_locations.begin(), point_locations.end());

	// Populate the output with the existing plebs...  Evenly divided.
	size_t index;
	for( index = 0; index < output.size() / 2; ++index)
	{
		output[index] = p1;
	}
	for( ; index < output.size(); ++index)
	{
		output[index] = p2;
	}

	// Shuffle them between the cross points
	size_t last_index = 0;
	for( quick_vector<size_t>::const_iterator iter = point_locations.begin(); iter != point_locations.end(); ++iter )
	{
		shuffle_range(output, last_index, *iter, random);
		last_index = *iter;
	}
	shuffle_range(output, last_index, num_data_points, random);
}

pleb::pleb_data_entry create_random_triangle(size_t width, size_t height, Random<number_type>& random)
{
	pleb::pleb_data_entry e;
	for( size_t t = 0; t < 3; ++t )
	{
		e.pleb_verts[t].x = random.next_int(width);
		e.pleb_verts[t].y = random.next_int(height);
		e.pleb_verts[t].r = random.next();
		e.pleb_verts[t].g = random.next();
		e.pleb_verts[t].b = random.next();
		e.pleb_verts[t].a = random.next();
	}
	return e;
}

void random_pleb(size_t width, size_t height, size_t count, Random<number_type>& random, pleb& retval)
{
	retval.pleb_data.resize(count);
	for( size_t i = 0; i < count; ++i )
	{
		retval.pleb_data[i] = create_random_triangle(width, height, random);
	}
}

void generate_random_population(size_t width, size_t height, Random<number_type>& random, size_t count, size_t triangles, population& output)
{
	output.clear();
	output.resize(count);

	for( size_t i = 0; i < count; ++i )
	{
		random_pleb(width, height, triangles, random, output[i]);
	}
}

struct pleb_data
{
	size_t pleb_index;
	number_type error;
	rc_pointer<amethyst::image<number_type> > image;
};

bool pleb_is_better(const pleb_data& p1, const pleb_data& p2)
{
	return p1.error < p2.error;
}

bool pleb_is_worse(const pleb_data& p1, const pleb_data& p2)
{
	return p1.error > p2.error;
}

void add_to_statistics(
	const pleb_data& data,
	size_t count_of_each,
	quick_vector<pleb_data>& best,
	quick_vector<pleb_data>& worst)
{
	if( best.empty() )
	{
		best.push_back(data);
	}
	else
	{
		best.push_back(data);
		std::sort(best.begin(), best.end(), &pleb_is_better);
		if( best.size() > count_of_each )
		{
			best.erase(best.begin() + count_of_each, best.end());
		}
	}
	if( worst.empty() )
	{
		worst.push_back(data);
	}
	else
	{
		worst.push_back(data);
		std::sort(worst.begin(), worst.end(), &pleb_is_worse);
		if( worst.size() > count_of_each )
		{
			worst.erase(worst.begin() + count_of_each, worst.end());
		}
	}

}

void calculate_population_error(population& populous, const rc_pointer<image<number_type> >& reference,
	size_t count_of_each,
	quick_vector<pleb_data>& best,
	quick_vector<pleb_data>& worst,
	raster<quick_vector<coord2<number_type> > >& samples)
{
	size_t width = reference->get_width();
	size_t height = reference->get_height();

	best.clear();
	worst.clear();

	std::cout << "Converting and rasterizing...";
	typedef ::image<number_type> image;
	for( size_t i = 0; i < populous.size(); ++i )
	{
		pleb_data data;
		data.pleb_index = i;
		data.image = rc_pointer<image>(new image(width, height));

		quick_vector<alpha_triangle> triangles;
		std::cout << "...#" << i << " (" << populous[i].pleb_data.size() << ")" << std::flush;
		convert_to_triangles(populous[i], triangles);
		rasterize_triangles(triangles, *data.image, samples);
		data.error = calculate_error(*reference, *data.image);
		populous[i].error = data.error;
		add_to_statistics(data, count_of_each, best, worst);
	}
	std::cout << std::endl;
}

rc_pointer<image<number_type> > get_error_image(const image<number_type>& img1, const image<number_type>& img2)
{
	rc_pointer<image<number_type> > retval(new image<number_type>(img1.get_width(), img1.get_height()));

	for( size_t y = 0; y < retval->get_height(); ++y )
	{
		for( size_t x = 0; x < retval->get_width(); ++x )
		{
			color difference = (img1(x,y) - img2(x,y));
			(*retval)(x,y) = color(
				difference.r() * difference.r(),
				difference.g() * difference.g(),
				difference.b() * difference.b());
		}
	}
	return retval;
}

void run_generation(population& populous, const rc_pointer<image<number_type> >& reference,
	quick_vector<pleb_data>& best,
	quick_vector<pleb_data>& worst,
	size_t count_of_each,
	size_t images_to_dump,
	raster<quick_vector<coord2<number_type> > >& samples,
	size_t generation_number,
	number_type gamma,
	image_io<number_type>& io)
{
	std::cout << "Running generation #" << generation_number << std::endl;
	calculate_population_error(populous, reference, count_of_each, best, worst, samples);

	for( size_t i = 0; i < images_to_dump; ++i )
	{
		char buffer[1024];
		snprintf(buffer, sizeof(buffer), "%07d", generation_number);

		// Dump the best image...
		std::cout << "Generation " << buffer << ": Best error #" << i << " = " << best[i].error << " (" << best[i].pleb_index << ")" << std::endl;
		io.output(string_format("genetic_triangles_best-%1-%2.tga", buffer, i), *best[i].image, gamma);

		// Dump the worst image...
		std::cout << "Generation " << buffer << ": Worst error #" << i << " = " << worst[i].error  << " (" << worst[i].pleb_index << ")" << std::endl;
		io.output(string_format("genetic_triangles_worst-%1-%2.tga", buffer, i), *worst[i].image, gamma);

		// Dump the error image...
		rc_pointer<image<number_type> > error = get_error_image(*reference, *(best[i].image));
		io.output(string_format("genetic_triangles_error-%1-%2.tga", buffer, i), *error, gamma);
	}

	// Dump the reference image (for debugging purposes)
	io.output("genetic_triangles_reference.tga", *reference, gamma);
}

void write_population(const char* filename, const population& populous, size_t generation)
{
	std::ofstream output(filename);
	output << generation << "\n";
	output << " " << populous.size() << "\n";
	for( population::const_iterator i = populous.begin(); i != populous.end(); ++i )
	{
		output << " " << *i << "\n";
	}
	output.flush();
}

bool read_population(const char* filename, population& populous, size_t& generation)
{
	std::ifstream input(filename);

	input >> generation;

	size_t size;

	input >> size;

	populous.resize(size);

	for( population::iterator i = populous.begin(); i != populous.end(); ++i )
	{
		input >> *i;
	}
}

void mutate_plebs(population& populous, number_type mutation_rate, Random<number_type>& random, size_t max_width, size_t max_height)
{
	std::cout << "Mutating children... ";
	for( size_t i = 0; i < populous.size(); ++i )
	{
		if( random.next() < mutation_rate )
		{
			std::cout << i << " ";
			// Mutate me...
			size_t data_points = populous[i].pleb_data.size() * 18;
			size_t spot = random.next_int(data_points);
			size_t triangle = spot / 18;
			size_t raw_offset = spot % 18;

			switch( raw_offset % 6 )
			{
			case 0: // x
				populous[i].pleb_data[triangle].raw_data[raw_offset] = random.next_int(max_width);
				break;
			case 1: // y
				populous[i].pleb_data[triangle].raw_data[raw_offset] = random.next_int(max_height);
				break;
			default: // r, g, b, a
				populous[i].pleb_data[triangle].raw_data[raw_offset] = random.next();
			}
		}
	}
	std::cout << std::endl;
}

typedef void (*generation_tweaker)(population& populous, size_t generation, size_t width, size_t height, Random<number_type>& random);
typedef void (*generation_crosser)(population& populous, size_t generation, const quick_vector<pleb_data>& best, const quick_vector<pleb_data>& worst, size_t crossover_points, number_type mutation_rate, size_t width, size_t height, Random<number_type>& random);

void run_for_generations(population& populous, const rc_pointer<image<number_type> >& reference,
	raster<quick_vector<coord2<number_type> > >& samples,
	size_t starting_generation,
	size_t generations,
	size_t images_to_dump,
	int crossover_points,
	number_type gamma,
	Random<number_type>& random,
	image_io<number_type>& io,
	number_type mutation_rate,
	const char* population_file,
	generation_crosser cross_generation,
	generation_tweaker modify_generation)
{
	const size_t width = reference->get_width();
	const size_t height = reference->get_height();

	for( size_t generation = starting_generation; generation < generations; ++generation )
	{
		quick_vector<pleb_data> best;
		quick_vector<pleb_data> worst;

		size_t amount_to_cross = 1 << crossover_points;

		run_generation(populous, reference, best, worst, amount_to_cross, images_to_dump, samples, generation, gamma, io);

		if( (generation + 1) < generations )
		{
 			std::cout << "best[" << generation << "] =";
			for(size_t i = 0; i < best.size(); ++i )
			{
				std::cout << " " << best[i].pleb_index;
			}
			std::cout << std::endl;
			std::cout << "worst[" << generation << "] =";
			for(size_t i = 0; i < worst.size(); ++i )
			{
				std::cout << " " << worst[i].pleb_index;
			}
			std::cout << std::endl;
			// Cross over..
			cross_generation(populous, generation, best, worst, crossover_points, mutation_rate, width, height, random);

			// Do any non-crossing modifications (such as adding new genes).
			modify_generation(populous, generation, width, height, random);
		}
		write_population(population_file, populous, generation);
	}
}

void cross_best_replace_worst(population& populous, size_t generation, const quick_vector<pleb_data>& best, const quick_vector<pleb_data>& worst, size_t crossover_points, number_type mutation_rate, size_t width, size_t height, Random<number_type>& random)
{
	population crossed;
	std::cout << "Crossing " << best[0].pleb_index << " with " << best[1].pleb_index << std::endl;
	random_cross(populous[best[0].pleb_index], populous[best[1].pleb_index], crossover_points, (1 << crossover_points), crossed, random);

	// Mutate and repopulate...
	mutate_plebs(crossed, mutation_rate, random, width, height);

	// Replace the worst...
	for( size_t i = 0; i < crossed.size(); ++i )
	{
		populous[worst[i].pleb_index] = crossed[i];
	}
}

void cross_best_with_random(population& populous, size_t generation, const quick_vector<pleb_data>& best, const quick_vector<pleb_data>& worst, size_t crossover_points, number_type mutation_rate, size_t width, size_t height, Random<number_type>& random)
{
	const number_type birth_rate = 0.1;
	const size_t num_offspring = 3; // Must be > 2 for the random_cross function to do a proper shuffle.  I may want to fix this at some point.
	const number_type death_rate = num_offspring * birth_rate; // Equal to the number that are born... For now.
	population crossed;
	const pleb best_pleb = populous[best[0].pleb_index];

	// Cross a bunch of plebs...
	const size_t num_to_cross = birth_rate * populous.size();
	for( size_t count = 0; count < num_to_cross; ++count )
	{
		// Yes, this could end up doing some cloning, and it won't matter -- odds are better with a smaller population size.
		size_t random_guy = random.next_int(populous.size());
		std::cout << "Crossing " << best[0].pleb_index << " with random guy " << random_guy << std::endl;

		population crossing;
		random_cross(best_pleb, populous[random_guy], crossover_points, num_offspring, crossing, random);

		crossed.append(crossing);
	}

	// Mutate...
	mutate_plebs(crossed, mutation_rate, random, width, height);

	// Kill some.
	const size_t num_to_kill = death_rate * populous.size();
	std::cout << "Killing " << num_to_kill << " plebs:";
	quick_vector<size_t> death_list;
	unique_random_list(death_list, 0, populous.size(), num_to_kill, random);
	std::sort(death_list.begin(), death_list.end(), std::greater<size_t>());
	for( size_t i = 0; i < death_list.size(); ++i )
	{
		size_t guy_to_kill = death_list[i];
		std::cout << " " << guy_to_kill;
		populous.erase(populous.begin() + guy_to_kill);
	}
	std::cout << std::endl;


	// Replace some random plebs, not just the bad guys.
	std::cout << "Adding " << crossed.size() << " plebs." << std::endl;
	populous.append(crossed);
}

void leave_alone(population& populous, size_t generation, size_t width, size_t height, Random<number_type>& random)
{
	// Do nothing
}

void add_occasional_triangles(population& populous, size_t generation, size_t width, size_t height, Random<number_type>& random)
{
	const number_type max_triangles = 300;
	const size_t reach_max_at = 100000;
	number_type expected_triangles = generation * (max_triangles / reach_max_at);
	size_t triangles_at_this_generation = std::min<size_t>(expected_triangles, max_triangles);

	std::cout << "Generation " << generation << ": expect to have " << std::setprecision(6) << expected_triangles << " triangles..." << std::endl;

	if( populous.empty() )
	{
		populous.push_back(pleb(triangles_at_this_generation));
	}

	for( population::iterator i = populous.begin(); i != populous.end(); ++i )
	{
		if( i->pleb_data.size() < triangles_at_this_generation )
		{
			i->pleb_data.push_back(create_random_triangle(width, height, random));
		}
	}
}


int main(int argc, const char** argv)
{
	tga_io<number_type> io;

	if( argc < 2 )
	{
		std::cerr << "Usage: " << argv[0] << " <REFERENCE IMAGE>" << std::endl;
		return 1;
	}

	rc_pointer<image<number_type> > reference = io.input(argv[1]);

	const size_t width = reference->get_width();
	const size_t height = reference->get_height();
	const size_t samples_per_pixel = 1;
	const number_type gamma = 1;
	const size_t initial_population_size = 300;
	const int crossover_points = log(initial_population_size / 2.0) / log(2.0);
	const int num_generations = 1000000;
	const number_type mutation_rate = 0.50;
	const size_t images_per_generation = 1;
	const char* population_file = "genetic_triangles.population";
	size_t starting_generation = 1;

	mersenne_twist_random<number_type> random;

	raster<quick_vector<coord2<number_type> > > samples = get_samples(width, height, samples_per_pixel, rc_pointer<generator>(new jitter_sample_2d<number_type>(random)));

	population populous;


	const size_t starting_triangles = 100; // will increase in add_occasional_triangles
	generate_random_population(width, height, random, initial_population_size, starting_triangles, populous);

	if( argc > 2 )
	{
		if( argv[2] == std::string("--continue") )
		{
			std::cout << "continuing..." << std::endl;
			if( !read_population(population_file, populous, starting_generation) )
			{
				return 1;
			}
			std::cout << "Continuing with generation " << starting_generation << std::endl;
		}
		else
		{
			std::cout << "Not continuing (bad arg)" << std::endl;
		}
	}
	else
	{
		std::cout << "Not continuing (not enough args)" << std::endl;
	}

	run_for_generations(populous, reference, samples, starting_generation, num_generations, images_per_generation, crossover_points, gamma, random, io, mutation_rate, population_file, &cross_best_with_random, &add_occasional_triangles);

	//	run_for_generations(populous, reference, samples, starting_generation, num_generations, images_per_generation, crossover_points, gamma, random, io, mutation_rate, population_file, &cross_best_replace_worst, &add_occasional_triangles);

	return 0;
}