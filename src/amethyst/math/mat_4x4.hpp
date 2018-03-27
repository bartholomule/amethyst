#if    !defined(KH_MAT_4X4_H)
#define         KH_MAT_4X4_H

/*
  mat_4x4.h

  A 4x4 matrix class.
  NOTE: There is no error checking on the [] operators.
 */

/*
  Revision history:
  21jan1999 Created this file.
  25jan1999 added the << operator, and the constructor that takes an array of doubles.
            Added the functions to create a translate, scale, and rotation matrix.
	    Added a unary -.
	    Added a transpose function.
  17feb1999 Added the operator ().
  08Apr2000 Changed the includes that refer to "3d_stuff/xxx", as I am reorganizing.
  18May2001 Changed the matrix class to use in a new tracer...  Note: matrix
            additions (subtractions) will now be much slower, as it will need
	    to compute the inverse of the matrix after the addition.
	    Everything else is done with a tiny bit of slowdown, for matrix
	    inverse calculation (I know how to invert after multiply, scale,
	    etc.).
  09Jun2001 Fixed some mistakes that prevented compilation, commented out
            division by the homogeneous coord in vector/normal transform
	    functions.
*/

/*
  Thoughts--
  I was thinking about inverting a matrix the hard way, but I think I may have
  just changed my mind.  I may change to some kind of iteration, or LU
  decomposition.  For right now, I am not going to invert matricies.

  Inversion of matricies -- bleh. Forget that for now.  When you do decide to
  add inversion of matricies, just take it out of some book.
  | a b c d |   | A B C D |   | 1 0 0 0 |
  | e f g h | * | E F G H | = | 0 1 0 0 |
  | i j k l |   | I J K L |   | 0 0 1 0 |
  | m n o p |   | M N O P |   | 0 0 0 1 |
  a*A + b*E + c*I + d*M = 1
  a*B + b*F + c*J + d*N = 0
  a*C + b*G + c*K + d*O = 0
  a*D + b*H + c*L + d*P = 0

  e*A + f*E + g*I + h*M = 0
  e*B + f*F + g*J + h*N = 1
  e*C + f*G + g*K + h*O = 0
  e*D + f*H + g*L + h*P = 0

  i*A + j*E + k*I + l*M = 0
  i*B + j*F + k*J + l*N = 0
  i*C + j*G + k*K + l*O = 1
  i*D + j*H + k*L + l*P = 0

  m*A + n*E + o*I + p*M = 0
  m*B + n*F + o*J + p*N = 0
  m*C + n*G + o*K + p*O = 0
  m*D + n*H + o*L + p*P = 1

  A =  (1 - (b*E + c*I + d*M))/a
  B = -(b*F + c*J + d*N)/a
  C = -(b*G + c*K + d*O)/a
  D = -(b*H + c*L + d*P)/a

  A = -(f*E + g*I + h*M)/e
  B =  (1 - (f*F + g*J + h*N))/e
  C = -(f*G + g*K + h*O)/e
  D = -(f*H + g*L + h*P)/e

  A = -(j*E + k*I + l*M)/i
  B = -(j*F + k*J + L*N)/i
  C =  (1 - (j*G + k*K + l*O))/i
  D = -(j*H + k*L + l*P)/i

  A = -(n*E + o*I + p*M)/m
  B = -(n*F + o*J + p*N)/m
  C = -(n*G + o*K + p*O)/m
  D =  (1 - (n*H + o*L + p*P))/m

*/

#include <math/coord3.hpp>
#include <math/point3.hpp>
#include <math/vector3.hpp>
#include <cassert>
#include <cstring>

namespace amethyst
{
	template <class T> class matrix_4x4;

	template <class T> matrix_4x4<T> invert (const matrix_4x4<T>& rktMAT);

	template <class T> std::ostream& operator<<(std::ostream& o, const matrix_4x4<T>& m);

	template <class T>
	class matrix_4x4
	{
	public:
		inline matrix_4x4() {}
		matrix_4x4(const T dat[16]);
		matrix_4x4(const T dat[16], const T inv[16]);
		matrix_4x4(const matrix_4x4<T>& m);
		inline ~matrix_4x4() {}
		matrix_4x4<T>& operator=(const matrix_4x4<T>& m);

