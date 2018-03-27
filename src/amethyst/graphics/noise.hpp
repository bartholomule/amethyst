/*
 * $Id: noise.hpp,v 1.1 2008/06/22 17:27:07 kpharris Exp $
 *
 * Part of "Amethyst" -- A playground for graphics development.
 * Copyright (C) 2008 Kevin Harris
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

#if       !defined(AMETHYST__NOISE_HPP)
#define            AMETHYST__NOISE_HPP

#include <memory>
#include "amethyst/general/random.hpp"
#include <vector>
#include "amethyst/math/template_math.hpp"
#include "amethyst/math/coord3.hpp"

namespace amethyst
{

	/**
	 *
	 * A class for Perlin-style noise.
	 *
	 * @author Kevin Harris <kpharris@users.sourceforge.net>
	 * @version $Revision: 1.1 $
	 *
	 */
	template <class T>
	class noise
	{
	public:
		noise(std::shared_ptr<Random<T> > rnd = new default_random<T>(),
        int array_size = 256);
		noise(const noise<T>& n);
		~noise();
		noise<T>& operator=(const noise<T>& n);


		T operator()(const coord3<T>& point) const { return value(point); }
		T value(const coord3<T>& point) const;
		T turbulence(const coord3<T>& point, int levels = 8, T d = 2) const;

		coord3<T> vector_noise(const coord3<T>& point) const;
		coord3<T> vector_turbulence(const coord3<T>& point, int levels = 8, T d = 2) const;

		int phi_hash(int i) const;
		coord3<T> gradient(int i, int j, int k) const;
		T omega_knot(int i, int j, int k, T u, T v, T w) const;
		T weighting(T d) const;

	protected:
		T minus_1_to_1() { return(2 * rnd_gen->next() - 1); }
		coord3<T> rand_vec();

		std::shared_ptr<Random<T> > rnd_gen;

	private:
		void create_arrays();

		int size_of_arrays;
		std::vector<int> P_array;
		std::vector<coord3<T> > G_array;
	};

	template <class T>
	noise<T>::noise(std::shared_ptr<Random<T> > rnd, int array_size):
		rnd_gen(rnd),
		size_of_arrays(my_max(array_size, 16))
	{
		create_arrays();
	}

	template <class T>
	noise<T>::noise(const noise<T>& n):
		size_of_arrays(n.size_of_arrays),
		P_array(n.P_array),
		G_array(n.G_array)
	{
	}

	template <class T>
	noise<T>::~noise()
	{
	}

	template <class T>
	noise<T>& noise<T>::operator=(const noise<T>& n)
	{
		if(&n != this)
		{
			P_array = n.P_array;
			G_array = n.G_array;
		}
		return(*this);
	}


	template <class T>
	coord3<T> noise<T>::gradient(int i, int j, int k) const
	{
		int index = phi_hash(i + phi_hash(j + phi_hash(k)));
		return G_array[index];
	}


	template <class T>
	T noise<T>::value(const coord3<T>& point) const
	{
		int floor_x = int(floor(point.x()));
		int floor_y = int(floor(point.y()));
		int floor_z = int(floor(point.z()));

		T val = 0;
		for(int i = floor_x; i <= (floor_x + 1); ++i)
		{
			for(int j = floor_y; j <= (floor_y + 1); ++j)
			{
				for(int k = floor_z; k <= (floor_z + 1); ++k)
				{
					val += omega_knot(i,j,k,point.x()-i, point.y()-j, point.z()-k);
				}
			}
		}
		return val;
	}

	template <class T>
	T noise<T>::turbulence(const coord3<T>& point, int levels, T d) const
	{
		T return_value = 0;
		T scalar = 1;
		coord3<T> temp_coord = point;

		for(int i = 0; i <= levels; ++i)
		{
			// NOTE: In Pete's book, he's taking the absolute value of this.  That
			// will result in no negative turbulence values...
			return_value += scalar * tfabs(value(temp_coord));

			scalar *= 1 / d;
			temp_coord *= d;
		}
		return return_value;
	}

	template <class T>
	coord3<T> noise<T>::vector_noise(const coord3<T>& point) const
	{
		int floor_x = int(floor(point.x()));
		int floor_y = int(floor(point.y()));
		int floor_z = int(floor(point.z()));

		coord3<T> val(0,0,0);
		for(int i = floor_x; i <= (floor_x + 1); ++i)
		{
			for(int j = floor_y; j <= (floor_y + 1); ++j)
			{
				for(int k = floor_z; k <= (floor_z + 1); ++k)
				{
					val += gradient(i,j,k) * omega_knot(i,j,k,
						point.x()-i,
						point.y()-j,
						point.z()-k);
				}
			}
		}
		return val;
	}

	template <class T>
	coord3<T> noise<T>::vector_turbulence(const coord3<T>& point, int levels, T d) const
	{
		coord3<T> return_value(0,0,0);
		T scalar = 1;
		coord3<T> temp_coord = point;

		for(int i = 0; i < levels; ++i)
		{
			return_value += scalar * vector_noise(temp_coord);

			scalar *= 1 / d;
			temp_coord *= d;
		}
		return return_value;
	}

	template <class T>
	T noise<T>::omega_knot(int i, int j, int k, T u, T v, T w) const
	{
		T weight = weighting(u) * weighting(v) * weighting(w);
		return weight * dotprod(gradient(i,j,k), coord3<T>(u,v,w));
	}

	template <class T>
	int noise<T>::phi_hash(int i) const
	{
		int j = i % size_of_arrays;
		if(j < 0)
		{
			j += size_of_arrays;
		}
		return P_array[j];
	}

	template <class T>
	T noise<T>::weighting(T d) const
	{
		d = tfabs(d);
		if(d < 1)
		{
			return(2 * d*d*d - 3*d*d + 1);
		}
		return(0);
	}


	template <class T>
	coord3<T> noise<T>::rand_vec()
	{
		coord3<T> vec;

		do
		{
			vec.set(minus_1_to_1(),minus_1_to_1(),minus_1_to_1());
		}
		while(dotprod(vec,vec) > 1);

		return unit(vec);
	}

	template <class T>
	void noise<T>::create_arrays()
	{
		P_array.resize(size_of_arrays);
		G_array.resize(size_of_arrays);

		for(int i = 0; i < size_of_arrays; ++i)
		{
			P_array[i] = i;
			G_array[i] = rand_vec();
		}

		for(int i = size_of_arrays - 1; i > 0; --i)
		{
			std::swap(P_array[i], P_array[int(rnd_gen->next() * i)]);
		}
	}

} // namespace amethyst


#endif /* !defined(AMETHYST__NOISE_HPP) */

