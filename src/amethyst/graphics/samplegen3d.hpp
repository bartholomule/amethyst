#pragma once

#include "samplegen_base.hpp"

namespace amethyst
{
    template <typename T>
    class sample_generator_3d : public sample_generator_base<T, 3>
    {
    public:
        using parent = sample_generator_base<T, 3>;
        using parent::parent;

        virtual std::unique_ptr<sample_generator_3d<T>> clone_new() const = 0;
    protected:
        using parent::next_rand;
    };

    template <class T>
    class random_sample_3d : public sample_generator_3d<T>
    {
    public:
        using parent = sample_generator_3d<T>;
        using parent::parent;
        virtual ~random_sample_3d() = default;

        std::vector<typename parent::sample_type> get_samples(size_t num_samples) override
        {
            std::vector<typename parent::sample_type> result;
            result.reserve(num_samples);
            for(; num_samples > 0; --num_samples)
            {
                result.emplace_back(parent::next_rand());
            }
            return result;
        }

        void get_samples(size_t num_samples, typename parent::sample_output_fn pf) override
        {
            for (; num_samples > 0; --num_samples)
            {
                pf(parent::next_rand());
            }
        }

        std::unique_ptr<sample_generator_3d<T>> clone_new() const override
        {
            return std::make_unique<random_sample_3d<T>>(*this);
        }
    };

    template <class T>
    class sphere_sample_3d : public sample_generator_3d<T>
    {
    public:
        using parent = sample_generator_3d<T>;
        using parent::parent;
        using typename parent::sample_type;

        virtual ~sphere_sample_3d() = default;

        sample_type next_sample()
        {
            sample_type result = 2 * parent::next_rand() - sample_type(1,1,1);
            while (squared_length(result) >= 1)
            {
                result = 2 * parent::next_rand() - sample_type(1, 1, 1);
            }
            return result;
        }

        std::vector<sample_type> get_samples(size_t num_samples) override
        {
            std::vector<sample_type> result;
            result.reserve(num_samples);
            for (; num_samples > 0; --num_samples)
            {
                result.emplace_back(next_sample());
            }
            return result;
        }

        void get_samples(size_t num_samples, typename parent::sample_output_fn pf) override
        {
            for (; num_samples > 0; --num_samples)
            {
                pf(next_sample());
            }
        }

        std::unique_ptr<sample_generator_3d<T>> clone_new() const override
        {
            return std::make_unique<sphere_sample_3d<T>>(*this);
        }
    };
}