		inline const T* operator[](int index)  const
		{
			return(&data[index*4]);
		}
		inline T* operator[](int index)
		{
			return(&data[index*4]);
		}

		inline T& operator()(int row, int column) { return(data[row*4 + column]); }
		inline const T& operator()(int row, int column) const { return(data[row*4 + column]); }

		inline coord3<T> multiply(const coord3<T>& c, T homogeneous = 1, bool divide=true) const;
		matrix_4x4<T> operator*(const matrix_4x4<T>& mat) const;
		// multiply and divide by the homogeneous coord (homogeneous initially 1).
		coord3<T>     operator*(const coord3<T>& point) const;
		// multiply and divide by the homogeneous coord (homogeneous initially 1).
		point3<T>     operator*(const point3<T>& point) const;
		// multiply.  homogenous initially 0, no divide is done, and the homogeneous
		// is lost after the call.
		vector3<T>    operator*(const vector3<T>& vector) const;

		matrix_4x4<T> operator+(const matrix_4x4<T>& mat) const;
		matrix_4x4<T> operator-(const matrix_4x4<T>& mat) const;
		inline matrix_4x4<T> operator-() const { return((*this)*(-1)); }

		matrix_4x4<T> operator*(T d) const;

		template <class P> friend matrix_4x4<P> operator*(P d,
			const matrix_4x4<P> &mat);
		template <class P> friend matrix_4x4<P> transpose(const matrix_4x4<P>& m);

		void set_identity();
		void set_translate(const coord3<T>& amount);
		void set_scale(const coord3<T>& amount);
		void set_rotate(const coord3<T>& u, const coord3<T>& v, const coord3<T>& w);
		void set_transform(const coord3<T>& u, const coord3<T>& v, const coord3<T>& w, const coord3<T>& origin);

		// These take angles in radians!!!
		void set_rotate_x(T theta);
		void set_rotate_y(T theta);
		void set_rotate_z(T theta);


		// These transform an external item to the internal coord system.
		coord3<T> transform_as_point(const coord3<T>& c) const;
		coord3<T> transform_as_vector(const coord3<T>& c) const;
		coord3<T> transform_as_normal(const coord3<T>& c) const;

		// These transform an internal item to the external coord system.
		coord3<T> ext_transform_as_point(const coord3<T>& c) const;
		coord3<T> ext_transform_as_vector(const coord3<T>& c) const;
		coord3<T> ext_transform_as_normal(const coord3<T>& c) const;


		static matrix_4x4<T> identity();
		static matrix_4x4<T> make_translate(const coord3<T>& amount);
		static matrix_4x4<T> make_scale(const coord3<T>& amount);
		static matrix_4x4<T> make_rotate(const coord3<T>& u, const coord3<T>& v, const coord3<T>& w);
		static matrix_4x4<T> make_transform(const coord3<T>& u, const coord3<T>& v, const coord3<T>& w, const coord3<T>& origin);


		friend std::ostream& operator << <>(std::ostream& o, const matrix_4x4<T>& m);

		matrix_4x4<T> inverse() const { return matrix_4x4<T>(inverse_data,data); }

	private:
		T data[16];
		T inverse_data[16];
	};

#define LINEAR(y,x) ((y)*4 + (x))
	template <class T>
	matrix_4x4<T>::matrix_4x4(const T dat[16])
	{
		for(int i = 0; i < 16; ++i)
		{
			data[i] = dat[i];
		}
		matrix_4x4<T> temp = invert(*this);
		for(int i = 0; i < 16; ++i)
		{
			inverse_data[i] = temp.data[i];
		}
	}

	template <class T>
	matrix_4x4<T>::matrix_4x4(const T dat[16], const T inv[16])
	{
		for(int i = 0; i < 16; ++i)
		{
			data[i] = dat[i];
			inverse_data[i] = inv[i];
		}
	}


	template <class T>
	matrix_4x4<T>::matrix_4x4(const matrix_4x4& m)
	{
		for(int i = 0; i < 16; ++i)
		{
			data[i] = m.data[i];
			inverse_data[i] = m.inverse_data[i];
		}
	}

