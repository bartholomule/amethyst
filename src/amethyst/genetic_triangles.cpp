#define KH_USE_TIME_FOR_RANDOM

#include "amethyst/graphics/image.hpp"
#include "amethyst/graphics/image_loader.hpp"
#include <vector>
#include "amethyst/general/string_format.hpp"
#include "amethyst/general/random.hpp"
#include "amethyst/graphics/alpha_triangle_2d.hpp"

#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include <map>

using namespace amethyst;

using number_type = double;
// using number_type = float;
using color = rgbcolor<number_type>;
using alpha_triangle = alpha_triangle_2d<number_type>;

struct globals
{
    number_type birth_rate;
    number_type death_rate;
    number_type mutation_rate;
    size_t images_per_generation;
    std::string population_filename;
    size_t min_triangles;
    size_t max_triangles;
    size_t generation_for_max;
    size_t crossover_points;
    size_t num_offspring;
    size_t min_population_size;
    size_t max_population_size;
    size_t width;
    size_t height;
    std::shared_ptr<image_io<number_type>> io;
} GLOBALS;

// Add the term to the current sum, add the dropped bits to the correction
// term, and return the new sum
template <typename T>
inline T corrected_sum(T current_sum, T term, T& correction)
{
    volatile T sum1 = current_sum + term;
    // In an ideal world, this would be zero: (x - (x + t) + t)
    correction += (current_sum - sum1) + term;
    return sum1;
}

void rasterize_triangles(std::vector<alpha_triangle> triangles, image<number_type>& img)
{
    // Blank the image...
    size_t num_pixels = img.get_width() * img.get_height();
    color* pixels = img.reinterpret<color*>();
    for (size_t i = 0; i < num_pixels; ++i)
    {
        pixels[i].set(0, 0, 0);
    }

    // Draw the triangles
    for (std::vector<alpha_triangle>::const_iterator iter = triangles.begin();
         iter != triangles.end();
         ++iter)
    {
        dda_rasterize_triangle(img, *iter);
    }
}

void generate_random_triangles(std::vector<alpha_triangle>& triangles,
                               size_t count, int width, int height, amethyst::random<number_type>& r)
{
    triangles.clear();
    for (size_t i = 0; i < count; ++i)
    {
        alpha_triangle t;
        t.v1.rgb = color(r.next(), r.next(), r.next());
        t.v1.xy = coord2<number_type>(r.next() * width, r.next() * height);
        t.v1.a = r.next();
        t.v2.rgb = color(r.next(), r.next(), r.next());
        t.v2.xy = coord2<number_type>(r.next() * width, r.next() * height);
        t.v2.a = r.next();
        t.v3.rgb = color(r.next(), r.next(), r.next());
        t.v3.xy = coord2<number_type>(r.next() * width, r.next() * height);
        t.v3.a = r.next();

        triangles.push_back(t);
    }
}

inline color squared_difference(const color& c1, const color& c2)
{
    /*
       return color(
        ((c1.r() - c2.r()) * (c1.r() - c2.r())),
        ((c1.g() - c2.g()) * (c1.g() - c2.g())),
        ((c1.b() - c2.b()) * (c1.b() - c2.b())));
     */
    return color(
        fabs(c1.r() - c2.r()),
        fabs(c1.g() - c2.g()),
        fabs(c1.b() - c2.b()));
}

number_type calculate_error(const image<number_type>& ref, const image<number_type>& img)
{
    number_type error(0);
    number_type addition_error(0);

    size_t width = ref.get_width();
    size_t height = ref.get_height();

    const color* ref_pixels = ref.raw_data();
    const color* img_pixels = img.raw_data();

    size_t max_pixel = width * height;
    for (size_t i = 0; i < max_pixel; ++i)
    {
        color difference = 256 * squared_difference(ref_pixels[i], img_pixels[i]);
        error = corrected_sum(error,
                              difference.r() + difference.g() + difference.b(),
                              addition_error);
    }
    return error + addition_error;
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
            for (size_t i = 0; i < 18; ++i)
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
            for (size_t i = 0; i < 18; ++i)
            {
                raw_data[i] = e.raw_data[i];
            }
            return *this;
        }
    };

    std::vector<pleb_data_entry> pleb_data;

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

typedef std::vector<pleb> population;


