/*
 * $Id: random.hpp,v 1.2 2008/12/15 17:27:26 kpharris Exp $
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

#if    !defined(AMETHYST__RANDOM_HPP)
#define         AMETHYST__RANDOM_HPP

/*
  random.hpp

  Classes to allow for the use of random number generators that are not
  dependant on any particular machine.  Any random number generator that should
  be used needs to be derived from the class Random.
*/
/*
  Revision history:
  17jan1999 Created this file,
  19jan1999 Added the define NEAR_ONE, for use in the sample generators (but I
  need it here, so it can be used in both 2 and 3d stuff).
  25May2001 Changed this file so that it's contents are templated...
  16May2004 Added a mersenne twist random generator.  Moved into namespace amethyst.

*/

/* a define to use when making the range of numbers, so that my regular random
   samples don't end up hitting 1. */
#define NEAR_ONE 0.9999999999999995

#include <typeinfo>
#include <cstdlib>
#include <algorithm>

#include <time.h>

#include "amethyst/general/types.hpp"

namespace amethyst
{

	template <class T>
	class Random
	{
	public:
		virtual ~Random() {}
		virtual T next() = 0;
		virtual UInt32 next_int() = 0;
		virtual Random<T>* clone_new() const = 0;
		virtual void set_seed(UInt32 seed) = 0;

		UInt32 next_int(UInt32 max);
		template <class array_type>
		void shuffle(array_type& arr);

		template <class iterator_type>
		void shuffle(iterator_type first, iterator_type last);
	private:
		Random<T>& operator=(const Random<T>&); // Not implemented.
	};


	/* The default random number generator.  This is based on erand48. */
	template <class T>
	class default_random:public Random<T>
	{
	public:
		default_random();
		default_random(UInt32 seed);
		default_random(const default_random& old_rand);

		virtual ~default_random() {}


		virtual T next();
		virtual UInt32 next_int();

		virtual void set_seed(UInt32 seed);
		void set_seed(unsigned short seed[3]);

		virtual default_random<T>* clone_new() const;
	private:
		unsigned short int state[3];
	};

	/* A mersenne twister */
	template <class T>
	class mersenne_twist_random : public Random<T>
	{
	public:
		mersenne_twist_random();
		mersenne_twist_random(UInt32 seed);
		mersenne_twist_random(const mersenne_twist_random& old_rand);
		virtual ~mersenne_twist_random() { }

		virtual T next();
		virtual UInt32 next_int();
		virtual void set_seed(UInt32 seed);

		virtual mersenne_twist_random<T>* clone_new() const;
	private:
		static const size_t STATE_SIZE = 624;
		UInt32 state[STATE_SIZE];
		size_t state_used; // How many random numbers are left before
		// recalculation needs to take place.
		enum {ARCH_BIG_ENDIAN, ARCH_LITTLE_ENDIAN, ARCH_NON_IEEE} arch_type;
	};

	template <class T>
	template <class array_type>
	void Random<T>::shuffle(array_type& arr)
	{
		size_t current = arr.size() - 1;
		while( current > 1 )
		{
			size_t swap_number = size_t(next_int() % (current - 1));

			std::swap(arr[current], arr[swap_number]);
			--current;
		}
	}

	template <class T>
	template <class iterator_type>
	void Random<T>::shuffle(iterator_type first, iterator_type last)
	{
		if( first != last )
		{
			size_t current = last - first - 1;
			while( current > 1 )
			{
				size_t swap_number = size_t(next_int() % (current - 1));

				std::swap(*(first + current), *(first + swap_number));
				--current;
			}
		}
	}

	template <class T>
	UInt32 Random<T>::next_int(UInt32 max)
	{
		UInt32 val = next_int();
		UInt32 retval= static_cast<UInt32>(ldexp(double(val), -32) * max);
		return retval;
	}

	template <class T>
	/* If the time should be used to generate an initial seed, use it, otherwise,
		just start with a default seed */
	default_random<T>::default_random() : Random<T>()
	{
		set_seed(1073741827 * time(0)); /* a large, likely prime number & the time */
	}

	template <class T>
	default_random<T>::default_random(UInt32 seed)
	{
		set_seed(seed);
	}

	template <class T>
	default_random<T>::default_random(const default_random<T>& old_rand):
		Random<T>(old_rand)
	{
		state[0] = old_rand.state[0];
		state[1] = old_rand.state[1];
		state[2] = old_rand.state[2];
	}

	template <class T>
	T default_random<T>::next()
	{
		return(T(erand48(state)));
	}

	template <class T>
	UInt32 default_random<T>::next_int()
	{
		return(jrand48(state));
	}

	template <class T>
	void default_random<T>::set_seed(UInt32 seed)
	{
		unsigned short int max_short = (unsigned short) - 1;
		int short_bits = sizeof(short) * 8;

		state[0] = seed & max_short;
		state[1] = (seed >>= short_bits) & max_short;
		state[2] = (seed >>= short_bits) & max_short;
	}

	template <class T>
	void default_random<T>::set_seed(unsigned short seed[3])
	{
		state[0] = seed[0]; state[1] = seed[1]; state[2] = seed[2];
	}

	template <class T>
	default_random<T>* default_random<T>::clone_new() const
	{
		return new default_random<T>(*this);
	}