	template <class T>
	matrix_4x4<T>& matrix_4x4<T>::operator=(const matrix_4x4<T>& m)
	{
		if(&m != this)
		{
			for(int i = 0; i < 16; ++i)
			{
				data[i] = m.data[i];
				inverse_data[i] = m.inverse_data[i];
			}
		}
		return(*this);
	}


	template <class T>
	matrix_4x4<T> transpose(const matrix_4x4<T>& m)
	{
		matrix_4x4<T> dest;

		for(int row = 0; row < 4; ++row)
		{
			for(int col = 0; col < 4; ++col)
			{
				dest(row,col) = m(col,row);
				dest.inverse_data[LINEAR(row,col)] = m.inverse_data[LINEAR(row,col)];
			}
		}

		return(dest);
	}

	template <class T>
	matrix_4x4<T> matrix_4x4<T>::operator*(const matrix_4x4<T>& mat) const
	{
		matrix_4x4<T> dest;

		for(int dest_row = 0; dest_row < 4; ++dest_row)
		{
			for(int dest_column = 0; dest_column < 4; ++dest_column)
			{
				T cur_element = 0;
				T inv_element = 0;
				for(int src_column = 0; src_column < 4; ++src_column)
				{
					cur_element += ((*this)(dest_row,src_column) *
						mat(src_column,dest_column));
					// Multiply the inverses in reverse order, to preserve them...
					inv_element += (mat(dest_row,src_column) *
						(*this)(src_column,dest_column));
				}
				dest(dest_row, dest_column) = cur_element;
				dest.inverse_data[LINEAR(dest_row, dest_column)] = inv_element;
			}
		}

		return(dest);
	}

	template <class T>
	inline coord3<T> matrix_4x4<T>::multiply(const coord3<T>& c, T homogeneous, bool divide) const
	{
		coord3<T> dest;
		T result_homogeneous;

		dest.x() = data[LINEAR(0,0)] * c.x() +
			data[LINEAR(0,1)] * c.y() +
			data[LINEAR(0,2)] * c.z() +
			data[LINEAR(0,3)] * homogeneous;

		dest.y() = data[LINEAR(1,0)] * c.x() +
			data[LINEAR(1,1)] * c.y() +
			data[LINEAR(1,2)] * c.z() +
			data[LINEAR(1,3)] * homogeneous;

		dest.z() = data[LINEAR(2,0)] * c.x() +
			data[LINEAR(2,1)] * c.y() +
			data[LINEAR(2,2)] * c.z() +
			data[LINEAR(2,3)] * homogeneous;
		result_homogeneous = data[LINEAR(3,0)] * c.x() +
			data[LINEAR(3,1)] * c.y() +
			data[LINEAR(3,2)] * c.z() +
			data[LINEAR(3,3)] * homogeneous;

		if(divide)
		{
			dest /= result_homogeneous;
		}
		return dest;
	}

	template <class T>
	coord3<T> matrix_4x4<T>::operator*(const coord3<T>& point) const
	{
		return(multiply(point, 1, true));
	}

	template <class T>
	matrix_4x4<T> matrix_4x4<T>::operator+(const matrix_4x4<T>& mat) const
	{
		T temp_data[16];
		for(int i = 0; i < 16; ++i)
		{
			temp_data[i] = data[i] + mat.data[i];
		}
		return(matrix_4x4<T>(temp_data));
	}

	template <class T>
	matrix_4x4<T> matrix_4x4<T>::operator-(const matrix_4x4<T>& mat) const
	{
		T temp_data[16];
		for(int i = 0; i < 16; ++i)
		{
			temp_data[i] = data[i] + mat.data[i];
		}
		return(matrix_4x4<T>(temp_data));
	}

	template <class T>
	matrix_4x4<T> matrix_4x4<T>::operator*(T d) const
	{
		matrix_4x4<T> ret_mat;
		for(int i = 0; i < 16; ++i)
		{
			ret_mat.data[i] = data[i] * d;
			ret_mat.inverse_data[i] = inverse_data[i] / d;
		}
		return(ret_mat);
	}

