#pragma once

/*
   samplegen2d.h

   A set of classes to generate random samples.
 */

/*
   Revision history:
   19jan1999 Copied this file from the 2d samplegen.  Renamed all classes in it
   to make sure that they don't conflict with other versions that I
   have.
   19jan1999 Added the regular_sample_3d class.
   20jan1999 Added the jitter_sample_3d class.
   Added the poisson_sample_3d class.
   Changed everything from using a pair of doubles to using my coord2 class.
   08Apr2000 Changed the include of random to be in utils, and coord2 to be in graph_math
   25May2001 Changed this class to be templated, moving the members back in here
   from the samplegen3d.cpp file.
   16May2004 Changed the names from 3d to 2d.  Added a clone function.
   Added nrooks and multi-jitter sample generators.
   06Apr2018 Refactored a bit, made more C++11-ish.
 */

#include "amethyst/general/random.hpp"
#include "amethyst/math/coord2.hpp"
#include <algorithm>
#include <vector>
#include <memory>

namespace amethyst
{

    template <class T>
    class sample_generator_2d
    {
    public:
        using sample_output_fn = std::function<void(coord2<T>)>;
        using random_type = random<T>;

        sample_generator_2d(const random_type& r = default_random<T>()) : sample_generator_2d(r.clone_new()) { }
        sample_generator_2d(std::shared_ptr<random_type> r) : rand_gen(std::move(r)) { }
        sample_generator_2d(const sample_generator_2d<T>& gen) : rand_gen(gen.rand_gen->clone_new()) { }
        virtual ~sample_generator_2d() = default;

        virtual std::vector<coord2<T>> get_samples(size_t num_samples) = 0;
        virtual void get_samples(size_t num_samples, sample_output_fn pf)
        {
            std::vector<coord2<T>> v = get_samples(num_samples);
            for (auto s : v)
            {
                pf(s);
            }
        }
        virtual std::unique_ptr<sample_generator_2d<T>> clone_new() const = 0;

        const random_type& get_rand_gen() const { return *rand_gen; }
        void set_rand_gen(const random_type& r) { rand_gen = r.clone_new(); }

    protected:
        coord2<T> next_rand() { return { rand_gen->next(), rand_gen->next() }; }
        T next_fp_rand() { return rand_gen->next(); }
        uint32_t next_int_rand() { return rand_gen->next_int(); };

    private:
        std::shared_ptr<random_type> rand_gen;
    };

    template <class T>
    class random_sample_2d : public sample_generator_2d<T>
    {
    public:
        using parent = sample_generator_2d<T>;

        random_sample_2d(const random_type& r = default_random<T>()) : parent(r) {}
        virtual ~random_sample_2d() = default;

        std::vector<coord2<T>> get_samples(size_t num_samples) override
        {
            std::vector<coord2<T>> v;
            v.reserve(num_samples);

            for (size_t i = 0; i < num_samples; ++i)
            {
                v.emplace_back(sample_generator_2d<T>::next_rand());
            }
            return v;
        }

        void get_samples(size_t num_samples, sample_output_fn pf) override
        {
            for (size_t i = 0; i < num_samples; ++i)
            {
                pf(sample_generator_2d<T>::next_rand());
            }
        }

        std::unique_ptr<sample_generator_2d<T>> clone_new() const override
        {
            return std::make_unique<random_sample_2d<T>>(*this);
        }
    };

    template <class T>
    class regular_sample_2d : public sample_generator_2d<T>
    {
    public:
        using parent = sample_generator_2d<T>;

        regular_sample_2d(const random_type& r = default_random<T>())
            : parent(r)
        {
        }
        virtual ~regular_sample_2d() = default;