std::ostream& operator<<(std::ostream& o, const pleb& p)
{
    o << p.pleb_data.size() << " " << p.error;
    for (size_t i = 0; i < p.pleb_data.size(); ++i)
    {
        for (size_t j = 0; j < 18; ++j)
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

    for (size_t i = 0; i < p.pleb_data.size(); ++i)
    {
        for (size_t j = 0; j < 18; ++j)
        {
            input >> p.pleb_data[i].raw_data[j];
        }
    }
    return input;
}


void convert_to_triangles(pleb& p, std::vector<alpha_triangle>& output, size_t image_width, size_t image_height)
{
    size_t triangles = p.pleb_data.size();
    output.resize(triangles);
    for (size_t i = 0; i < triangles; ++i)
    {
        output[i].v1.rgb.set( p.pleb_data[i].pleb_verts[0].r, p.pleb_data[i].pleb_verts[0].g, p.pleb_data[i].pleb_verts[0].b);
        output[i].v1.xy.set( p.pleb_data[i].pleb_verts[0].x / 100.0 * image_width, p.pleb_data[i].pleb_verts[0].y / 100.0 * image_height);
        output[i].v1.a = p.pleb_data[i].pleb_verts[0].a;

        output[i].v2.rgb.set( p.pleb_data[i].pleb_verts[1].r, p.pleb_data[i].pleb_verts[1].g, p.pleb_data[i].pleb_verts[1].b);
        output[i].v2.xy.set( p.pleb_data[i].pleb_verts[1].x / 100.0 * image_width, p.pleb_data[i].pleb_verts[1].y / 100.0 * image_height);
        output[i].v2.a = p.pleb_data[i].pleb_verts[1].a;

        output[i].v3.rgb.set( p.pleb_data[i].pleb_verts[2].r, p.pleb_data[i].pleb_verts[2].g, p.pleb_data[i].pleb_verts[2].b);
        output[i].v3.xy.set(p.pleb_data[i].pleb_verts[2].x / 100.0 * image_width, p.pleb_data[i].pleb_verts[2].y / 100.0 * image_height);
        output[i].v3.a = p.pleb_data[i].pleb_verts[2].a;
    }
}


void shuffle_range(std::vector<pleb>& data, size_t index1, size_t index2, amethyst::random<number_type>& rnd)
{
    if (!data.empty())
    {
        //		std::cout << "Shuffling between " << index1 << " and " << index2 << std::endl;

        size_t current = data.size() - 1;
        while (current > 1)
        {
            size_t swap_number = size_t(rnd.next_int(uint32_t(current)));

            for (size_t index = index1; index < index2; ++index)
            {
                std::swap(data[current].pleb_data[index / 18].raw_data[index % 18], data[swap_number].pleb_data[index / 18].raw_data[index % 18]);
            }
            --current;
        }
    }
}

void unique_random_list(std::vector<size_t>& output, size_t min_value, size_t max_value, size_t count, amethyst::random<number_type>& rnd)
{
    output.clear();
    output.reserve(count);

    if (max_value < count)
    {
        throw std::runtime_error("output size is too large for the max value");
    }

    const size_t range = max_value - min_value;

    while (output.size() < count)
    {
        size_t next_loc = min_value + rnd.next_int(uint32_t(range));
        if (std::find(output.begin(), output.end(), next_loc) == output.end())
        {
            output.push_back(next_loc);
        }
    }
}

void random_cross(const pleb& p1, const pleb& p2, size_t points, size_t count, std::vector<pleb>& output, amethyst::random<number_type>& rnd)
{
    //	std::cout << "Generating random cross..." << std::endl;
    output.clear();
    output.resize(count);

    // Generate some random crossover points, all unique.
    std::vector<size_t> point_locations;
    point_locations.reserve(points);
    const size_t num_data_points = p1.pleb_data.size() * 18;
    //	std::cout << "Generating " << points << " points between 1 and " << num_data_points << std::endl;

    unique_random_list(point_locations, 1, num_data_points, points, rnd);
    std::sort(point_locations.begin(), point_locations.end());

    // Populate the output with the existing plebs...  Evenly divided.
    size_t index;
    for (index = 0; index < output.size() / 2; ++index)
    {
        output[index] = p1;
    }
    for (; index < output.size(); ++index)
    {
        output[index] = p2;
    }

    // Shuffle them between the cross points
    size_t last_index = 0;
    for (std::vector<size_t>::const_iterator iter = point_locations.begin(); iter != point_locations.end(); ++iter)
    {
        shuffle_range(output, last_index, *iter, rnd);
        last_index = *iter;
    }
    shuffle_range(output, last_index, num_data_points, rnd);
}

pleb::pleb_data_entry create_random_triangle(amethyst::random<number_type>& rnd)
{
    pleb::pleb_data_entry e;
    for (size_t t = 0; t < 3; ++t)
    {
        e.pleb_verts[t].x = rnd.next_int(100);
        e.pleb_verts[t].y = rnd.next_int(100);
        e.pleb_verts[t].r = rnd.next();
        e.pleb_verts[t].g = rnd.next();
        e.pleb_verts[t].b = rnd.next();
        e.pleb_verts[t].a = rnd.next();
    }
    return e;
}

void random_pleb(size_t count, amethyst::random<number_type>& rnd, pleb& retval)
{
    retval.pleb_data.resize(count);
    for (size_t i = 0; i < count; ++i)
    {
        retval.pleb_data[i] = create_random_triangle(rnd);
    }
}

void generate_random_population(size_t width, size_t height, amethyst::random<number_type>& rnd, size_t count, size_t triangles, population& output)
{
    output.clear();
    output.resize(count);

    for (size_t i = 0; i < count; ++i)
    {
        random_pleb(triangles, rnd, output[i]);
    }
}

struct pleb_data
{
    size_t pleb_index;
    number_type error;
    amethyst::image<number_type> image;
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
    std::vector<pleb_data>& best,
    std::vector<pleb_data>& worst)
{
    if (best.empty())
    {
        best.push_back(data);
    }
    else
    {
        best.push_back(data);
        std::sort(best.begin(), best.end(), &pleb_is_better);
        if (best.size() > count_of_each)
        {
            best.erase(best.begin() + count_of_each, best.end());
        }
    }
    if (worst.empty())
    {
        worst.push_back(data);
    }
    else
    {
        worst.push_back(data);
        std::sort(worst.begin(), worst.end(), &pleb_is_worse);
        if (worst.size() > count_of_each)
        {
            worst.erase(worst.begin() + count_of_each, worst.end());
        }
    }

}

void calculate_population_error(population& populous, const image<number_type>& reference,
                                size_t count_of_each,
                                std::vector<pleb_data>& best,
                                std::vector<pleb_data>& worst)
{
    size_t width = reference.get_width();
    size_t height = reference.get_height();

    best.clear();
    worst.clear();

    std::cout << "Converting and rasterizing...";
    for (size_t i = 0; i < populous.size(); ++i)
    {
        pleb_data data;
        data.pleb_index = i;
        data.image = image<number_type>(width, height);

        std::vector<alpha_triangle> triangles;
        std::cout << "...#" << i << " (" << populous[i].pleb_data.size() << ")" << std::flush;
        convert_to_triangles(populous[i], triangles, width, height);
        rasterize_triangles(triangles, data.image);
        data.error = calculate_error(reference, data.image);
        populous[i].error = data.error;
        add_to_statistics(data, count_of_each, best, worst);
    }
    std::cout << std::endl;
}

image<number_type> get_error_image(const image<number_type>& img1, const image<number_type>& img2)
{
    image<number_type> retval(img1.get_width(), img1.get_height());

    const color* in_colors1 = img1.raw_data();
    const color* in_colors2 = img2.raw_data();
    color* out_pixels = retval.reinterpret<color*>();

    color max_color = color(-100, -100, -100);
    color error_sum = color(0, 0, 0);

    size_t num_pixels = retval.get_width() * retval.get_height();

    for (size_t i = 0; i < num_pixels; ++i)
    {
        color difference = squared_difference(in_colors1[i], in_colors2[i]);

        max_color.set_r(std::max(max_color.r(), difference.r()));
        max_color.set_g(std::max(max_color.g(), difference.g()));
        max_color.set_b(std::max(max_color.b(), difference.b()));

        error_sum += difference;

        out_pixels[i] = difference;
    }

    const color average = error_sum * (1 / number_type(num_pixels));
    const color scale(1 / max_color.r(), 1 / max_color.g(), 1 / max_color.b());
    const color desired_ave_color(0.5, 0.5, 0.5);
    number_type gamma_r = std::log(average.r()) / std::log(desired_ave_color.r());
    number_type gamma_g = std::log(average.g()) / std::log(desired_ave_color.g());
    number_type gamma_b = std::log(average.b()) / std::log(desired_ave_color.b());

    std::cout << "average color=" << average << " max color=" << max_color << " gamma=" << gamma_r << "," << gamma_g << "," << gamma_b << std::endl;

    for (size_t i = 0; i < num_pixels; ++i)
    {
        color& pixel = out_pixels[i];
        // Scale the color and gamma convert it.
        pixel.set_r(std::pow(pixel.r() * scale.r(), 1 / gamma_r));
        pixel.set_g(std::pow(pixel.g() * scale.g(), 1 / gamma_g));
        pixel.set_b(std::pow(pixel.b() * scale.b(), 1 / gamma_b));
    }
    return retval;
}

void run_generation(population& populous, const image<number_type>& reference,
                    std::vector<pleb_data>& best,
                    std::vector<pleb_data>& worst,
                    size_t count_of_each,
                    size_t images_to_dump,
                    size_t generation_number,
                    number_type gamma,
                    image_io<number_type>& io)
{
    std::cout << "Running generation #" << generation_number << std::endl;
    calculate_population_error(populous, reference, count_of_each, best, worst);

    for (size_t i = 0; i < images_to_dump; ++i)
    {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "%07zd", generation_number);

        // Dump the best image...
        std::cout << "Generation " << buffer << ": Best error #" << i << " = " << std::setprecision(3) << std::fixed << best[i].error << " (" << best[i].pleb_index << ")" << std::endl;
        io.output(string_format("genetic_triangles_best-%1-%2.%3", buffer, i, io.default_extension()), best[i].image, gamma);

        // Dump the worst image...
        std::cout << "Generation " << buffer << ": Worst error #" << i << " = " << std::setprecision(3) << std::fixed << worst[i].error << " (" << worst[i].pleb_index << ")" << std::endl;
        io.output(string_format("genetic_triangles_worst-%1-%2.%3", buffer, i, io.default_extension()), worst[i].image, gamma);

        // Dump the error image...
        image<number_type> error = get_error_image(reference, best[i].image);
        io.output(string_format("genetic_triangles_error-%1-%2.%3", buffer, i, io.default_extension()), error, gamma);
    }

    // Dump the reference image (for debugging purposes)
    io.output("genetic_triangles_reference." + io.default_extension(), reference, gamma);
}

