/*
 * $Id: samplegen2d.hpp,v 1.5 2008/06/21 22:25:10 kpharris Exp $
 *
 * Part of "Amethyst" a playground for graphics development
 * Copyright (C) 2004 Kevin Harris
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#if    !defined(AMETHYST__SAMPLEGEN_2D_HPP)
#define         AMETHYST__SAMPLEGEN_2D_HPP

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
 */

#include <algorithm>
#include "amethyst/general/random.hpp"
#include <vector>
#include <memory>
#include "amethyst/math/coord2.hpp"

namespace amethyst
{

    template <class T>
    class sample_generator_2d
    {
    public:
        sample_generator_2d(const Random<T>& r = default_random<T>());
        sample_generator_2d(std::shared_ptr<Random<T>>& r) : rand_gen(r) {
        }
        sample_generator_2d(const sample_generator_2d<T>& gen) : rand_gen(gen.rand_gen->clone_new()) {
        }
        virtual ~sample_generator_2d() {
        }

        virtual std::vector<coord2<T>> get_samples(int num_samples) = 0;
        virtual void get_samples(int num_samples, void (*pf)(const coord2<T>&)) = 0;
        const Random<T>& get_rand_gen() const {
            return *rand_gen;
        }
        void set_rand_gen(const Random<T>& r) {
            rand_gen.reset(r.clone_new());
        }

        template <class operation>
        void filter_samples(int num_samples, operation& o);

        virtual sample_generator_2d<T>* clone_new() const = 0;

    protected:
        coord2<T> next_rand();
        T next_fp_rand();
        unsigned long next_int_rand();
    private:
        std::shared_ptr<Random<T>> rand_gen;
    };

    template <class T>
    class random_sample_2d : public sample_generator_2d<T>
    {
    public:
        random_sample_2d(const Random<T>& r = default_random<T>());
        virtual ~random_sample_2d() {
        }
        virtual std::vector<coord2<T>> get_samples(int num_samples);
        virtual void get_samples(int num_samples, void (*pf)(const coord2<T>&));

        virtual random_sample_2d<T>* clone_new() const;
    };

    template <class T>
    class regular_sample_2d : public sample_generator_2d<T>
    {
    public:
        regular_sample_2d(const Random<T>& r = default_random<T>());
        virtual ~regular_sample_2d() {
        }
        virtual std::vector<coord2<T>> get_samples(int num_samples);
        virtual void get_samples(int num_samples, void (*pf)(const coord2<T>&));

        virtual regular_sample_2d<T>* clone_new() const;
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
        nrooks_sample_2d(const Random<T>& r = default_random<T>());
        virtual ~nrooks_sample_2d() {
        }
        virtual std::vector<coord2<T>> get_samples(int num_samples);
        virtual void get_samples(int num_samples, void (*pf)(const coord2<T>&));

        virtual nrooks_sample_2d<T>* clone_new() const;
    };

    /* Basically the same as the regular samples, but they are randomized within the block */
    template <class T>
    class jitter_sample_2d : public sample_generator_2d<T>
    {
    public:
        jitter_sample_2d(const Random<T>& r = default_random<T>());
        virtual ~jitter_sample_2d() {
        }
        virtual std::vector<coord2<T>> get_samples(int num_samples);
        virtual void get_samples(int num_samples, void (*pf)(const coord2<T>&));

        virtual jitter_sample_2d<T>* clone_new() const;
    };

    template <class T>
    class multi_jitter_sample_2d :  public sample_generator_2d<T>
    {
    public:
        multi_jitter_sample_2d(const Random<T>& r = default_random<T>());
        virtual ~multi_jitter_sample_2d() {
        }
        virtual std::vector<coord2<T>> get_samples(int num_samples);
        virtual void get_samples(int num_samples, void (*pf)(const coord2<T>&));

        virtual multi_jitter_sample_2d<T>* clone_new() const;
    };

    /* A class for Poisson Disc sampling */
    template <class T>
    class poisson_sample_2d : public sample_generator_2d<T>
    {
    public:
        poisson_sample_2d(const Random<T>& r = default_random<T>(),
                          T distance = 0.1);
        virtual ~poisson_sample_2d() {
        }
        virtual std::vector<coord2<T>> get_samples(int num_samples);
        virtual void get_samples(int num_samples, void (*pf)(const coord2<T>&));

        virtual poisson_sample_2d<T>* clone_new() const;
    private:
        T distance_between_samples;
    };

    /* sample_generator stuff */
    template <class T>
    sample_generator_2d<T>::sample_generator_2d(const Random<T>& r) :
        rand_gen(r.clone_new())
    {
    }