	template <class T>
	matrix_4x4<T> operator*(T d, const matrix_4x4<T>& mat)
	{
		matrix_4x4<T> ret_mat;
		for(int i = 0; i < 16; ++i)
		{
			ret_mat.data[i] = mat.data[i] * d;
			ret_mat.inverse_data[i] = mat.inverse_data[i] / d;
		}
		return(ret_mat);
	}

	template <class T>
	void matrix_4x4<T>::set_identity()
	{
		for(int row = 0; row < 4; ++row)
		{
			for(int col = 0; col < 4; ++col)
			{
				data[LINEAR(row,col)] = inverse_data[LINEAR(row,col)] = (row == col);
			}
		}
	}

	template <class T>
	void matrix_4x4<T>::set_scale(const coord3<T>& c)
	{
		for(int row = 0; row < 4; ++row)
		{
			for(int col = 0; col < 4; ++col)
			{
				if(row == col)
				{
					if(row < 3)
					{
						data[LINEAR(row,col)] = c[row];
					}
					else
					{
						data[LINEAR(row,col)] = 1;
					}
					inverse_data[LINEAR(row,col)] = 1/data[LINEAR(row,col)];
				}
				else
				{
					data[LINEAR(row,col)] = inverse_data[LINEAR(row,col)] = 0;
				}
			}
		}
	}

	template <class T>
	void matrix_4x4<T>::set_translate(const coord3<T>& c)
	{
		for(int row = 0; row < 4; ++row)
		{
			for(int col = 0; col < 4; ++col)
			{
				if(row == col)
				{
					data[LINEAR(row,col)] = inverse_data[LINEAR(row,col)] = 1;
				}
				else if(col == 3)
				{
					if(row < 3)
					{
						data[LINEAR(row,col)] = c[row];
						inverse_data[LINEAR(row,col)] = -c[row];
					}
					else
					{
						data[LINEAR(row,col)] = 1;
						inverse_data[LINEAR(row,col)] = 1;
					}
				}
				else
				{
					data[LINEAR(row,col)] = inverse_data[LINEAR(row,col)] = 0;
				}
			}
		}
	}

	template <class T>
	void matrix_4x4<T>::set_rotate(const coord3<T>& u, const coord3<T>& v, const coord3<T>& w)
	{
		data[LINEAR(0,0)] = u.x();
		data[LINEAR(0,1)] = u.y();
		data[LINEAR(0,2)] = u.z();
		data[LINEAR(0,3)] = 0;

		data[LINEAR(1,0)] = v.x();
		data[LINEAR(1,1)] = v.y();
		data[LINEAR(1,2)] = v.z();
		data[LINEAR(1,3)] = 0;

		data[LINEAR(2,0)] = w.x();
		data[LINEAR(2,1)] = w.y();
		data[LINEAR(2,2)] = w.z();
		data[LINEAR(2,3)] = 0;

		data[LINEAR(3,0)] = 0;
		data[LINEAR(3,1)] = 0;
		data[LINEAR(3,2)] = 0;
		data[LINEAR(3,3)] = 1;

		// Inverse == transpose for rotations...
		for(int row = 0; row < 4; ++row)
		{
			for(int col = 0; col < 4; ++col)
			{
				inverse_data[LINEAR(row,col)] = data[LINEAR(col, row)];
			}
		}
	}

	template <class T>
	void matrix_4x4<T>::set_transform(const coord3<T>& u, const coord3<T>& v, const coord3<T>& w, const coord3<T>& origin)
	{
		// Make the rotation first, then add in the translate (for speed and ease of
		// inverse creation).
		data[LINEAR(0,0)] = u.x();
		data[LINEAR(0,1)] = u.y();
		data[LINEAR(0,2)] = u.z();
		data[LINEAR(0,3)] = 0;

		data[LINEAR(1,0)] = v.x();
		data[LINEAR(1,1)] = v.y();
		data[LINEAR(1,2)] = v.z();
		data[LINEAR(1,3)] = 0;

		data[LINEAR(2,0)] = w.x();
		data[LINEAR(2,1)] = w.y();
		data[LINEAR(2,2)] = w.z();
		data[LINEAR(2,3)] = 0;

		data[LINEAR(3,0)] = 0;
		data[LINEAR(3,1)] = 0;
		data[LINEAR(3,2)] = 0;
		data[LINEAR(3,3)] = 1;

		// Inverse == transpose for rotations...
		for(int row = 0; row < 4; ++row)
		{
			for(int col = 0; col < 4; ++col)
			{
				inverse_data = data[LINEAR(col, row)];
			}
		}

		for(int i = 0; i < 3; ++i)
		{
			data[LINEAR(i,3)] = origin[i];
			inverse_data[LINEAR(3,i)] = origin[i];
		}
	}