void write_population(const std::string& filename, const population& populous, size_t generation)
{
    std::ofstream output(filename.c_str());
    output << generation << "\n";
    output << " " << populous.size() << "\n";
    for (population::const_iterator i = populous.begin(); i != populous.end(); ++i)
    {
        output << " " << *i << "\n";
    }
    output.flush();
}

bool read_population(const std::string& filename, population& populous, size_t& generation)
{
    std::ifstream input(filename.c_str());

    input >> generation;

    size_t size;

    input >> size;

    populous.resize(size);

    for (population::iterator i = populous.begin(); i != populous.end(); ++i)
    {
        if (!(input >> *i))
        {
            return false;
        }
    }
    return true;
}

void mutate_plebs(population& populous, number_type mutation_rate, amethyst::random<number_type>& rnd, size_t max_width, size_t max_height)
{
    std::cout << "Mutating children... ";
    for (size_t i = 0; i < populous.size(); ++i)
    {
        if (rnd.next() < mutation_rate)
        {
            std::cout << i << " ";
            // Mutate me...
            size_t data_points = populous[i].pleb_data.size() * 18;
            size_t spot = rnd.next_int(uint32_t(data_points));
            size_t triangle = spot / 18;
            size_t raw_offset = spot % 18;

            switch (raw_offset % 6)
            {
            case 0: // x
                populous[i].pleb_data[triangle].raw_data[raw_offset] = rnd.next_int(uint32_t(max_width));
                break;
            case 1: // y
                populous[i].pleb_data[triangle].raw_data[raw_offset] = rnd.next_int(uint32_t(max_height));
                break;
            default: // r, g, b, a
                populous[i].pleb_data[triangle].raw_data[raw_offset] = rnd.next();
            }
        }
    }
    std::cout << std::endl;
}

