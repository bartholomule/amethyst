#pragma once
#include "general/random.hpp"
#include "math/coord2.hpp"
#include "math/coord3.hpp"
#include <algorithm>
#include <vector>
#include <memory>

namespace amethyst
{
    namespace traits
    {
        template <typename T, size_t N>
        struct math_for_size { };

        template<typename T>
        struct math_for_size<T, 1>
        {
            using math_type = T;
        };

        template<typename T>
        struct math_for_size<T, 2>
        {
            using math_type = coord2<T>;
        };

        template<typename T>
        struct math_for_size<T, 3>
        {
            using math_type = coord3<T>;
        };
    }

    template <class T, size_t N>
    class sample_generator_base
    {
    public:
        using sample_type = typename traits::math_for_size<T,N>::math_type;
        using sample_output_fn = std::function<void(sample_type)>;
        using random_type = random<T>;

        sample_generator_base(const random_type& r = default_random<T>()) : sample_generator_base(r.clone_new()) { }
        sample_generator_base(std::shared_ptr<random_type> r) : rand_gen(std::move(r)) { }
        sample_generator_base(const sample_generator_base&) = default;
        virtual ~sample_generator_base() = default;

        virtual std::vector<sample_type> get_samples(size_t num_samples) = 0;
        virtual void get_samples(size_t num_samples, sample_output_fn pf)
        {
            std::vector<sample_type> v = get_samples(num_samples);
            for (const auto& s : v)
            {
                pf(s);
            }
        }

        const random_type& get_rand_gen() const { return *rand_gen; }
        void set_rand_gen(const random_type& r) { rand_gen = r.clone_new(); }

        T next_fp_rand() { return rand_gen->next(); }
        uint32_t next_int_rand() { return rand_gen->next_int(); };


        sample_type next_rand()
        {
            if constexpr(N == 1)
                return rand_gen->next();
            else if constexpr(N == 2)
                return { rand_gen->next(), rand_gen->next() };
            else
                return { rand_gen->next(), rand_gen->next(), rand_gen->next() };
        }

    private:
        std::shared_ptr<random_type> rand_gen;
    };
}