
/*
 * Program 4 - Kevin Harris - 20070207, 20070210
 * Peter Shirley's CS5600 course, Spring 2007
 * ---------------------------------------------------------------------------
 * No.  I'm not in the course, but he invited people to complete the
 * assignments.  I thought this may be a way to get back into improving
 * amethyst and/or panorama.
 *
 * ---------------------------------------------------------------------------
 *
 *   Modify your program to include a z-buffer so that the closest z-value wins
 * when you are deciding whether to turn a pixel a color.
 *
 * Extra credit
 * Use non-uniform filtering to make your antialiasing better.
 */

/*
 * What I've found when writing this:
 * * Something is causing triangles to poke through when they shouldn't.
 */

#include <graphics/image.hpp>
#include <graphics/rgbcolor.hpp>
#include <graphics/image_loader.hpp>
#include <graphics/samplegen2d.hpp>
#include <math/point3.hpp>
#include <vector>
#include <general/string_format.hpp>
#include <limits>
#include <string>

#include <iostream>
#include <fstream>

using namespace amethyst;

typedef double number_type;
//typedef float number_type;
typedef rgbcolor<number_type> color;
typedef sample_generator_2d<number_type> generator;

struct point_entry
{
    point_entry() {
    }
    point_entry(point3<number_type>& p, rgbcolor<number_type>& c) : location(p), color(c) {
    }
    point3<number_type> location;
    rgbcolor<number_type> color;
};

struct vert_index_triplet
{
    vert_index_triplet() : first(-1), second(-1), third(-1) {
    }
    vert_index_triplet(int one, int two, int three) : first(one), second(two), third(three) {
    }
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
    std::vector<point_entry> vertices;
    std::vector<vert_index_triplet> triangles;
};

