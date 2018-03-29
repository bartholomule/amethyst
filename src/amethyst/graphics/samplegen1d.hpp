/*
 * $Id: samplegen1d.hpp,v 1.3 2008/06/21 22:25:10 kpharris Exp $
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

#if    !defined(AMETHYST__SAMPLEGEN_1D_HPP)
#define         AMETHYST__SAMPLEGEN_1D_HPP

/*
   samplegen1d.hpp

   A set of classes to generate random samples.
 */

/*
   Revision history:
   17jan1999 Created this file.
   19jan1999 Added virtual destructors to the sample_generator and random_sample
   classes.
   19jan1999 Renamed and moved this file (old name was samplegen.h).  Renamed
   all classes in it to make sure that they don't conflict with other
   versions that I will need later.
   19jan1999 Added the regular_sample_2d class.
   20jan1999 Added the jitter_sample_2d class.
   Added the poisson_sample_2d class.
   25jan1999 Changed the include of random to look in the parent directory.
   08Apr2000 Changed the include of random to look in <utils/xxx>
   25May2001 Changed this class to be templated, moving the members back in here
   from the samplegen2d.cpp file.
   16May2004 Changed the names from 2d to 1d.  Added a clone function.
 */

#include <vector>
#include "amethyst/general/random.hpp"

namespace amethyst
{
    template <class T>
    class sample_generator_1d
    {
    public:
        sample_generator_1d(const Random<T>& r = default_random<T>());
        sample_generator_1d(const sample_generator_1d<T>& gen) :
            rand_gen(gen.rand_gen->clone_new())
        {
        }
        virtual ~sample_generator_1d() {
            delete rand_gen;
        }

        virtual std::vector<T> get_samples(int num_samples) = 0;
        virtual void get_samples(int num_samples, void (*pf)(T)) = 0;

        template <class operation>
        void filter_samples(int num_samples, operation& o);

        const Random<T>& get_rand_gen() const {
            return *rand_gen;
        }

        virtual sample_generator_1d<T>* clone_new() const = 0;

    protected:
        T next_rand();
    private:
        Random<T>* rand_gen;
    };

    template <class T>
    class random_sample_1d : public sample_generator_1d<T>
    {
    public:
        random_sample_1d(const Random<T>& r = default_random<T>());
        virtual ~random_sample_1d() {
        }
        virtual std::vector<T> get_samples(int num_samples);
        virtual void get_samples(int num_samples, void (*pf)(T));
        virtual random_sample_1d<T>* clone_new() const;
    };

    template <class T>
    class regular_sample_1d : public sample_generator_1d<T>
    {
    public:
        regular_sample_1d(const Random<T>& r = default_random<T>());
        virtual ~regular_sample_1d() {
        }
        virtual std::vector<T> get_samples(int num_samples);
        virtual void get_samples(int num_samples, void (*pf)(T));
        virtual regular_sample_1d<T>* clone_new() const;
    };

    template <class T>
    class jitter_sample_1d : public sample_generator_1d<T>
    {
    public:
        jitter_sample_1d(const Random<T>& r = default_random<T>());
        virtual ~jitter_sample_1d() {
        }
        virtual std::vector<T> get_samples(int num_samples);
        virtual void get_samples(int num_samples, void (*pf)(T));
        virtual jitter_sample_1d<T>* clone_new() const;
    };


    /* A class for Poisson sampling (the equiv of the disc sampling with fewer dimensions) */
    template <class T>
    class poisson_sample_1d : public sample_generator_1d<T>
    {
    public:
        poisson_sample_1d(const Random<T>& r = default_random<T>(),
                          T distance = 0.1);
        virtual ~poisson_sample_1d() {
        }
        virtual std::vector<T> get_samples(int num_samples);
        virtual void get_samples(int num_samples, void (*pf)(T));
        virtual poisson_sample_1d<T>* clone_new() const;
    private:
        T distance_between_samples;
    };


    /* sample_generator stuff */
    template <class T>
    sample_generator_1d<T>::sample_generator_1d(const Random<T>& r) :
        rand_gen(r.clone_new())
    {
    }

    template <class T>
    T sample_generator_1d<T>::next_rand()
    {
        return rand_gen->next();
    }

    template <class T>
    template <class operation>
    void sample_generator_1d<T>::filter_samples(int num_samples, operation& o)
    {
        const std::vector<T> v = get_samples(num_samples);
        for (typename std::vector<T>::const_iterator iter = v.begin();
             iter != v.end();
             ++iter)
        {
            o(*iter);
        }
    }