	template <class T>
	void matrix_4x4<T>::set_rotate_x(T theta)
	{
		T s_theta = sin(theta);
		T c_theta = cos(theta);

		//right side/bottom side
		data[LINEAR(0,3)] = data[LINEAR(1,3)] = data[LINEAR(2,3)] = 0;
		data[LINEAR(3,0)] = data[LINEAR(3,1)] = data[LINEAR(3,2)] = 0;
		data[LINEAR(3,3)] = 1;

		// Internal data...
		data[LINEAR(0,0)] = 1;
		data[LINEAR(0,1)] = 0;
		data[LINEAR(0,2)] = 0;

		data[LINEAR(1,0)] = 0;
		data[LINEAR(1,1)] = c_theta;
		data[LINEAR(1,2)] = -s_theta;

		data[LINEAR(2,0)] = 0;
		data[LINEAR(2,1)] = s_theta;
		data[LINEAR(2,2)] = c_theta;

		// Create the inverse...
		for(int row = 0; row < 4; ++row)
		{
			for(int col = 0; col < 4; ++col)
			{
				inverse_data[LINEAR(row,col)] = data[LINEAR(col,row)];
			}
		}
	}
	template <class T>
	void matrix_4x4<T>::set_rotate_y(T theta)
	{
		T s_theta = sin(theta);
		T c_theta = cos(theta);

		//right side/bottom side
		data[LINEAR(0,3)] = data[LINEAR(1,3)] = data[LINEAR(2,3)] = 0;
		data[LINEAR(3,0)] = data[LINEAR(3,1)] = data[LINEAR(3,2)] = 0;
		data[LINEAR(3,3)] = 1;

		// Internal data...
		data[LINEAR(0,0)] = c_theta;
		data[LINEAR(0,1)] = 0;
		data[LINEAR(0,2)] = s_theta;

		data[LINEAR(1,0)] = 0;
		data[LINEAR(1,1)] = 1;
		data[LINEAR(1,2)] = 0;

		data[LINEAR(2,0)] = -s_theta;
		data[LINEAR(2,1)] = 0;
		data[LINEAR(2,2)] = c_theta;

		// Create the inverse...
		for(int row = 0; row < 4; ++row)
			for(int col = 0; col < 4; ++col)
				inverse_data[LINEAR(row,col)] = data[LINEAR(col,row)];
	}
	template <class T>
	void matrix_4x4<T>::set_rotate_z(T theta)
	{
		T s_theta = sin(theta);
		T c_theta = cos(theta);

		//right side/bottom side
		data[LINEAR(0,3)] = data[LINEAR(1,3)] = data[LINEAR(2,3)] = 0;
		data[LINEAR(3,0)] = data[LINEAR(3,1)] = data[LINEAR(3,2)] = 0;
		data[LINEAR(3,3)] = 1;

		// Internal data...
		data[LINEAR(0,0)] = c_theta;
		data[LINEAR(0,1)] = -s_theta;
		data[LINEAR(0,2)] = 0;

		data[LINEAR(1,0)] = s_theta;
		data[LINEAR(1,1)] = c_theta;
		data[LINEAR(1,2)] = 0;

		data[LINEAR(2,0)] = 0;
		data[LINEAR(2,1)] = 0;
		data[LINEAR(2,2)] = 1;

		// Create the inverse...
		for(int row = 0; row < 4; ++row)
		{
			for(int col = 0; col < 4; ++col)
			{
				inverse_data[LINEAR(row,col)] = data[LINEAR(col,row)];
			}
		}
	}

