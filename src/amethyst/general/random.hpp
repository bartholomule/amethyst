#pragma once
/*
   random.hpp

   Classes to allow for the use of random number generators that are not
   dependant on any particular machine.  Any random number generator that should
   be used needs to be derived from the class random.
 */
/*
   Revision history:
   17jan1999 Created this file,
   19jan1999 Added the define NEAR_ONE, for use in the sample generators (but I
   need it here, so it can be used in both 2 and 3d stuff).
   25May2001 Changed this file so that it's contents are templated...
   16May2004 Added a mersenne twist random generator.  Moved into namespace amethyst.
   06Apr2018 Deleted nearly all of the implementation.  Using the std random instead.

 */

/* a define to use when making the range of numbers, so that my regular random
   samples don't end up hitting 1. */
#define NEAR_ONE 0.9999999999999995

#include <typeinfo>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <memory>

#include <time.h>

#include "amethyst/general/types.hpp"

namespace amethyst
{
    // If T is a floating point type, the result of next() will be numbers between 0 and 1.
    // If T is an integer, the result of next() will be numbers between 0 and std::numeric_limits<T>::max()
    template <class T>
    class random
    {
    public:
        random() = default;
        virtual ~random() = default;
        random(const random&) = default;
        random& operator=(const random&) = default;

        virtual T next()
        {
            // These floating point conversions are somewhat non-uniform, but good enough for now
            if constexpr(std::is_same<T, double>::value)
                return ldexp(double(next_long()), -63);
            else if constexpr(std::is_floating_point<T>::value)
                return ldexp(next_int(), -31);
            else if constexpr(sizeof(T) > sizeof(uint32_t))
                return T(next_int()) << 32 | T(next_int());
            else
                return T(next_int());
        }

        virtual uint32_t next_int() = 0;
        virtual uint64_t next_long() { return uint64_t(next_int()) << 32 | next_int(); }

        virtual std::unique_ptr<random<T>> clone_new() const = 0;
        virtual void set_seed(uint32_t seed) = 0;

        template <typename IntType, typename Iter>
        void fill_int(IntType maxval, Iter first, Iter last)
        {
            for (; first != last; ++first)
            {
                if constexpr(sizeof(IntType <= 8))
                    *first = IntType(next_int(maxval));
                else
                    *first = IntType(next_long(maxval));
            }
        }

        uint32_t next_int(uint32_t max)
        {
            return static_cast<uint32_t>(ldexp(double(next_int()), -31) * max);
        }

        uint64_t next_long(uint64_t max)
        {
            return static_cast<uint64_t>(ldexp(double(next_long()), -63) * max);
        }

        template <class array_type>
        void shuffle(array_type& arr)
        {
            if (arr.empty())
                return;

            size_t current = arr.size() - 1;
            while (current > 1)
            {
                size_t swap_number = size_t(next_int() % (current - 1));

                std::swap(arr[current], arr[swap_number]);
                --current;
            }
        }

        template <class iterator_type>
        void shuffle(iterator_type first, iterator_type last)
        {
            if (first != last)
            {
                size_t current = last - first - 1;
                while (current > 1)
                {
                    size_t swap_number = size_t(next_int() % (current - 1));

                    std::swap(*(first + current), *(first + swap_number));
                    --current;
                }
            }
        }
    };

    template <typename T, typename StdRand>
    class standard_random : public random<T>
    {
    public:
        standard_random() : standard_random(uint32_t(1073741827 * time(0))) // large number and the time.
        {
        }
        standard_random(uint32_t seed) : m_rand(seed)
        {
        }
        standard_random(const standard_random&) = default;
        virtual ~standard_random() = default;

        using random::next_int;
        uint32_t next_int() override { return m_rand(); }
        void set_seed(uint32_t seed) override { m_rand.seed(seed); }
        std::unique_ptr<random<T>> clone_new() const
        {
            return std::make_unique<standard_random<T,StdRand>>(*this);
        }
    private:
        StdRand m_rand;
    };

    template <typename T>
    using mersenne_twist_random = standard_random<T, std::mt19937>;

    template <typename T>
    using default_random = standard_random<T, std::minstd_rand>;
}