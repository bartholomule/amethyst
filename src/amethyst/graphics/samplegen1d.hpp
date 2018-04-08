#pragma once
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
#include "amethyst/general/random.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace amethyst
{
    template <class T>
    class sample_generator_1d
    {
    public:
        using sample_output_fn = std::function<void(T)>;
        using random_type = random<T>;

        sample_generator_1d(const random_type& r = default_random<T>()) : rand_gen(r.clone_new()) { }
        sample_generator_1d(std::shared_ptr<random_type> r) : rand_gen(std::move(r)) { }
        sample_generator_1d(const sample_generator_1d&) = default;
        virtual ~sample_generator_1d() = default;

        virtual std::vector<T> get_samples(int num_samples) = 0;
        virtual void get_samples(int num_samples, sample_output_fn pf) = 0;
        virtual std::unique_ptr<sample_generator_1d<T>> clone_new() const = 0;

        const random_type& get_rand_gen() const { return *rand_gen; }

    protected:
        T next_rand() { return rand_gen->next(); }

    private:
        std::shared_ptr<random_type> rand_gen;
    };

    template <class T>
    class random_sample_1d : public sample_generator_1d<T>
    {
    public:
        random_sample_1d(const random_type& r = default_random<T>())
            : sample_generator_1d(r)
        {
        }
        virtual ~random_sample_1d() = default;
        std::vector<T> get_samples(int num_samples) override;
        void get_samples(int num_samples, sample_output_fn pf) override;
        std::unique_ptr<random_sample_1d<T>> clone_new() const override
        {
            return std::make_unique<random_sample1d<T>>(*this);
        }
    };

    template <class T>
    class regular_sample_1d : public sample_generator_1d<T>
    {
    public:
        regular_sample_1d(const random_type& r = default_random<T>())
            : sample_generator_1d(r)
        {
        }
        virtual ~regular_sample_1d() = default;
        std::vector<T> get_samples(int num_samples) override;
        void get_samples(int num_samples, sample_output_fn pf) override;
        std::unique_ptr<regular_sample_1d<T>> clone_new() const override
        {
            return std::make_unique<regular_sample_1d<T>>(*this);
        }
    };

    template <class T>
    class jitter_sample_1d : public sample_generator_1d<T>
    {
    public:
        jitter_sample_1d(const random_type& r = default_random<T>())
            : sample_generator_1d(r)
        {
        }
        virtual ~jitter_sample_1d() = default;
        std::vector<T> get_samples(int num_samples) override;
        void get_samples(int num_samples, sample_output_fn pf) override;
        std::unique_ptr<sample_generator_1d<T>> clone_new() const override
        {
            return std::make_unique<jitter_sample_1d<T>>(*this);
        }
    };


    /* A class for Poisson sampling (the equiv of the disc sampling with fewer dimensions) */
    template <class T>
    class poisson_sample_1d : public sample_generator_1d<T>
    {
    public:
        poisson_sample_1d(const random_type& r = default_random<T>(), T distance = 0.1)
            : sample_generator_1d(r)
            , distance_between_samples(distance)
        {
        }
        virtual ~poisson_sample_1d() = default;
        std::vector<T> get_samples(int num_samples) override;
        void get_samples(int num_samples, sample_output_fn pf) override;
        std::unique_ptr<sample_generator_1d<T>> clone_new() const override
        {
            return std::make_unique<poisson_sample_1d<T>>(*this);
        }
    private:
        T distance_between_samples;
    };

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
    void random_sample_1d<T>::get_samples(int num_samples, sample_output_fn pf)
    {
        int i;
        for (i = 0; i < num_samples; ++i)
        {
            pf(sample_generator_1d<T>::next_rand());
        }
    }

    template <class T>
    std::vector<T> regular_sample_1d<T>::get_samples(int num_samples)
    {
        const T scalar = NEAR_ONE / T(num_samples + 1);
        std::vector<T> v;
        for (int x = 1; x <= num_samples; ++x)
        {
            v.push_back(x * scalar);
        }
        return v;
    }

    template <class T>
    void regular_sample_1d<T>::get_samples(int num_samples, sample_output_fn pf)
    {
        const T scalar = NEAR_ONE / T(num_samples + 1);
        for (int x = 1; x <= num_samples; ++x)
        {
            pf(x * scalar);
        }
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
    void jitter_sample_1d<T>::get_samples(int num_samples, sample_output_fn pf)
    {
        int x;
        for (x = 0; x < num_samples; ++x)
        {
            pf((x + sample_generator_1d<T>::next_rand()) / T(num_samples));
        }
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
    void poisson_sample_1d<T>::get_samples(int num_samples, sample_output_fn pf)
    {
        std::vector<T> v = get_samples(num_samples);
        for (T t : v)
        {
            pf(t);
        }
    }
}