        using parent::get_samples;
        std::vector<coord2<T>> get_samples(size_t num_samples) override
        {
            size_t sample_height = size_t(sqrt(num_samples));
            size_t sample_width = num_samples / sample_height;
            T scalarY = NEAR_ONE / T(sample_height - 1);
            T scalarX = NEAR_ONE / T(sample_width - 1);

            std::vector<coord2<T>> v;
            v.reserve(num_samples);

            for (size_t y = 0; y < sample_height; ++y)
            {
                for (size_t x = 0; x < sample_width; ++x)
                {
                    v.emplace_back(coord2<T>(x * scalarX, y * scalarY));
                }
            }
            return v;
        }

        std::unique_ptr<sample_generator_2d<T>> clone_new() const override
        {
            return std::make_unique<regular_sample_2d<T>>(*this);
        }
    };

    /*
       This is a jittered version of the N-Rooks problem.
       For each sample, it will have a unique range in X and Y that no other
       sample can occupy.
       See this horrible ascii drawing for an example:
     |   | x |   |   |   |
     |   |   |   |   | x |
     |   |   | x |   |   |
     | x |   |   |   |   |
     |   |   |   | x |   |
     */
    template <class T>
    class nrooks_sample_2d : public sample_generator_2d<T>
    {
    public:
        using parent = sample_generator_2d<T>;

        nrooks_sample_2d(const random_type& r = default_random<T>())
            : parent(r)
        {
        }
        virtual ~nrooks_sample_2d() = default;

        using parent::get_samples;
        std::vector<coord2<T>> get_samples(size_t num_samples) override
        {
            std::vector<coord2<T>> vec(num_samples);
            for (int i = 0; i < num_samples; ++i)
            {
                coord2<T> p = sample_generator_2d<T>::next_rand();
                vec[i].set((p.x() + i) / double(num_samples),
                    (p.y() + i) / double(num_samples));
            }
            // shuffle the X coordinates.
            if (num_samples > 1)
            {
                for (size_t i = num_samples - 1; i > 0; --i)
                {
                    // Question: What is slower, a FP mul + integer convert, or a integer
                    // mod?  Some places say that the FP mul gives better uniformity.
                    int target = int(sample_generator_2d<T>::next_fp_rand() * i);
                    std::swap(vec[i].x(), vec[target].x());
                }
            }
            return vec;
        }

        std::unique_ptr<sample_generator_2d<T>> clone_new() const override
        {
            return std::make_unique<nrooks_sample_2d<T>>(*this);
        }
    };

    /* Basically the same as the regular samples, but they are randomized within the block */
    template <class T>
    class jitter_sample_2d : public sample_generator_2d<T>
    {
    public:
        using parent = sample_generator_2d<T>;
        jitter_sample_2d(const random_type& r = default_random<T>())
            : parent(r)
        {
        }
        virtual ~jitter_sample_2d() = default;

        using parent::get_samples;
        std::vector<coord2<T>> get_samples(size_t num_samples) override
        {
            // This arrangement tends to favor width over height.  As most images are
            // wider than they are tall, this is not a bad thing.  If an even square
            // is used, they will be equal.
            size_t sample_height = size_t(sqrt(T(num_samples)));
            size_t sample_width = num_samples / sample_height;
            std::vector<coord2<T>> v;
            v.reserve(num_samples);

            for (size_t y = 0; y < sample_height; ++y)
            {
                for (size_t x = 0; x < sample_width; ++x)
                {
                    coord2<T> p = sample_generator_2d<T>::next_rand();
                    v.emplace_back(coord2<T>((x + p.x()) / T(sample_width),
                        (y + p.y()) / T(sample_height)));
                }
            }
            return v;
        }

        std::unique_ptr<sample_generator_2d<T>> clone_new() const override
        {
            return std::make_unique<jitter_sample_2d<T>>(*this);
        }
    };

    template <class T>
    class multi_jitter_sample_2d : public sample_generator_2d<T>
    {
    public:
        using parent = sample_generator_2d<T>;

        multi_jitter_sample_2d(const random_type& r = default_random<T>())
            : parent(r)
        {
        }
        virtual ~multi_jitter_sample_2d() = default;