	template <class T>
	matrix_4x4<T> matrix_4x4<T>::identity()
	{
		matrix_4x4<T> ret_mat;

		ret_mat.set_identity();

		return(ret_mat);
	}

	template <class T>
	matrix_4x4<T> matrix_4x4<T>::make_translate(const coord3<T>& amount)
	{
		matrix_4x4<T> ret_mat;

		ret_mat.set_translate(amount);

		return(ret_mat);
	}

	template <class T>
	matrix_4x4<T> matrix_4x4<T>::make_scale(const coord3<T>& amount)
	{
		matrix_4x4<T> ret_mat;

		ret_mat.set_scale(amount);

		return(ret_mat);
	}

	template <class T>
	matrix_4x4<T>  matrix_4x4<T>::make_rotate(const coord3<T>& u, const coord3<T>& v, const coord3<T>& w)
	{
		matrix_4x4<T> ret_mat;

		ret_mat.set_rotate(u,v,w);

		return(ret_mat);
	}

	template <class T>
	matrix_4x4<T> matrix_4x4<T>::make_transform(const coord3<T>& u, const coord3<T>& v, const coord3<T>& w, const coord3<T>& origin)
	{
		matrix_4x4<T> ret_mat;

		ret_mat.set_transform(u,v,w,origin);

		return(ret_mat);
	}


	template <class T>
	vector3<T> matrix_4x4<T>::operator*(const vector3<T>& vector) const
	{
		return transform_as_vector(vector);
	}

	template <class T>
	point3<T> matrix_4x4<T>::operator*(const point3<T>& point) const
	{
		return transform_as_point(point);
	}


	template <class T>
	coord3<T> matrix_4x4<T>::transform_as_point(const coord3<T>& c) const
	{
		coord3<T> dest;
		T homogeneous_coord;

		dest.x() = data[LINEAR(0,0)] * c.x() +
			data[LINEAR(0,1)] * c.y() +
			data[LINEAR(0,2)] * c.z() +
			data[LINEAR(0,3)];

		dest.y() = data[LINEAR(1,0)] * c.x() +
			data[LINEAR(1,1)] * c.y() +
			data[LINEAR(1,2)] * c.z() +
			data[LINEAR(1,3)];

		dest.z() = data[LINEAR(2,0)] * c.x() +
			data[LINEAR(2,1)] * c.y() +
			data[LINEAR(2,2)] * c.z() +
			data[LINEAR(2,3)];
		homogeneous_coord = data[LINEAR(3,0)] * c.x() +
			data[LINEAR(3,1)] * c.y() +
			data[LINEAR(3,2)] * c.z() +
			data[LINEAR(3,3)];
		dest /= homogeneous_coord;
		return(dest);
	}

	template <class T>
	coord3<T> matrix_4x4<T>::transform_as_vector(const coord3<T>& c) const
	{
		coord3<T> dest;
		T homogeneous_coord;

		dest.x() = data[LINEAR(0,0)] * c.x() +
			data[LINEAR(0,1)] * c.y() +
			data[LINEAR(0,2)] * c.z();

		dest.y() = data[LINEAR(1,0)] * c.x() +
			data[LINEAR(1,1)] * c.y() +
			data[LINEAR(1,2)] * c.z();

		dest.z() = data[LINEAR(2,0)] * c.x() +
			data[LINEAR(2,1)] * c.y() +
			data[LINEAR(2,2)] * c.z();
		homogeneous_coord = data[LINEAR(3,0)] * c.x() +
			data[LINEAR(3,1)] * c.y() +
			data[LINEAR(3,2)] * c.z();
		//  dest /= homogeneous_coord;
		return(dest);
	}