	template <class T>
	mersenne_twist_random<T>::mersenne_twist_random() : Random<T>()
	{
		set_seed(1073741827 * time(0)); /* a large, likely prime number & the time */
	}

	template <class T>
	mersenne_twist_random<T>::mersenne_twist_random(UInt32 seed) : Random<T>()
	{
		set_seed(seed);
	}

	template <class T>
	mersenne_twist_random<T>::mersenne_twist_random(const mersenne_twist_random<T>& old_rand):
		Random<T>(old_rand),
		arch_type(old_rand.arch_type)
	{
		for( size_t i = 0; i < STATE_SIZE; ++i )
		{
			state[i] = old_rand.state[i];
		}
		state_used = old_rand.state_used;
	}

	template <class T>
	void mersenne_twist_random<T>::set_seed(UInt32 seed)
	{
		// This was borrowed from mersenne.cpp (A. Fog)
		state[0] = seed;
		for( state_used = 1; state_used < STATE_SIZE; ++state_used )
		{
			state[state_used] = (
				1812433253UL * (
					(state[state_used-1]) ^	(state[state_used-1] >> 30)
				) + state_used);
		}
		// Find out how the FP numbers are arranged.
		union {double f; UInt32 i[2];} convert;
		convert.f = 1.0;
		if (convert.i[1] == 0x3FF00000)
		{
			arch_type = ARCH_LITTLE_ENDIAN;
		}
		else if (convert.i[0] == 0x3FF00000)
		{
			arch_type = ARCH_BIG_ENDIAN;
		}
		else
		{
			arch_type = ARCH_NON_IEEE;
		}
	}

	const UInt32 MERS_M = 397;
	const UInt32 MERS_R = 31;
	const UInt32 MERS_U = 11;
	const UInt32 MERS_S = 7;
	const UInt32 MERS_T = 15;
	const UInt32 MERS_L = 18;
	const UInt32 MERS_A = 0x9908B0DF;
	const UInt32 MERS_B = 0x9D2C5680;
	const UInt32 MERS_C = 0xEFC60000;

	template <class T>
	UInt32 mersenne_twist_random<T>::next_int()
	{
		// generate 32 random bits
		UInt32 y;

		if (state_used >= STATE_SIZE)
		{
			// generate STATE_SIZE words at one time
			const UInt32 LOWER_MASK = (1LU << MERS_R) - 1; // lower MERS_R bits
			const UInt32 UPPER_MASK = -1L  << MERS_R;      // upper (32 - MERS_R) bits
			static const UInt32 mag01[2] = {0, MERS_A};

			size_t kk;
			for (kk=0; kk < STATE_SIZE-MERS_M; kk++)
			{
				y = (state[kk] & UPPER_MASK) | (state[kk+1] & LOWER_MASK);
				state[kk] = state[kk+MERS_M] ^ (y >> 1) ^ mag01[y & 1];
			}
			for (; kk < STATE_SIZE-1; kk++)
			{
				y = (state[kk] & UPPER_MASK) | (state[kk+1] & LOWER_MASK);
				state[kk] = state[kk+(MERS_M-STATE_SIZE)] ^ (y >> 1) ^ mag01[y & 1];
			}

			y = (state[STATE_SIZE-1] & UPPER_MASK) | (state[0] & LOWER_MASK);
			state[STATE_SIZE-1] = state[MERS_M-1] ^ (y >> 1) ^ mag01[y & 1];
			state_used = 0;
		}

		y = state[state_used++];

		// Tempering (May be omitted):
		y ^=  y >> MERS_U;
		y ^= (y << MERS_S) & MERS_B;
		y ^= (y << MERS_T) & MERS_C;
		y ^=  y >> MERS_L;
		return y;
	}

	template <class T>
	T mersenne_twist_random<T>::next()
	{
		const size_t num_longs_required = sizeof(double) / sizeof(UInt32);
		union
		{
			double d;
			UInt32 i[num_longs_required];
		} conversion;

		UInt32 random_int = next_int();
		UInt32 random_int2 = next_int();

		/*
		 *  S EEEEEEEEEEE FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
		 *  0 1        11 12                                                63
		 */

		switch( arch_type )
		{
		case ARCH_LITTLE_ENDIAN:
			if( num_longs_required == 2 ) // The common case.
			{
				conversion.i[0] = random_int;
				conversion.i[1] = (random_int2 >> 12) | 0x3FF00000;
				return conversion.d - 1.0;
			}
		case ARCH_BIG_ENDIAN:
			if( num_longs_required == 2 ) // The common case.
			{
				conversion.i[1] = random_int;
				conversion.i[0] = (random_int >> 12) | 0x3FF00000;
				return conversion.d - 1.0;
			}
		case ARCH_NON_IEEE:
		default:
			;
		}
		// This somewhat slower method works for all architectures, including
		// non-IEEE floating point representation:
		// NOTE: This process removes the uniformness, as rounding comes into
		// effect.
		return (double)random_int * (1./((double)(UInt32)(-1L)+1.));
	}

	template <class T>
	mersenne_twist_random<T>* mersenne_twist_random<T>::clone_new() const
	{
		return new mersenne_twist_random<T>(*this);
	}
} // namespace amethyst
#endif /* !defined(AMETHYST__RANDOM_HPP) */