    /* sample_generator stuff */
    template <class T>
    random_sample_1d<T>::random_sample_1d(const Random<T>& r) :
        sample_generator_1d<T>(r)
    {
    }

    template <class T>
    std::vector<T> random_sample_1d<T>::get_samples(int num_samples)
    {
        std::vector<T> v;
        int i;

        for (i = 0; i < num_samples; ++i)
        {
            v.push_back(sample_generator_1d<T>::next_rand());
        }
        return v;
    }

    template <class T>
    void random_sample_1d<T>::get_samples(int num_samples, void (*pf)(T))
    {
        int i;
        for (i = 0; i < num_samples; ++i)
        {
            pf(sample_generator_1d<T>::next_rand());
        }
    }

    template <class T>
    random_sample_1d<T>* random_sample_1d<T>::clone_new() const
    {
        return new random_sample_1d<T>(*this);
    }

    /* regular_generator stuff */
    template <class T>
    regular_sample_1d<T>::regular_sample_1d(const Random<T>& r) :
        sample_generator_1d<T>(r)
    {
    }

    template <class T>
    std::vector<T> regular_sample_1d<T>::get_samples(int num_samples)
    {
        const T scalar = NEAR_ONE / T(num_samples - 1);
        std::vector<T> v;
        for (int x = 0; x < num_samples; ++x)
        {
            v.push_back(x * scalar);
        }
        return v;
    }

    template <class T>
    void regular_sample_1d<T>::get_samples(int num_samples, void (*pf)(T))
    {
        const T scalar = NEAR_ONE / T(num_samples - 1);
        for (int x = 0; x < num_samples; ++x)
        {
            pf(x * scalar);
        }
    }

    template <class T>
    regular_sample_1d<T>* regular_sample_1d<T>::clone_new() const
    {
        return new regular_sample_1d<T>(*this);
    }

    /* jitter sample stuff */
    template <class T>
    jitter_sample_1d<T>::jitter_sample_1d(const Random<T>& r) :
        sample_generator_1d<T>(r)
    {
    }

    template <class T>
    std::vector<T> jitter_sample_1d<T>::get_samples(int num_samples)
    {
        int x;
        std::vector<T> v;
        for (x = 0; x < num_samples; ++x)
        {
            v.push_back((x + sample_generator_1d<T>::next_rand()) / T(num_samples));
        }
        return v;
    }

    template <class T>
    void jitter_sample_1d<T>::get_samples(int num_samples, void (*pf)(T))
    {
        int x;
        for (x = 0; x < num_samples; ++x)
        {
            pf((x + sample_generator_1d<T>::next_rand()) / T(num_samples));
        }
    }

    template <class T>
    jitter_sample_1d<T>* jitter_sample_1d<T>::clone_new() const
    {
        return new jitter_sample_1d<T>(*this);
    }

    /* stuff for the poisson class */
    template <class T>
    poisson_sample_1d<T>::poisson_sample_1d(const Random<T>& r,
                                            T distance) :
        sample_generator_1d<T>(r), distance_between_samples(distance)
    {
    }

    template <class T>
    std::vector<T> poisson_sample_1d<T>::get_samples(int num_samples)
    {
        std::vector<T> samples(num_samples);
        int samples_gathered = 0;
        int current_sample;
        T next_point;

        while (samples_gathered < num_samples)
        {
            next_point = sample_generator_1d<T>::next_rand();
            samples[samples_gathered] = next_point;
            for (current_sample = 0; current_sample < samples_gathered; ++current_sample)
            {
                if (fabs(next_point - samples[current_sample]) < distance_between_samples) {
                    break; /* Samples too close, can't use it. */
                }
            }
            /* If it made it all the way through the for loop, use the sample. */
            if (current_sample == samples_gathered) {
                ++samples_gathered;
            }
        }
        return samples;
    }

    template <class T>
    void poisson_sample_1d<T>::get_samples(int num_samples, void (*pf)(T))
    {
        std::vector<T> v = get_samples(num_samples);
        for (typename std::vector<T>::iterator vp = v.begin();
             vp != v.end();
             ++vp)
        {
            pf(*vp);
        }
    }

    template <class T>
    poisson_sample_1d<T>* poisson_sample_1d<T>::clone_new() const
    {
        return new poisson_sample_1d<T>(*this);
    }

} // namespace amethyst

#endif /* !defined(AMETHYST__SAMPLEGEN_1D_HPP) */