bool parse_file_data(const std::string& name, file_data& data)
{
    std::ifstream file(name.c_str());

    if (!file)
    {
        return false;
    }

    file >> data.header;
    std::cout << "Read header: " << data.header << std::endl;
    if (data.header != "cs5600")
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

    for (size_t v = 0; v < data.num_verts; ++v)
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
    for (size_t t = 0; t < data.num_triangles; ++t)
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
                        size_t triangle_number,
                        image<number_type>& image, raster<number_type>& zbuffer,
                        const raster<std::vector<coord2<number_type>>>& fixed_samples)
{
    // We're doing a simple projection (dropping the 'z'), so we can do all
    // sorts of shortcuts.

    // Stick the vertices in a array to simplify the searching, slope
    // calculation, etc.
    coord3<number_type> verts[3];
    verts[0] = p1.location.getcoord();
    verts[1] = p2.location.getcoord();
    verts[2] = p3.location.getcoord();
    //	std::cout << string_format("p1=%1, p2=%2, p3=%3", verts[0], verts[1], verts[2]) << std::endl;

    rgbcolor<number_type> colors[3];
    colors[0] = p1.color;
    colors[1] = p2.color;
    colors[2] = p3.color;

    // The next vertex (cyclical).  THis is used for simplification of the code below.
    int next[3] = { 1, 2, 0 };

    // Precalculate the vectors defining the two sides of the triangle that
    // include the first point (common corner).
    vector3<number_type> v1(verts[1] - verts[0]);
    vector3<number_type> v2(verts[2] - verts[0]);

    // precalculate some constant values...
    number_type v1xv2y = v1.x() * v2.y();
    number_type v2xv1y = v2.x() * v1.y();

    //	std::cout << "Precalculation for triangle " << triangle_number << std::endl;
    //	std::cout << string_format("p1=%1, p2=%2, p3=%3", verts[0], verts[1], verts[2]) << std::endl;
    //	std::cout << string_format("v1=%1, v2=%2, v1xv2y=%3, v2xv1y=%4", v1, v2, v1xv2y, v2xv1y) << std::endl;

    // Precalculate some color differences (for interpolation later)
    color c1 = colors[0];
    color c2 = colors[1] - colors[0];
    color c3 = colors[2] - colors[0];

    // See the comment below about why this is #ifdefd out.
#if defined(speedup_with_artifacts)

    // Precalculate the slope of each projected line segment.
    number_type slopes[3];
    for (int i = 0; i < 3; ++i)
    {
        slopes[i] = (verts[next[i]].y() - verts[i].y()) / (verts[next[i]].x() - verts[i].x());
    }
#endif


    // Find a simple bounding value for y that contains the entire triangle
    // (within the limits of the viewing area).
    number_type y_min = std::min(std::min(verts[0].y(), verts[1].y()), verts[2].y());
    number_type y_max = std::max(std::max(verts[0].y(), verts[1].y()), verts[2].y());

    int y1 = std::max<int>(int(y_min - 1), 0);
    int y2 = std::min<int>(int(y_max + 1), int(image.get_height()));

#if !defined(speedup_with_artifacts)
    number_type x_min = std::min(std::min(verts[0].x(), verts[1].x()), verts[2].x());
    number_type x_max = std::max(std::max(verts[0].x(), verts[1].x()), verts[2].x());
    int x1 = std::max<int>(int(x_min - 1), 0);
    int x2 = std::min<int>(int(x_max + 1), int(image.get_width()));
#endif

    //	std::cout << string_format("y range=[%1,%2]", y1, y2) << std::endl;
    int total_sample_count = 0;

    // Rasterize each potential line that intersects the projected triangle.
    for (int y = y1; y < y2; ++y)
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
        for (int i = 0; i < 3; ++i)
        {
            number_type px = (y - verts[i].y()) / slopes[i] + verts[i].x();
            //			std::cout << string_format("y=%1, verts[%2].y=%3, slope[%2]=%4, verts[%2].x=%5", y, i, verts[i].y(), slopes[i], verts[i].x()) << std::endl;
            if (((px > verts[i].x()) && (px <= verts[next[i]].x())) ||
                ((px <= verts[i].x()) && (px > verts[next[i]].x())))
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
        for (int x = x1; x < x2; ++x)
        {
            // Take some number of samples and check each to see if it is inside
            // the triangle.  If so, interpolate the colors given at the three
            // vertices.
            color c(number_type(0));
            int samples_inside = 0;

            const std::vector<coord2<number_type>>& samples = fixed_samples(x, y);

            number_type z = 0;

            //			std::cout << "Samples.size()=" << samples.size() << std::endl;

            for (size_t i = 0; i < samples.size(); ++i)
            {
                coord3<number_type> sp3(samples[i].x() + x, samples[i].y() + y, 0);
                // The vector between the sample point and the primary vertex.
                coord3<number_type> v3(sp3 - verts[0]);

                //				std::cout << string_format("sp3=%1, v3=%2", sp3, v3) << std::endl;

                // Check if (x+sx, x+sy) is inside the triangle.
                number_type beta = (
                    ((v3.x() * v2.y()) - (v3.y() * v2.x())) /
                    ((v1.x() * v2.y()) - (v1.y() * v2.x()))
                    );
                number_type gamma = (v3.y() - beta * v1.y()) / v2.y();

                //				std::cout << string_format("beta=%1, gamma=%2, beta+gamma=%3", beta, gamma, beta+gamma) << std::endl;

                if ((beta > 0) &&
                    (gamma > 0) &&
                    ((beta + gamma) < 1))
                {
                    // We're inside the triangle... Call it a hit.
                    ++samples_inside;

                    // Interpolate the colors at the corners...
                    c += c1 + beta * c2 + gamma * c3;

                    number_type cur_z = verts[0].z() + beta * v1.z() + gamma * v2.z();
                    //					std::cout << string_format("Hit triangle at (%1,%2,%3)", sp3.x(), sp3.y(), cur_z) << std::endl;
                    z += cur_z;
                }
            }
            total_sample_count += samples_inside;
            if (samples_inside > 0)
            {
                number_type epsilon = std::numeric_limits<number_type>::epsilon();
                number_type scale = (1 / number_type(samples_inside));
                z = z * scale;
                int inverted_y = int(image.get_height()) - 1 - y;
                if (z < (zbuffer(x, inverted_y) + epsilon))
                {
                    c = c * scale;
                    // Store the color value.  Again, this flips the image vertically.
                    image(x, inverted_y) = c;
                    zbuffer(x, inverted_y) = z;
                }
            }
        }
    }
    if (total_sample_count == 0)
    {
        //		std::cout << string_format("No hits for triangle %1 {%2,%3,%4}",triangle_number, verts[0], verts[1], verts[2]) << std::endl;
        //		std::cout << string_format("x1=%1, x2=%2, y1=%3, y2=%4", x1,x2,y1,y2) << std::endl;
    }
}