typedef void (*generation_tweaker)(population& populous, size_t generation, size_t width, size_t height, amethyst::random<number_type>& rnd);
typedef void (*generation_crosser)(population& populous, size_t generation, const std::vector<pleb_data>& best, const std::vector<pleb_data>& worst, size_t width, size_t height, amethyst::random<number_type>& rnd);

void run_for_generations(population& populous, const image<number_type>& reference,
                         size_t starting_generation,
                         size_t generations,
                         number_type gamma,
                         amethyst::random<number_type>& rnd,
                         image_io<number_type>& io,
                         generation_crosser cross_generation,
                         generation_tweaker modify_generation)
{
    const size_t width = reference.get_width();
    const size_t height = reference.get_height();

    for (size_t generation = starting_generation; generation < generations; ++generation)
    {
        std::vector<pleb_data> best;
        std::vector<pleb_data> worst;

        size_t amount_to_cross = size_t(1) << GLOBALS.crossover_points;

        run_generation(populous, reference, best, worst, amount_to_cross, GLOBALS.images_per_generation, generation, gamma, io);

        if ((generation + 1) < generations)
        {
            std::cout << "best[" << generation << "] =";
            for (size_t i = 0; i < best.size(); ++i)
            {
                std::cout << " " << best[i].pleb_index;
            }
            std::cout << std::endl;
            std::cout << "worst[" << generation << "] =";
            for (size_t i = 0; i < worst.size(); ++i)
            {
                std::cout << " " << worst[i].pleb_index;
            }
            std::cout << std::endl;
            // Cross over..
            cross_generation(populous, generation, best, worst, width, height, rnd);

            // Do any non-crossing modifications (such as adding new genes).
            modify_generation(populous, generation, width, height, rnd);
        }
        write_population(GLOBALS.population_filename, populous, generation);
    }
}

void cross_best_replace_worst(population& populous, size_t generation, const std::vector<pleb_data>& best, const std::vector<pleb_data>& worst, size_t crossover_points, number_type mutation_rate, size_t width, size_t height, amethyst::random<number_type>& rnd)
{
    population crossed;
    std::cout << "Crossing " << best[0].pleb_index << " with " << best[1].pleb_index << std::endl;
    random_cross(populous[best[0].pleb_index], populous[best[1].pleb_index], crossover_points, (size_t(1) << crossover_points), crossed, rnd);

    // Mutate and repopulate...
    mutate_plebs(crossed, mutation_rate, rnd, width, height);

    // Replace the worst...
    for (size_t i = 0; i < crossed.size(); ++i)
    {
        populous[worst[i].pleb_index] = crossed[i];
    }
}

size_t triangles_at_generation(size_t generation)
{
    number_type difference = number_type(GLOBALS.max_triangles - GLOBALS.min_triangles);
    number_type rate = number_type(GLOBALS.max_triangles - GLOBALS.min_triangles) / GLOBALS.generation_for_max;
    number_type expected = GLOBALS.min_triangles + generation * rate;

    std::cout << string_format("Generation %1 should have %2 triangles (max=%3, rate=%4)", generation, expected, GLOBALS.max_triangles, rate) << std::endl;

    return std::min<size_t>(GLOBALS.max_triangles, size_t(expected));
}