    template <class T>
    coord2<T> sample_generator_2d<T>::next_rand()
    {
        return coord2<T>(rand_gen->next(), rand_gen->next());
    }

    template <class T>
    T sample_generator_2d<T>::next_fp_rand()
    {
        return rand_gen->next();
    }

    template <class T>
    unsigned long sample_generator_2d<T>::next_int_rand()
    {
        return rand_gen->next_int();
    }

    template <class T>
    template <class operation>
    void sample_generator_2d<T>::filter_samples(int num_samples, operation& o)
    {
        std::vector<coord2<T>> v = get_samples(num_samples);
        for (typename std::vector<coord2<T>>::const_iterator iter = v.begin();
             iter != v.end();
             ++iter)
        {
            o(*iter);
        }
    }

    /* random_sample stuff */
    template <class T>
    random_sample_2d<T>::random_sample_2d(const Random<T>& r) :
        sample_generator_2d<T>(r)
    {
    }

    template <class T>
    std::vector<coord2<T>> random_sample_2d<T>::get_samples(int num_samples)
    {
        std::vector<coord2<T>> v;
        int i;

        for (i = 0; i < num_samples; ++i)
        {
            v.push_back(sample_generator_2d<T>::next_rand());
        }
        return v;
    }

    template <class T>
    void random_sample_2d<T>::get_samples(int num_samples, void (*pf)(const coord2<T>&))
    {
        int i;
        for (i = 0; i < num_samples; ++i)
        {
            pf(sample_generator_2d<T>::next_rand());
        }
    }

    template <class T>
    random_sample_2d<T>* random_sample_2d<T>::clone_new() const
    {
        return new random_sample_2d<T>(*this);
    }

    /* regular_sample stuff */
    template <class T>
    regular_sample_2d<T>::regular_sample_2d(const Random<T>& r) :
        sample_generator_2d<T>(r)
    {
    }

    template <class T>
    std::vector<coord2<T>> regular_sample_2d<T>::get_samples(int num_samples)
    {
        int x, y;
        int sample_height = (int)sqrt(num_samples);
        int sample_width = num_samples / sample_height;
        T scalarY = NEAR_ONE / T(sample_height - 1);
        T scalarX = NEAR_ONE / T(sample_width - 1);

        std::vector<coord2<T>> v;

        for (y = 0; y < sample_height; ++y)
        {
            for (x = 0; x < sample_width; ++x)
            {
                v.push_back(coord2<T>(x * scalarX, y * scalarY));
            }
        }
        return v;
    }

    template <class T>
    void regular_sample_2d<T>::get_samples(int num_samples, void (*pf)(const coord2<T>&))
    {
        int x, y;
        int sample_height = (int)sqrt(num_samples);
        int sample_width = num_samples / sample_height;
        T scalarY = NEAR_ONE / T(sample_height - 1);
        T scalarX = NEAR_ONE / T(sample_width - 1);

        for (y = 0; y < sample_height; ++y)
        {
            for (x = 0; x < sample_width; ++x)
            {
                pf(coord2<T>(x * scalarX, y * scalarY));
            }
        }
    }

    template <class T>
    regular_sample_2d<T>* regular_sample_2d<T>::clone_new() const
    {
        return new regular_sample_2d<T>(*this);
    }

    /* nrooks sample stuff */
    template <class T>
    nrooks_sample_2d<T>::nrooks_sample_2d(const Random<T>& r) :
        sample_generator_2d<T>(r)
    {
    }

    template <class T>
    std::vector<coord2<T>> nrooks_sample_2d<T>::get_samples(int num_samples)
    {
        std::vector<coord2<T>> vec(num_samples);
        for (int i = 0; i < num_samples; ++i)
        {
            coord2<T> p = sample_generator_2d<T>::next_rand();
            vec[i].set((p.x() + i) / double(num_samples),
                       (p.y() + i) / double(num_samples));
        }
        // shuffle the X coordinates.
        for (int i = num_samples - 1; i > 0; --i)
        {
            // Question: What is slower, a FP mul + integer convert, or a integer
            // mod?  Some places say that the FP mul gives better uniformity.
            int target = int(sample_generator_2d<T>::next_fp_rand() * i);
            std::swap(vec[i].x(), vec[target].x());
        }
        return vec;
    }