void rasterize_triangles(const file_data& data, image<number_type>& image,
                         const raster<std::vector<coord2<number_type>>>& samples)
{
    raster<number_type> zbuffer(image.get_width(), image.get_height());

    // Blank the image and set the zbuffer to the max.
    image.fill({ 0,0,0 });
    zbuffer.fill(std::numeric_limits<number_type>::max());

    size_t percent_done = ~0;

    // Go through each image and rasterize it.
    for (std::vector<vert_index_triplet>::const_iterator iter = data.triangles.begin();
         iter != data.triangles.end();
         ++iter)
    {
        size_t new_percent = 100 * (iter - data.triangles.begin()) / data.triangles.size();
        if (new_percent != percent_done)
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
            iter - data.triangles.begin(),
            image, zbuffer, samples);
    }
}

raster<std::vector<coord2<number_type>>> get_samples(
    int width, int height,
    int samples_per_pixel,
    std::shared_ptr<generator> sampler = std::shared_ptr<generator>())
{
    raster<std::vector<coord2<number_type>>> samples(width, height);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (sampler && samples_per_pixel > 1)
            {
                samples(x, y) = sampler->get_samples(samples_per_pixel);
            }
            else
            {
                samples(x, y) = std::vector<coord2<number_type>>(1, coord2<number_type>(0.5, 0.5));
            }
        }
    }
    return samples;
}


int main(int argc, const char** argv)
{
    file_data data;
    std::string filename("program3.txt");
    if (parse_file_data(filename, data))
    {
        std::cout << string_format("Read data from file \"%1\"", filename) << std::endl;
        std::cout << string_format("File contains %1 verts and %2 triangles", data.vertices.size(), data.triangles.size()) << std::endl;

        // Project and rasterize the triangles.
        image<number_type> image(data.width, data.height);
        raster<std::vector<coord2<number_type>>> samples = get_samples(data.width, data.height,
                                                                       data.samples_per_pixel,
                                                                       std::shared_ptr<generator>(new regular_sample_2d<number_type>()));
        //			std::shared_ptr<generator>(new jitter_sample_2d<number_type>()));

        std::cout << "rasterizing with antialiasing" << std::endl;
        rasterize_triangles(data, image, samples);
        std::cout << "Writing data to file.." << std::endl;
        save_image(string_format("cs5600_program04_antialiased-%1spp-%2.ppm", samples(0, 0).size(), data.gamma), image, data.gamma);
        save_image(string_format("cs5600_program04_antialiased-%1spp-%2.ppm", samples(0, 0).size(), 1.7), image, 1.7);

        data.samples_per_pixel = 64;
        samples = get_samples(data.width, data.height,
                              data.samples_per_pixel,
                              std::shared_ptr<generator>(new regular_sample_2d<number_type>()));
        std::cout << "rasterizing with antialiasing" << std::endl;
        rasterize_triangles(data, image, samples);
        std::cout << "Writing data to file.." << std::endl;
        save_image(string_format("cs5600_program04_antialiased-%1spp-%2.ppm", samples(0, 0).size(), data.gamma), image, data.gamma);
        save_image(string_format("cs5600_program04_antialiased-%1spp-%2.ppm", samples(0, 0).size(), 1.7), image, 1.7);
        data.samples_per_pixel = 256;
        samples = get_samples(data.width, data.height,
                              data.samples_per_pixel,
                              std::shared_ptr<generator>(new regular_sample_2d<number_type>()));
        std::cout << "rasterizing with antialiasing" << std::endl;
        rasterize_triangles(data, image, samples);
        std::cout << "Writing data to file.." << std::endl;
        save_image(string_format("cs5600_program04_antialiased-%1spp-%2.ppm", samples(0, 0).size(), data.gamma), image, data.gamma);
        save_image(string_format("cs5600_program04_antialiased-%1spp-%2.ppm", samples(0, 0).size(), 1.7), image, 1.7);

        std::cout << "rasterizing" << std::endl;
        samples = get_samples(data.width, data.height, 1);
        rasterize_triangles(data, image, samples);
        std::cout << "Writing data to file.." << std::endl;
        save_image(string_format("cs5600_program04-%1spp-%2.ppm", samples(0, 0).size(), data.gamma), image, data.gamma);
        save_image(string_format("cs5600_program04-%1spp-%2.ppm", samples(0, 0).size(), 1.7), image, 1.7);
    }
    else
    {
        std::cout << string_format("Could not read file \"%1\"", filename) << std::endl;
        return 1;
    }
    return 0;
}