void cross_best_with_random(population& populous, size_t generation, const std::vector<pleb_data>& best, const std::vector<pleb_data>& worst, size_t width, size_t height, amethyst::random<number_type>& rnd)
{
    population crossed;
    const pleb best_pleb = populous[best[0].pleb_index];

    // Cross a bunch of plebs...
    size_t num_to_cross = size_t(GLOBALS.birth_rate * populous.size());
    if ((num_to_cross * GLOBALS.num_offspring) > GLOBALS.max_population_size)
    {
        // Replace the entire population.
        num_to_cross = GLOBALS.max_population_size / GLOBALS.num_offspring;
    }
    else if (num_to_cross * GLOBALS.num_offspring < (GLOBALS.max_population_size - GLOBALS.num_offspring))
    {
        if (rnd.next() < GLOBALS.birth_rate)
        {
            // Add one more...
            ++num_to_cross;
        }
    }

    for (size_t count = 0; count < num_to_cross; ++count)
    {
        // Yes, this could end up doing some cloning, and it won't matter -- odds are better with a smaller population size.
        size_t random_guy = rnd.next_int(uint32_t(populous.size()));
        std::cout << "Crossing " << best[0].pleb_index << " with random guy " << random_guy << std::endl;

        population crossing;
        random_cross(best_pleb, populous[random_guy], GLOBALS.crossover_points, GLOBALS.num_offspring, crossing, rnd);

        crossed.insert(crossed.end(), crossing.begin(), crossing.end());
    }

    // Mutate...
    mutate_plebs(crossed, GLOBALS.mutation_rate, rnd, width, height);

    // Kill some.
    size_t num_to_kill = size_t(GLOBALS.death_rate * populous.size());

    // Clamp the population size.
    if ((populous.size() + crossed.size() - num_to_kill) > GLOBALS.max_population_size)
    {
        num_to_kill = std::min(populous.size(), populous.size() + crossed.size() - num_to_kill - GLOBALS.max_population_size);
    }

    std::cout << "Killing " << num_to_kill << " plebs (out of " << populous.size() << "):";
    std::vector<size_t> death_list;
    unique_random_list(death_list, 0, populous.size(), num_to_kill, rnd);
    std::sort(death_list.begin(), death_list.end(), std::greater<size_t>());
    for (size_t i = 0; i < death_list.size(); ++i)
    {
        size_t guy_to_kill = death_list[i];
        std::cout << " " << guy_to_kill;
        populous.erase(populous.begin() + guy_to_kill);
    }
    std::cout << std::endl;

    // Replace some random plebs, not just the bad guys.
    std::cout << "Adding " << crossed.size() << " plebs." << std::endl;
    populous.insert(populous.end(), crossed.begin(), crossed.end());

    // Come back to the minimum population size.
    if (populous.size() < GLOBALS.min_population_size)
    {
        size_t triangles = triangles_at_generation(generation);
        while (populous.size() < GLOBALS.min_population_size)
        {
            population::value_type pleb;
            random_pleb(triangles, rnd, pleb);
            populous.push_back(pleb);
        }
    }
}

void leave_alone(population& populous, size_t generation, size_t width, size_t height, amethyst::random<number_type>& rnd)
{
    // Do nothing
}

void add_occasional_triangles(population& populous, size_t generation, size_t width, size_t height, amethyst::random<number_type>& rnd)
{
    size_t triangles = triangles_at_generation(generation);

    if (populous.empty())
    {
        populous.push_back(pleb(triangles));
    }

    for (population::iterator i = populous.begin(); i != populous.end(); ++i)
    {
        if (i->pleb_data.size() < triangles)
        {
            i->pleb_data.push_back(create_random_triangle(rnd));
        }
    }
}

enum parser_option_argtype
{
    E_OPT_ARG_NONE, E_OPT_ARG_REQUIRED, E_OPT_ARG_OPTIONAL
};

struct parser_option
{
    int opt_id;
    char opt_short;
    std::string opt_long;
    parser_option_argtype arg_type;
    std::string default_value;
    std::string argname;
    std::string description;
};

bool long_opt_equals(std::string name, const parser_option& opt)
{
    return (!opt.opt_long.empty()) && (opt.opt_long == name);
}

bool short_opt_equals(char c, const parser_option& opt)
{
    return (opt.opt_short != '\0') && (opt.opt_short == c);
}

class cmdline_parser
{
public:

    int add_optional_arg(const std::string& longopt, char shortopt, const std::string& default_value, const std::string& description, const std::string& argname = "arg")
    {
        parser_option opt;
        opt.opt_id = int(m_valid_options.size());
        opt.opt_short = shortopt;
        opt.opt_long = longopt;
        opt.arg_type = E_OPT_ARG_OPTIONAL;
        opt.default_value = default_value;
        opt.description = description;
        opt.argname = argname;
        m_valid_options.push_back(opt);

        return opt.opt_id;
    }

    int add_required_arg(const std::string& longopt, char shortopt, const std::string& description, const std::string& argname = "arg" )
    {
        parser_option opt;
        opt.opt_id = int(m_valid_options.size());
        opt.opt_short = shortopt;
        opt.opt_long = longopt;
        opt.arg_type = E_OPT_ARG_REQUIRED;
        opt.description = description;
        opt.argname = argname;
        m_valid_options.push_back(opt);

        return opt.opt_id;
    }

    int add_argless(const std::string& longopt, char shortopt, const std::string& description)
    {
        parser_option opt;
        opt.opt_id = int(m_valid_options.size());
        opt.opt_short = shortopt;
        opt.opt_long = longopt;
        opt.arg_type = E_OPT_ARG_NONE;
        opt.description = description;
        m_valid_options.push_back(opt);

        return opt.opt_id;
    }