	template <class T>
	coord3<T> matrix_4x4<T>::transform_as_normal(const coord3<T>& c) const
	{
		// Internal normals are transformed to external normals by the inverse
		// transform matrix...  We do the opposite here, as we are taking external to
		// internal.
		coord3<T> dest;
		T homogeneous_coord;

		dest.x() = data[LINEAR(0,0)] * c.x() +
			data[LINEAR(1,0)] * c.y() +
			data[LINEAR(2,0)] * c.z();

		dest.y() = data[LINEAR(0,1)] * c.x() +
			data[LINEAR(1,1)] * c.y() +
			data[LINEAR(2,1)] * c.z() +

			dest.z() = data[LINEAR(0,2)] * c.x() +
			data[LINEAR(1,2)] * c.y() +
			data[LINEAR(2,2)] * c.z();
		homogeneous_coord = data[LINEAR(0,3)] * c.x() +
			data[LINEAR(1,3)] * c.y() +
			data[LINEAR(2,3)] * c.z();
		//  dest /= homogeneous_coord;
		dest /= dest.length();
		return(dest);
	}


	template <class T>
	coord3<T> matrix_4x4<T>::ext_transform_as_point(const coord3<T>& c) const
	{
		coord3<T> dest;
		T homogeneous_coord;

		dest.x() = inverse_data[LINEAR(0,0)] * c.x() +
			inverse_data[LINEAR(0,1)] * c.y() +
			inverse_data[LINEAR(0,2)] * c.z() +
			inverse_data[LINEAR(0,3)];

		dest.y() = inverse_data[LINEAR(1,0)] * c.x() +
			inverse_data[LINEAR(1,1)] * c.y() +
			inverse_data[LINEAR(1,2)] * c.z() +
			inverse_data[LINEAR(1,3)];

		dest.z() = inverse_data[LINEAR(2,0)] * c.x() +
			inverse_data[LINEAR(2,1)] * c.y() +
			inverse_data[LINEAR(2,2)] * c.z() +
			inverse_data[LINEAR(2,3)];
		homogeneous_coord = inverse_data[LINEAR(3,0)] * c.x() +
			inverse_data[LINEAR(3,1)] * c.y() +
			inverse_data[LINEAR(3,2)] * c.z() +
			inverse_data[LINEAR(3,3)];
		dest /= homogeneous_coord;
		return(dest);
	}

	template <class T>
	coord3<T> matrix_4x4<T>::ext_transform_as_vector(const coord3<T>& c) const
	{
		coord3<T> dest;
		T homogeneous_coord;

		dest.x() = inverse_data[LINEAR(0,0)] * c.x() +
			inverse_data[LINEAR(0,1)] * c.y() +
			inverse_data[LINEAR(0,2)] * c.z();

		dest.y() = inverse_data[LINEAR(1,0)] * c.x() +
			inverse_data[LINEAR(1,1)] * c.y() +
			inverse_data[LINEAR(1,2)] * c.z();

		dest.z() = inverse_data[LINEAR(2,0)] * c.x() +
			inverse_data[LINEAR(2,1)] * c.y() +
			inverse_data[LINEAR(2,2)] * c.z();
		homogeneous_coord = inverse_data[LINEAR(3,0)] * c.x() +
			inverse_data[LINEAR(3,1)] * c.y() +
			inverse_data[LINEAR(3,2)] * c.z();
		//  dest /= homogeneous_coord;
		return(dest);
	}

	template <class T>
	coord3<T> matrix_4x4<T>::ext_transform_as_normal(const coord3<T>& c) const
	{
		// Internal normals are transformed to external normals by the inverse
		// transform matrix...  We do this here.
		coord3<T> dest;
		T homogeneous_coord;

		dest.x() = inverse_data[LINEAR(0,0)] * c.x() +
			inverse_data[LINEAR(1,0)] * c.y() +
			inverse_data[LINEAR(2,0)] * c.z();

		dest.y() = inverse_data[LINEAR(0,1)] * c.x() +
			inverse_data[LINEAR(1,1)] * c.y() +
			inverse_data[LINEAR(2,1)] * c.z();

		dest.z() = inverse_data[LINEAR(0,2)] * c.x() +
			inverse_data[LINEAR(1,2)] * c.y() +
			inverse_data[LINEAR(2,2)] * c.z();
		homogeneous_coord = inverse_data[LINEAR(0,3)] * c.x() +
			inverse_data[LINEAR(1,3)] * c.y() +
			inverse_data[LINEAR(2,3)] * c.z();
		//  dest /= homogeneous_coord;
		dest /= dest.length();
		return(dest);
	}