    template <class T>
    void nrooks_sample_2d<T>::get_samples(int num_samples, void (*pf)(const coord2<T>&))
    {
        std::vector<coord2<T>> vec = get_samples(num_samples);
        for (typename std::vector<coord2<T>>::const_iterator iter = vec.begin();
             iter != vec.end();
             ++iter)
        {
            pf(*iter);
        }
    }

    template <class T>
    nrooks_sample_2d<T>* nrooks_sample_2d<T>::clone_new() const
    {
        return new nrooks_sample_2d<T>(*this);
    }

    /* jitter_sample stuff */
    template <class T>
    jitter_sample_2d<T>::jitter_sample_2d(const Random<T>& r) :
        sample_generator_2d<T>(r)
    {
    }

    template <class T>
    std::vector<coord2<T>> jitter_sample_2d<T>::get_samples(int num_samples)
    {
        int x, y;
        // This arrangement tends to favor width over height.  As most images are
        // wider than they are tall, this is not a bad thing.  If an even square
        // is used, they will be equal.
        int sample_height = (int)sqrt(T(num_samples));
        int sample_width = num_samples / sample_height;
        std::vector<coord2<T>> v;

        for (y = 0; y < sample_height; ++y)
        {
            for (x = 0; x < sample_width; ++x)
            {
                coord2<T> p = sample_generator_2d<T>::next_rand();
                v.push_back(coord2<T>((x + p.x()) / T(sample_width),
                                      (y + p.y()) / T(sample_height)));
            }
        }
        return v;
    }

    template <class T>
    void jitter_sample_2d<T>::get_samples(int num_samples,
                                          void (*pf)(const coord2<T>&))
    {
        int x, y;
        // See the comments about favorint width in the vector version.
        int sample_height = (int)sqrt(T(num_samples));
        int sample_width = num_samples / sample_height;
        for (y = 0; y < sample_height; ++y)
        {
            for (x = 0; x < sample_width; ++x)
            {
                coord2<T> p = sample_generator_2d<T>::next_rand();
                pf(coord2<T>((x + p.x()) / T(sample_width),
                             (y + p.y()) / T(sample_height)));
            }
        }
    }

    template <class T>
    jitter_sample_2d<T>* jitter_sample_2d<T>::clone_new() const
    {
        return new jitter_sample_2d<T>(*this);
    }

    template <class T>
    multi_jitter_sample_2d<T>::multi_jitter_sample_2d(const Random<T>& r) :
        sample_generator_2d<T>(r)
    {
    }

    template <class T>
    std::vector<coord2<T>> multi_jitter_sample_2d<T>::get_samples(int num_samples)
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
                std::swap( vec[linear_y + current].y(),
                           vec[linear_y + target_x].y());

                // Shuffle the x coordinates along the column.
                int target_y = int(p.y() * current);
                // This is just here to make the swap code look like it came from a
                // proper loop (in x) -- improves readability.
                int x = y;
                std::swap( vec[current * sqrt_samples + x].x(),
                           vec[target_y * sqrt_samples + x].x());
            }
        }
        return vec;
    }

    template <class T>
    void multi_jitter_sample_2d<T>::get_samples(int num_samples, void (*pf)(const coord2<T>&))
    {
        std::vector<coord2<T>> vec = get_samples(num_samples);

        for (typename std::vector<coord2<T>>::const_iterator iter = vec.begin();
             iter != vec.end();
             ++iter)
        {
            pf(*iter);
        }
    }

    template <class T>
    multi_jitter_sample_2d<T>* multi_jitter_sample_2d<T>::clone_new() const
    {
        return new multi_jitter_sample_2d<T>(*this);
    }

    /* stuff for poisson disc sampling */
    template <class T>
    poisson_sample_2d<T>::poisson_sample_2d(const Random<T>& r, T distance) :
        sample_generator_2d<T>(r), distance_between_samples(distance)
    {
    }

    template <class T>
    std::vector<coord2<T>> poisson_sample_2d<T>::get_samples(int num_samples)
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
                    fflush(stdout);
                }
            }
        }
        return samples;
    }

    template <class T>
    void poisson_sample_2d<T>::get_samples(int num_samples, void (*pf)(const coord2<T>&))
    {
        std::vector<coord2<T>> v = get_samples(num_samples);
        for (typename std::vector<coord2<T>>::iterator vp = v.begin();
             vp != v.end();
             ++vp)
        {
            pf(*vp);
        }
    }

    template <class T>
    poisson_sample_2d<T>* poisson_sample_2d<T>::clone_new() const
    {
        return new poisson_sample_2d<T>(*this);
    }

} // namespace amethyst

#endif /* !defined(AMETHYST__SAMPLEGEN_2D_HPP) */