    int find_by_name(const std::string& longopt) const
    {
        std::vector<parser_option>::const_iterator loc;
        for (loc = m_valid_options.begin(); loc != m_valid_options.end(); ++loc)
        {
            if (long_opt_equals(longopt, *loc))
            {
                break;
            }
        }

        if (loc != m_valid_options.end())
        {
            return loc->opt_id;
        }
        return -1;
    }

    int find_by_name(char shortopt) const
    {
        std::vector<parser_option>::const_iterator loc;
        for (loc = m_valid_options.begin(); loc != m_valid_options.end(); ++loc)
        {
            if (short_opt_equals(shortopt, *loc))
            {
                break;
            }
        }

        if (loc != m_valid_options.end())
        {
            return loc->opt_id;
        }
        return -1;
    }

    bool opt_was_supplied(int id)
    {
        return m_parsed_options.find(id) != m_parsed_options.end();
    }

    bool opt_was_supplied(const std::string& name)
    {
        return opt_was_supplied(find_by_name(name));
    }

    std::string get_option_value(int id, const std::string& default_if_none)
    {
        value_map::const_iterator iter = m_parsed_options.find(id);
        if ((iter != m_parsed_options.end()) && !iter->second.empty())
        {
            return *iter->second.rbegin();
        }
        return default_if_none;
    }

    std::string get_option_value(const std::string& name, const std::string& default_if_none)
    {
        return get_option_value(find_by_name(name), default_if_none);
    }

    // Don't pass argv[0] in here.
    template <typename iter_t>
    bool parse(iter_t start, iter_t finish, std::vector<std::string>& error_messages)
    {
        bool retval = true;
        error_messages.clear();
        m_parsed_options.clear();
        size_t index = 0;
        for (; start != finish; ++start)
        {
            ++index;
            std::string opt = *start;
            std::string attached_value;

            std::string::size_type equals = opt.find('=');
            if (equals != std::string::npos)
            {
                if ((equals + 1) < opt.size())
                {
                    attached_value = opt.substr(equals + 1);
                }
                opt = opt.substr(0, equals);
            }

            char shortopt = '\0';
            std::string longopt;

            if ((opt.size() > 0) && (opt[0] == '-'))
            {
                // Could be short, could be long.
                int id = -1;

                if (opt.size() == 2)
                {
                    // Short opt, or --.
                    shortopt = opt[1];

                    id = find_by_name(shortopt);
                }
                else if ((opt.size() > 2) && (opt[1] == '-'))
                {
                    longopt = opt.substr(2);

                    id = find_by_name(longopt);
                }


                if (id != -1)
                {
                    // We have a valid option.

                    const parser_option& po = m_valid_options[id];

                    if (po.arg_type == E_OPT_ARG_NONE)
                    {
                        if (equals == std::string::npos)
                        {
                            // Some random operation to create an entry for the matching ID.
                            m_parsed_options[id].clear();
                        }
                        else
                        {
                            error_messages.push_back(string_format("Option %1 (%2) illegally had an attached argument (\"%3\")", index, std::string(*start).substr(0, equals), attached_value));
                            retval = false;
                        }
                    }
                    else
                    {
                        // optional or required.
                        if (equals != std::string::npos)
                        {
                            m_parsed_options[id].push_back(attached_value);
                        }
                        else
                        {
                            // Not attached, check the next option.
                            if (start + 1 != finish)
                            {
                                std::string next = *(start + 1);
                                if ((next.size() <= 1) || (next[0] != '-'))
                                {
                                    m_parsed_options[id].push_back(next);
                                    ++start;
                                    ++index;
                                }
                                else if (po.arg_type == E_OPT_ARG_OPTIONAL)
                                {
                                    m_parsed_options[id].push_back(po.default_value);
                                }
                                else
                                {
                                    error_messages.push_back(string_format("Option %1 (%2) requires an argument", index, *start));
                                    retval = false;
                                }
                            }
                            else if (po.arg_type == E_OPT_ARG_OPTIONAL)
                            {
                                m_parsed_options[id].push_back(po.default_value);
                            }
                            else
                            {
                                error_messages.push_back(string_format("Option %1 (%2) requires an argument", index, *start));
                                retval = false;
                            }
                        }
                    }
                }
                else
                {
                    error_messages.push_back(string_format("Option %1 (%2) is invalid in \"%3\"", index, std::string(*start).substr(0, equals), std::string(*start)));
                    retval = false;
                }
            }
            else
            {
                error_messages.push_back(string_format("Non-option argument %1 is invalid: \"%2\"", index, *start));
                retval = false;
            }
        }
        return retval;
    }