	template <class T>
	std::ostream& operator << (std::ostream& o, const matrix_4x4<T>& m)
	{
		o << "[";
		o << "[" << m(0,0) << "," << m(0,1) << "," << m(0,2) << "," << m(0,3) << "]";
		//  o << endl;
		o << ",";
		o << "[" << m(1,0) << "," << m(1,1) << "," << m(1,2) << "," << m(1,3) << "]";
		//  o << endl;
		o << ",";
		o << "[" << m(2,0) << "," << m(2,1) << "," << m(2,2) << "," << m(2,3) << "]";
		//  o << endl;
		o << ",";
		o << "[" << m(3,0) << "," << m(3,1) << "," << m(3,2) << "," << m(3,3) << "]";
		o << "]";
		return(o);
	}

	template <typename T>
	std::string inspect(const matrix_4x4<T>& m)
	{
		return std::string("[") +
			"[" + inspect(m(0, 0)) + "," + inspect(m(0, 1)) + "," + inspect(m(0, 2)) + "," + inspect(m(0, 3)) + "],"
			"[" + inspect(m(1, 0)) + "," + inspect(m(1, 1)) + "," + inspect(m(1, 2)) + "," + inspect(m(1, 3)) + "],"
			"[" + inspect(m(2, 0)) + "," + inspect(m(2, 1)) + "," + inspect(m(2, 2)) + "," + inspect(m(2, 3)) + "],"
			"[" + inspect(m(3, 0)) + "," + inspect(m(3, 1)) + "," + inspect(m(3, 2)) + "," + inspect(m(3, 3)) + "]"
			+ "]";
	}

#undef LINEAR

	/* Invert a matrix, if possible.  This is done with the Gauss-Jordan
		Elimination Algorithm.  Currently, it is being done by simulating an
		augmented matrix.

		The augmented matrix is initially set as the identity, and as operations are
		performed to turn the given matrix into the identity, the same operations
		are performed on the augmented portion (turning it into the inverse).
	*/
	template <class T>
	matrix_4x4<T> invert (const matrix_4x4<T>& rktMAT)
	{
		int i, j, k;
		T factor;
		matrix_4x4<T> a = rktMAT;
		// B is the identity, A will be turned into the identity and B the inverse.
		matrix_4x4<T> b = matrix_4x4<T>::identity();
		T temp_row[4];

		for(i = 0; i < 4; ++i)
		{
			// Sort the rows, based on their diagonal elements
			for(j = 0; j < 4; ++j)
			{
				if(fabs(a(i,i)) < fabs(a(j,i)))
				{
					for(k = 0; k < 4; ++k)
					{
						temp_row[k] = a(k,i);
						a(k,i) = a(k,j);
						a(k,j) = temp_row[k];
						temp_row[k] = b(k,i);
						b(k,i) = b(k,j);
						b(k,j) = temp_row[k];
					}
				}
			}
			// If there's a zero on the diagonal, it can't be inverted...
			assert(fabs(a(i,i)) > T(1e-9));

			// Divide to make the element on the diag be 1
			factor = a(i,i);
			for(j = 3; j >= 0; --j)
			{
				a(j,i) = a(j,i) / factor;
				b(j,i) = b(j,i) / factor;
			}

			for(j = i + 1; j < 4; ++j)
			{
				// Turn element i,j into a 0, by subtracting it's value times a known row
				// with a 1 (i) and replacing.
				factor = -a(i,j);
				for(k = 0; k < 4; ++k)
				{
					a(k,j) = a(k,j) + a(k,i) * factor;
					b(k,j) = b(k,j) + b(k,i) * factor;
				}
			}
		}
		for(i = 3; i >= 1; --i)
		{
			for(j = i-1; j >= 0; --j)
			{
				factor = -a(i,j);
				for(k = 0; k < 4; ++k)
				{
					a(k,j) = a(k,j) + a(k,i) * factor;
					b(k,j) = b(k,j) + b(k,i) * factor;
				}
			}
		}
		return b;
	}

} // end namespace amethyst

#endif /* !defined(KH_MAT_4X4_H) */