        using parent::get_samples;
        std::vector<coord2<T>> get_samples(size_t num_samples) override
        {
            int sqrt_samples = int(sqrt(num_samples));
            int adjusted_samples = sqrt_samples * sqrt_samples;
            // Force it to be a perfect square.
            if (adjusted_samples != num_samples)
            {
                ++sqrt_samples;
                num_samples = (sqrt_samples * sqrt_samples);
            }

            std::vector<coord2<T>> vec(num_samples);

            T subcell_width = T(1.0) / T(num_samples);

            // Initialize points to the "canonical" multi-jittered pattern
            for (int y = 0; y < sqrt_samples; ++y)
            {
                int linear_y = y * sqrt_samples;
                for (int x = 0; x < sqrt_samples; ++x)
                {
                    coord2<T> p = sample_generator_2d<T>::next_rand();
                    vec[linear_y + x].set(subcell_width * (x + p.x() + linear_y),
                        subcell_width * (y + p.y() + x * sqrt_samples));
                }
            }

            // Shuffle x coords within each column (x) and y coords within each row (y).
            // Note: This shares a loop, to avoid doubling the number of loops required.
            for (int y = 0; y < sqrt_samples; ++y)
            {
                int linear_y = y * sqrt_samples;
                for (int current = sqrt_samples - 1; current > 0; --current)
                {
                    coord2<T> p = sample_generator_2d<T>::next_rand();

                    // Shuffle the y coordinates along the row.
                    int target_x = int(p.x() * current);
                    std::swap(vec[linear_y + current].y(),
                        vec[linear_y + target_x].y());

                    // Shuffle the x coordinates along the column.
                    int target_y = int(p.y() * current);
                    // This is just here to make the swap code look like it came from a
                    // proper loop (in x) -- improves readability.
                    int x = y;
                    std::swap(vec[current * sqrt_samples + x].x(),
                        vec[target_y * sqrt_samples + x].x());
                }
            }
            return vec;
        }

        std::unique_ptr<sample_generator_2d<T>> clone_new() const override
        {
            return std::make_unique<multi_jitter_sample_2d<T>>(*this);
        }
    };

    /* A class for Poisson Disc sampling */
    template <class T>
    class poisson_sample_2d : public sample_generator_2d<T>
    {
    public:
        using parent = sample_generator_2d<T>;
        poisson_sample_2d(const random_type& r = default_random<T>(), T distance = 0.1)
            : parent(r)
            , distance_between_samples(distance)
        {
        }
        virtual ~poisson_sample_2d() = default;

        using parent::get_samples;
        std::vector<coord2<T>> get_samples(size_t num_samples) override
        {
            std::vector<coord2<T>> samples(num_samples);
            int samples_gathered = 0;
            int current_sample;
            int bad_attempts = 0;
            int restarts = 0;
            coord2<T> next_point;

            while (samples_gathered < num_samples)
            {
                next_point = sample_generator_2d<T>::next_rand();
                samples[samples_gathered] = next_point;
                for (current_sample = 0; current_sample < samples_gathered; ++current_sample)
                {
                    if (length(next_point - samples[current_sample]) < distance_between_samples)
                    {
                        ++bad_attempts;
                        break; /* Samples too close, can't use it. */
                    }
                }
                /* If it made it all the way through the for loop, use the sample. */
                if (current_sample == samples_gathered)
                {
                    ++samples_gathered;
                    bad_attempts = 0;
                }
                else if (bad_attempts > num_samples)
                {
                    samples_gathered = 0;
                    ++restarts;
                    if (restarts > num_samples / 2)
                    {
                        std::cerr << "Distance " << distance_between_samples << " is too large for the sample count (" << num_samples << ").  Lowering." << std::endl;
                        distance_between_samples *= 0.9; // decrease the distance
                        restarts = 0;
                    }
                }
            }
            return samples;
        }

        std::unique_ptr<sample_generator_2d<T>> clone_new() const override
        {
            return std::make_unique<poisson_sample_2d<T>>(*this);
        }
    private:
        T distance_between_samples;
    };
}