    std::string get_help() const
    {
        std::string retval;
        const size_t column_indent = 20;
        const size_t line_width = 80;

        retval += "Valid Options:\n";
        for (std::vector<parser_option>::const_iterator opt = m_valid_options.begin();
             opt != m_valid_options.end();
             ++opt)
        {
            std::string opt_help;
            std::string def;
            if (opt->opt_short != '\0')
            {
                opt_help.append(1, '-').append(1, opt->opt_short);
            }
            if (!opt->opt_long.empty())
            {
                if (!opt_help.empty())
                {
                    opt_help.append(", ");
                }
                opt_help.append("--").append(opt->opt_long);
            }
            if (opt->arg_type == E_OPT_ARG_REQUIRED)
            {
                opt_help.append(" <").append(opt->argname).append(1, '>');
            }
            else if (opt->arg_type == E_OPT_ARG_OPTIONAL)
            {
                opt_help.append(" [").append(opt->argname).append(1, ']');
                def = " (default is \"" + opt->default_value + "\")";
            }

            if (opt_help.size() >= column_indent)
            {
                opt_help.append(1, '\n');
            }
            else
            {
                opt_help.append(std::max<int>(1, column_indent - int(opt_help.size())), ' ');
            }
            opt_help.append(opt->description);
            opt_help.append(def);

            // Break up the line and append it to the running output.
            while ((opt_help.size() > line_width) || (opt_help.find('\n') != std::string::npos))
            {
                size_t newline_pos = opt_help.find('\n');
                size_t space_pos = opt_help.rfind(' ', line_width);

                size_t hardbreak = newline_pos;
                if (newline_pos > line_width)
                {
                    hardbreak = space_pos;
                }
                if ((hardbreak < column_indent) || (hardbreak == std::string::npos))
                {
                    // Just split at the column width because we found no spaces
                    retval.append(opt_help, 0, line_width - 1).append("-\n");
                    opt_help = std::string(column_indent, ' ') + opt_help.substr(line_width - 1);
                }
                else
                {
                    retval.append(opt_help, 0, hardbreak).append(1, '\n');
                    if ((hardbreak + 1) < opt_help.size())
                    {
                        opt_help = std::string(column_indent, ' ') + opt_help.substr(hardbreak + 1);
                    }
                    else
                    {
                        opt_help.erase();
                    }
                }
            }
            retval.append(opt_help).append(1, '\n');
        }

        return retval;
    }

private:
    std::vector<parser_option> m_valid_options;
    typedef std::map<int, std::vector<std::string>> value_map;
    value_map m_parsed_options;
};

int string_to_int(const std::string& s)
{
    const char* sp = s.c_str();
    const char* end = sp;

    int retval = strtol(sp, (char**)&end, 10);

    if ((end - sp) != s.length())
    {
        throw std::runtime_error("invalid integer");
    }
    return retval;
}

double string_to_double(const std::string& s)
{
    const char* sp = s.c_str();
    const char* end = sp;

    double retval = strtod(sp, (char**)&end);

    if ((end - sp) != s.length())
    {
        throw std::runtime_error("invalid floating point number");
    }
    return retval;
}

image<number_type> scale_image(const image<number_type>& img, size_t width, size_t height, size_t spp = 1)
{
    image<number_type> dest(width, height);

    size_t source_width = img.get_width();
    size_t source_height = img.get_height();

    number_type y_factor = source_height / number_type(height);
    number_type x_factor = source_width / number_type(width);
    number_type half_spp = number_type(spp) / 2;

    for (unsigned y = 0; y < height; ++y)
    {
        for (unsigned x = 0; x < width; ++x)
        {
            color target(0, 0, 0);
            for (unsigned yspp = 0; yspp < spp; ++yspp)
            {
                number_type yshift = yspp - half_spp;
                size_t source_ypos = std::min<size_t>(source_height - 1, std::max<int>(0, int(y_factor * (y + yshift) + 0.5)));

                for (unsigned xspp = 0; xspp < spp; ++xspp)
                {
                    number_type xshift = xspp - half_spp;
                    size_t source_xpos = std::min<size_t>(source_width - 1, std::max<int>(0, int(x_factor * (x + xshift) + 0.5)));

                    target += img(source_xpos, source_ypos);
                }
            }
            target /= number_type(spp * spp);
            dest(x, y) = target;
        }
    }

    return dest;
}

