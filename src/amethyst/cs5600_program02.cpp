/*
 * Program 2 - Kevin Harris 20070204
 * Peter Shirley's CS5600 course, spring 2007
 *
 * No.  I'm not in the course, but he invited people to complete the
 * assignments.  I thought this may be a way to get back into improving
 * amethyst and/or panorama.
 * ---------------------------------------------------------------------------
 *
 * Requirements:
 * Create a web page that has nine images that you create by a
 * program. The intensity (between 0 and 1) of the image is:
 * double f(double x, double y, double kx, double ky, int nx, int ny)
 * { return 0.25*(x/nx)*(1+cos( kx* x*x))*(y/ny)*(1+cos(ky*y*y)); }
 *
 * where the image has nx by ny pixels and on the coordinate system
 * where (x,y) in a member of the set [0,nx] X [0,ny], and
 * (nx,ny) = (512,384) and kx=0.004 and ky=0.006.
 *
 * The coordinate system is that pixel 0,0 is the lower lefthand
 * corner and pixel i,j covers area [i,i+1] \times [j,j+1]
 *
 * For each of regular, random, and jittered samples, create an image
 * with 4, 16, and 64 samples per pixel. For the random and jittered
 * samples, use a different sample set for each pixel.
 *
 * One way to think of this assignment is that you are looking through
 * a window screen (like the one to keep flies out) and you see
 * different intensities through the holes. As x and y get bigger you
 * see lots of high frequencies even through one screen hole. Your
 * mission is to make an image where each pixel is the average
 * intensity seen through each screen hole. You do this for one pixel
 * by sampling several points and averaging the function value. For
 * example, with four samples, the estimate of the intensity of pixel
 * (7,4) might be:
 * f(7+r(),4+r(),...) + f(7+r(),4+r(),...) + f(7+r(),4+r(),...) +
 * f(7+r(),4+r(),...)
 *
 * where r() is a function like drand48() that returns random numbers
 * in (0,1)
 *
 * Gamma correction:
 * While you are computing a greyscale intensity between zero and one,
 * you may need to output a 24-bit RGB. For most image files you would
 * write 0 0 0 for black and 255 255 255 for white. Because monitors
 * are non-linear, you should gamma correct the intensity before
 * writing it to a file. For monitors with gamma 2, each of the R, G,
 * and B channels would be int(255.99*sqrt(I)) where I is the
 * intesity. You use 255.99 to handle the case where I is exactly 1.0.
 *
 * Extra credit:
 * In addition, do an analytic solution for the image and plot the
 * error as number of samples changes for the three sampling methods.
 */

/*
 * What I've found when writing this:
 * * The sample generation classes were not flexible enough to change the
 *   random number generator after creation.  I fixed this.
 * * The image IO needed some work, as it did not properly handle conversions
 *   and gamma adjustment.
 *
 */

#include <graphics/raster.hpp>
#include <graphics/rgbcolor.hpp>
#include <graphics/image_loader.hpp>
#include <graphics/samplegen2d.hpp>
#include <general/string_format.hpp>
#include <string>
#include <iostream>

using namespace amethyst;

typedef double number_type;
typedef rgbcolor<number_type> color;
typedef sample_generator_2d<number_type> generator;

number_type f(number_type x, number_type y, number_type kx, number_type ky, int nx, int ny)
{
	return (	0.25 *
		(x / nx) *
		(1 + cos(kx * x * x)) *
		(y / ny) *
		(1 + cos(ky * y * y))
	);
}

void generate_image(const std::string& filename,
	int width, int height, int samples_per_pixel,
	number_type kx, number_type ky, number_type gamma,
	std::shared_ptr<generator> sampler)
{
	image<number_type> image(width, height);
	for( int y = 0; y < height; ++y )
	{
		for( int x = 0; x < width; ++x )
		{
			std::vector<coord2<number_type> > samples =
				sampler->get_samples(samples_per_pixel);

			number_type total = 0;
			for( size_t i = 0; i < samples.size(); ++i )
			{
				total += f(x + samples[i].x(),
					y + samples[i].y(),
					kx, ky, width, height);
			}
			// The image needs to be inverted so that (0,0) is the lower left
			// corner of the image.
			image(x, height - 1 - y) = color(total / samples.size());
		}
	}
	std::cerr << "Saving image " << filename << std::endl;
	save_image(filename, image, gamma);
}

int main(int argc, const char** argv)
{
	number_type kx = 0.004;
	number_type ky = 0.006;
	int width = 512;
	int height = 384;

	typedef std::pair<const char*const, std::shared_ptr<generator> > gen_pair;
	std::vector<gen_pair> generators;

	generators.push_back(gen_pair("regular", std::shared_ptr<generator>(new regular_sample_2d<number_type>())));
	generators.push_back(gen_pair("random", std::shared_ptr<generator>(new random_sample_2d<number_type>())));
	generators.push_back(gen_pair("jitter", std::shared_ptr<generator>(new jitter_sample_2d<number_type>())));
	generators.push_back(gen_pair("multi_jitter", std::shared_ptr<generator>(new multi_jitter_sample_2d<number_type>())));
	generators.push_back(gen_pair("poisson", std::shared_ptr<generator>(new poisson_sample_2d<number_type>(default_random<number_type>(), 1.0/64))));
	generators.push_back(gen_pair("nrooks", std::shared_ptr<generator>(new nrooks_sample_2d<number_type>())));

	number_type gamma_values[] = { 1.7, 2.2 };
	int sample_counts[] = { 4, 16, 64, 256 };
	for( size_t k = 0; k < sizeof(gamma_values) / sizeof(*gamma_values); ++k )
	{
		number_type gamma = gamma_values[k];
		for( size_t j = 0; j < generators.size(); ++j )
		{
			// Reset the random seed so the various gamma outputs will differ
			// only in gamma, and not in randomness.
			generators[j].second->set_rand_gen(default_random<number_type>(0));

			for( size_t i = 0; i < sizeof(sample_counts) / sizeof(*sample_counts); ++i )
			{
				generate_image(
					string_format("cs5600_program02-%1-%2-%3.ppm",
						generators[j].first,
						sample_counts[i],
						gamma),
					width, height, sample_counts[i],
					kx, ky, gamma, generators[j].second);
			}
		}
	}
}