bool parse_command_line(int argc, const char** argv,
                        int& num_generations, bool& resume, std::string& input_file)
{
    cmdline_parser parser;
    parser.add_argless("help", 'h', "Show this help text");
    parser.add_argless("continue", 'c', "Continue with saved population");
    parser.add_required_arg("input-image", 'i', "Load image from <filename>", "filename");
    parser.add_optional_arg("min-population-size", 'p', "300", "Set initial (and minimum) population size to [size]", "size");
    parser.add_optional_arg("max-population-size", '\0', "300", "Set the the maximum population [size].  See --birth-rate.", "size");
    parser.add_optional_arg("population-file", 'o', "genetic_triangles.population", "Write the population to [filename] at the end of each generation. If the --continue option is supplied as well, the population and starting generation will also be loaded from [filename]. ", "filename");
    parser.add_optional_arg("max-generation", 'm', "1000000", "Run until generation [generation] is reached", "generation");
    parser.add_optional_arg("image-count", '\0', "1", "Save [count] best and worst images at the end of each generation", "count");
    parser.add_optional_arg("min-triangles", '\0', "1", "Set the minimum number of triangles (starting triangles) to [count]", "count");
    parser.add_optional_arg("max-triangles", '\0', "300", "Set the maximum number of triangles (ending triangles) to [count]", "count");
    parser.add_optional_arg("reach-max-at", '\0', "10000", "Reach the maximum number of triangles at generation [generation]", "generation");
    parser.add_required_arg("crossover-points", '\0', "Use <points> different points when doing a genetic cross.  You probably do not want to change this.", "points");
    parser.add_optional_arg("death-rate", '\0', "0.1", "The [rate] of population decrease each generation.", "rate");
    parser.add_optional_arg("birth-rate", '\0', "0.1", "The [rate] of population increase each generation.  If this is larger than the death rate, the population size will increase until the --max-population-size value is reached and euthanasia is forced", "rate");
    parser.add_optional_arg("mutation-rate", 'r', "1.0", "Set the probability of offspring mutation to [rate].\nSupplying 1.0 means every child will mutate\n", "rate");
    parser.add_optional_arg("children", '\0', "3", "Set the number of children per crossing to [children].  Must be 2 or larger.", "children");
    parser.add_optional_arg("width", '\0', "0", "Set the output image width", "width");
    parser.add_optional_arg("height", '\0', "0", "Set the output image height", "height");

    bool retval = true;

    std::vector<std::string> errs;
    if (!parser.parse(argv + std::min(1, argc), argv + argc, errs))
    {
        std::cerr << "Failed to parse: " << std::endl;
        for (size_t i = 0; i < errs.size(); ++i)
        {
            std::cerr << "  " << errs[i] << std::endl;
        }
        retval = false;
    }

    if (parser.opt_was_supplied("help") || !retval)
    {
        std::cerr << parser.get_help();
        return retval;
    }

    // Do all other option manipulation here.
    input_file = parser.get_option_value("input-image", "");
    if (input_file.empty())
    {
        std::cerr << "No input image filename was supplied." << std::endl;
        retval = false;
    }
    num_generations = string_to_int(parser.get_option_value("max-generations", "1000000"));
    resume = parser.opt_was_supplied("continue");
    GLOBALS.io = getImageLoader<number_type>(input_file);
    GLOBALS.min_population_size = string_to_int(parser.get_option_value("min-population-size", "300"));
    GLOBALS.max_population_size = string_to_int(parser.get_option_value("max-population-size", "300"));
    GLOBALS.mutation_rate = string_to_double(parser.get_option_value("mutation-rate", "1.0"));
    GLOBALS.birth_rate = string_to_double(parser.get_option_value("birth-rate", "0.1"));
    GLOBALS.death_rate = string_to_double(parser.get_option_value("death-rate", "0.1"));
    GLOBALS.images_per_generation = string_to_int(parser.get_option_value("image-count", "1"));
    GLOBALS.population_filename = parser.get_option_value("population-file", "genetic_triangles.population");
    GLOBALS.min_triangles = string_to_int(parser.get_option_value("min-triangles", "1"));
    GLOBALS.max_triangles = string_to_int(parser.get_option_value("max-triangles", "300"));
    GLOBALS.generation_for_max = string_to_int(parser.get_option_value("reach-max-at", "10000"));
    GLOBALS.num_offspring = string_to_int(parser.get_option_value("children", "3"));
    GLOBALS.width = string_to_int(parser.get_option_value("width", "0"));
    GLOBALS.height = string_to_int(parser.get_option_value("height", "0"));

    if (parser.opt_was_supplied("crossover-points"))
    {
        GLOBALS.crossover_points = string_to_int(parser.get_option_value("crossover-points", ""));
    }
    else
    {
        GLOBALS.crossover_points = size_t(log(GLOBALS.max_population_size / 2.0) / log(2.0));
    }

    return retval;
}

int main(int argc, const char** argv)
try
{
    int num_generations;
    bool resume;
    std::string image_filename;

    if (!parse_command_line(argc, argv, num_generations, resume, image_filename))
    {
        return 1;
    }

    image<number_type> reference = GLOBALS.io->input(image_filename);

    const size_t ref_width = reference.get_width();
    const size_t ref_height = reference.get_height();
    const number_type gamma = 1;
    size_t starting_generation = 1;

    if ((ref_width <= 1) || (ref_height <= 1))
    {
        std::cerr << "Bad image file." << std::endl;
        return 1;
    }

    if (GLOBALS.width == 0 || GLOBALS.height == 0)
    {
        GLOBALS.width = ref_width;
        GLOBALS.height = ref_height;
    }

    // Scale the image to match the global size...
    if (GLOBALS.width != ref_width || GLOBALS.height != ref_height)
    {
        reference = scale_image(reference, GLOBALS.width, GLOBALS.height);
    }

    mersenne_twist_random<number_type> rnd;

    population populous;

    generate_random_population(GLOBALS.width, GLOBALS.height, rnd,
                               GLOBALS.min_population_size, GLOBALS.min_triangles, populous);

    if (resume)
    {
        std::cout << "continuing..." << std::endl;
        if (!read_population(GLOBALS.population_filename, populous, starting_generation))
        {
            std::cerr << "failed to read population..." << std::endl;
            return 1;
        }
        std::cout << "Continuing with generation " << ++starting_generation << std::endl;
    }

    run_for_generations(populous, reference,
                        starting_generation, num_generations,
                        gamma, rnd, *GLOBALS.io, &cross_best_with_random, &add_occasional_triangles);

    return 0;
}
catch (const std::runtime_error& e)
{
    std::cerr << "Exception escaped: " << e.what() << std::endl;
    return 1;
}
