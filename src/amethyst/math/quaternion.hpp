#if       !defined(KH_QUATERNION_H)
#define            KH_QUATERNION_H

#include "math/coord3.hpp"
#include "math/vector3.hpp"
#include "math/mat_4x4.hpp"

#include <iosfwd>

//
// A quaternion class.  This is to be used for rotations (and scales, if you
// are VERY careful).  I suggest that scales be done separately (like
// translates).
//
//
//-------------------------------------------------------------------------------
//                       Basic info about quaternions:
//-------------------------------------------------------------------------------
// It is composed of 1 real (r) and 3 'imaginary' (vec imag) parts.
// r    = cos(theta/2)
// imag = sin(theta/2) * k, where k is the vector to rotate about.
//
// This could also be viewed as:
// q0 + qx*I + qy*J + qz*K, where:
// q0=real portion, qx..qz are the imag portions, and...
// I*I=-1, J*J=-1, K*K=-1,
// I*J= K, J*K= I, K*I= J
// J*I=-K, K*J=-I, I*K=-J
// The vector notation, however, is easier to manipulate.
//
// Quaternions are normally written as the real part plus a vector containing
// the imaginary portions: r + imag
//
// The conjugate of a quaternion is found by negating the sign of the 3
// imaginary components.
//
// The dot product (norm) is found by the sum of the squared components:
// norm = real * real + imag dot imag
//
// For a pure rotation, the norm of the quaternion should be equal to 1, or a
// scale will happen (by the square of the scalar applied to the quaternion, if
// it's conjugate has the scalar applied as well).
//
// Quaternion composition (multiplication) is done as follows (for q and p):
// (p.real * q.real - q.imag dot p.imag) +                   <--- real portion
// (q.real * p.imag + p.imag * q.real + q.imag cross p.imag) <--- imag portion
//
// Multiplication of a vector (v) and a quaternion (q) is done by creating a
// quaternion with real portion=0, and imag portion=v, and multiplying as with
// two quaternions:
// (-v dot q.imag) + (q.real * v + v cross q.imag)
//
// Rotation of a vector (v) using a quaternion (q) can be done by:
// q * v * conjugate(q)
// The result is found in the imaginary portion of the resulting quaternion.
//
// Quaternions may be chained together, like rotation matrices:
// v' = q3*(q2*(q1 * v * conjugate(q1))*conjugate(q2))*conjugate(q3)
//    = (q3*q2*q1) * v * conjugate(q3*q2*q1)
// This may be done much faster than multiplying a sequence of matrices out,
// although it does not perform scaling and translation at the same time.
//
//-------------------------------------------------------------------------------
//                       How to easily use this class
//-------------------------------------------------------------------------------
// A quaternion can be created in one of 3 ways:
// 1. Knowing the real and imaginary parts (the real will be 0 for a pure vector).
// 2. A call to 'makeQuaternion', with the angle (degrees), and the vector to
//    rotate about (if the vector is not a unit vector, use
//    'makeUnitQuaternion' instead).
// 3. A call to 'quaternionFromMatrix', with the matrix to extract a quaternion
//    from.  This is not a very fast method.
//
// A vector (or point) can be rotated by a quaternion in one of 3 ways:
// 1. quaternion::rotate, giving it the vector, and the quaternion to rotate
//    it by.
// 2. using a sequence such as q * v * conjugate(q)
//    Where q can be any number of quaternions squashed together by means of
//    multiplication.
// 3. Converting the quaternion into a rotation matrix, and multiplying the
//    vector by that (using the vector's applyTransform function, as of now).
//
// NOTES:
// When working with quaternions, it is best to leave them as quaternions as
// long as possible, as the rotation by nested applications of a quaternion
// rotate is much slower than multiplying out the chain of quaternions, and
// applying the rotate at one time.
//
//
//-------------------------------------------------------------------------------
//                      Functions that this file offers
//-------------------------------------------------------------------------------
// quat = quat
// quat + quat, quat += quat
// quat - quat, quat -= quat
// quat * quat, quat *= quat
// quat * vec,  quat *= vec
// vec  * quat
// quat * scalar
// scalar * quat
// quat.getImag(), quat.setImag(vec)
// quat.getReal(), quat.setReal(scalar)
// quat.set(scalar, vec)
// conjugate(quat),       quat.conjugate()
// dotprod(quat,quat), quat.dotprod(quat)
// quat.norm()
// quat::rotate(vec, scalar, vec)
// quat::rotate(vec, quat)
// operator<<(ostream&,quat)
// makeQuaternion(scalar angle, vec)
// makeUnitQuaternion(scalar angle, vec)
// matrixFromQuaternion(quat)
// quaternionFromMatrix(mat)

namespace amethyst
{
	template <class T>
	class quaternion
	{
	public:
		typedef coord3<T> coord_type;
	private:
		T      real;
		coord_type imag;
	public:

		// Constructors/copy constructors
		inline explicit quaternion(T real = T(0), const coord_type& imag_c = coord_type(0,0,0)):
			real(real), imag(imag_c)
		{
		}
		inline explicit quaternion(T real, const vector3<T>& imag_v):
			real(real), imag(imag_v.x(), imag_v.y(), imag_v.z())
		{
		}
		inline explicit quaternion(const coord_type& imag_c): real(T(0)), imag(imag_c){ }
		inline explicit quaternion(const vector3<T>& imag_v): real(T(0)), imag(imag_v.x(), imag_v.y(), imag_v.z()){ }
		inline quaternion(const quaternion& p): real(p.real), imag(p.imag)    { }

		inline T getReal() const              { return real; }
		inline coord_type getImag() const         { return imag; }
		inline void setReal (T r)             { real = r; }
		inline void setImag (const coord_type& v) { imag = v; }
		inline void set(T r, const coord_type& v) { real = r; imag = v; }

		inline quaternion& operator = (const quaternion& p)
		{
			real = p.real;
			imag = p.imag;
			return(*this);
		}

		inline quaternion& operator += (const quaternion& p)
		{
			real += p.real;
			imag += p.imag;
			return(*this);
		}

		inline quaternion& operator -= (const quaternion& p)
		{
			real -= p.real;
			imag -= p.imag;
			return(*this);
		}

		inline quaternion& operator *= (const quaternion& p)
		{
			T new_real = real * p.real - amethyst::dotprod(imag, p.imag);
			imag = real * p.imag + imag * p.real + amethyst::crossprod(imag,p.imag);
			real = new_real;
			return(*this);
		}

		inline quaternion& operator *= (const coord_type& v)
		{
			T new_real =  - amethyst::dotprod(imag, v);
			imag = real * v + amethyst::crossprod(imag, v);
			real = new_real;
			return(*this);
		}
		inline quaternion& operator *= (const vector3<T>& v)
		{
			return this->operator*=(coord_type(v.x(), v.y(), v.z()));
		}
		inline quaternion& operator *= (T scalar)
		{
			real *= scalar;
			imag *= scalar;
			return(*this);
		}
		inline quaternion& operator /= (T scalar)
		{
			real /= scalar;
			imag /= scalar;
			return(*this);
		}


		inline friend quaternion operator*(const quaternion& q1, const quaternion& q2)
		{
			quaternion temp(q1);
			return temp *= q2;
		}
		inline friend quaternion operator*(const quaternion& q1, const coord_type& v2)
		{
			quaternion temp(q1);
			return temp *= v2;
		}
		inline friend quaternion operator*(const coord_type& v1, const quaternion& q2)
		{
			quaternion temp(0,v1);
			return temp *= q2;
		}
		inline friend quaternion operator*(T scalar, const quaternion& q2)
		{
			quaternion temp(q2);
			return temp *= scalar;
		}
		inline friend quaternion operator*(const quaternion& q1, T scalar)
		{
			quaternion temp(q1);
			return temp *= scalar;
		}
		inline friend quaternion operator/(const quaternion& q1, T scalar)
		{
			quaternion temp(q1);
			return temp /= scalar;
		}
		inline friend quaternion operator+(const quaternion& q1, const quaternion& q2)
		{
			quaternion temp(q1);
			return temp += q2;
		}
		inline friend quaternion operator-(const quaternion& q1, const quaternion& q2)
		{
			quaternion temp(q1);
			return temp -= q2;
		}
		// unary -
		inline friend quaternion operator-(const quaternion& q)
		{
			return quaternion(-q.real, coord_type(-q.imag.x(),-q.imag.y(),-q.imag.z()));
		}

		inline quaternion conjugate (void) const
		{
			return quaternion (real, coord_type (-imag.x(), -imag.y(), -imag.z()));
		}

		inline T dotprod(const quaternion& p) const
		{
			return real * p.real + amethyst::dotprod(imag, p.imag);
		}

		inline T norm (void) const
		{
			return sqrt(this->dotprod(*this));
		}

		static inline coord_type rotate(const coord_type& point, T angle, const coord_type& line)
		{
			quaternion temp(line, angle);
			temp /= temp.norm();
			quaternion rotated = temp * point * temp.conjugate();
			return rotated.getImag();
		}
		static inline coord_type rotate(const coord_type& v, const quaternion& q)
		{
			quaternion temp = q * v * q.conjugate();
			return temp.getImag();
		}
		static inline coord_type rotate(const vector3<T>& v, const quaternion& q)
		{
			quaternion temp = q * v * q.conjugate();
			return temp.getImag();
		}
		static inline coord_type rotate(const point3<T>& p, const quaternion& q)
		{
			quaternion temp = q * p * q.conjugate();
			return temp.getImag();
		}

		friend std::ostream& operator<<(std::ostream& o, const quaternion& q)
		{
			o << "{" << q.real << "," << q.imag << "}";
			return o;
		}
	};


	template <class T>
	quaternion<T> operator*(const quaternion<T>& q, const vector3<T>& v)
	{
		quaternion<T> temp(q);
		return temp *= v;
	}

	template <class T>
	quaternion<T> operator*(const vector3<T>& v, const quaternion<T>& q)
	{
		quaternion<T> temp(0, v);
		return temp *= q;
	}

	// The dot product of two quaternions.
	template <class T>
	inline T dotprod(const quaternion<T>& q1, const quaternion<T>& q2)
	{
		return q1.dotprod(q2);
	}


	// The conjugate of a quaternion.
	template <class T>
	inline quaternion<T> conjugate(const quaternion<T>& q)
	{
		return q.conjugate();
	}

	// Make a quaternion to represent the rotation about the given vector (line),
	// by the angle given (degrees).
	template <class T>
	inline quaternion<T> makeQuaternion(T angle, const coord3<T>& line)
	{
		angle *= M_PI / 180;
		T real = cos(angle / 2);
		return quaternion<T>(real, sin(angle/2) * line);
	}


	// Create a quaternion, as above, however, the vector is normalized before use.
	template <class T, class vector_type>
	inline quaternion<T> makeUnitQuaternion(T angle, const vector_type& line)
	{
		angle *= M_PI / 180;
		T real = cos(angle / 2);
		T norm = length(line);

		assert(norm > T(0));

		return quaternion<T>(real, sin(angle/2) * line / norm);
	}

	// Create a homogeneous rotation matrix from the given quaternion.
	template <class T>
	matrix_4x4<T> matrixFromQuaternion(const quaternion<T>& q)
	{
		// The method of converting a quaternion into a rotation matrix is:
		//  R = ((real*real - imag dot imag) * I +
		//       2 * imag[0] * S(imag) +
		//       2 * imag * transpose(imag))
		// Where S(imag) is the 'skew-symmetric' matrix, which represents the cross
		// product by the given vector.
		// This skew symmetric matrix looks like:
		// [  0  -k3  k2 ]
		// [  k3   0 -k1 ]
		// [ -k2  k1  0  ]
		// Where k1..3 are the 3 components of the vector being rotated around.

		matrix_4x4<T> mat;
		coord3<T>   v = q.getImag();
		T           r = q.getReal();
		T zero = T(0);
		T one  = T(1);
		T two  = T(2);

		mat(0,0) = one - two * (v[1] * v[1] + v[2] * v[2]);
		mat(0,1) =       two * (v[0] * v[1] -    r * v[2]);
		mat(0,2) =       two * (v[0] * v[2] +    r * v[1]);
		mat(0,3) = zero;

		mat(1,0) =       two * (v[0] * v[1] +    r * v[2]);
		mat(1,1) = one - two * (v[0] * v[0] + v[2] * v[2]);
		mat(1,2) =       two * (v[1] * v[2] -    r * v[0]);
		mat(1,3) = zero;

		mat(2,0) =       two * (v[0] * v[2] -    r * v[1]);
		mat(2,1) =       two * (v[1] * v[2] +    r * v[0]);
		mat(2,2) = one - two * (v[0] * v[0] + v[1] * v[1]);
		mat(2,3) = zero;

		mat(3,0) = zero;
		mat(3,1) = zero;
		mat(3,2) = zero;
		mat(3,3) = one;

		return mat;
	} /* matrixFromQuaternion() */



	// Create a quaternion from the rotation portion of a matrix.  There are some
	// assumptions made about the matrix:
	// 1. The matrix is only composed of rotations and translates (no skew, or
	//    strange warping).
	// 2. The first 3x3 sub-matrix is a true rotation matrix, meaning that
	//    multiplying it by it's transpose should result in a 3x3 identity matrix.
	//
	// Special care is taken to make sure that the cases for rotation by 0 and by
	// PI are handled correctly.
	template <class T>
	quaternion<T> quaternionFromMatrix(const matrix_4x4<T>& m)
	{
		T q_squared_values[4];
		T          real;
		coord3<T>  imag;

		T r11 = m(0,0);
		T r22 = m(1,1);
		T r33 = m(2,2);

		// The squared values of each component (0 == real, 1..3 = imag)
		q_squared_values[0] = (1 + r11 + r22 + r33) / 4;
		q_squared_values[1] = (1 + r11 - r22 - r33) / 4;
		q_squared_values[2] = (1 - r11 + r22 - r33) / 4;
		q_squared_values[3] = (1 - r11 - r22 + r33) / 4;

		// Search for the largest squared component.  This is to be used in solving
		// for the other components, so that a valid answer can be found for evil
		// rotations (0 and PI radians).
		int MaxQ_index = 0;
		T MaxQ_squared = q_squared_values[0];
		for(int i = 1; i < 4; ++i)
		{
			if(q_squared_values[i] > MaxQ_squared)
			{
				MaxQ_index = i;
				MaxQ_squared = q_squared_values[i];
			}
		}

		// Now that the largest component has been found (square root of the largest
		// found above), find the rest of them using the following formulas (derived
		// from a real ugly matrix):
		//  r * q0 = (r21 - r12) / 4
		//  r * q1 = (r02 - r20) / 4
		//  r * q2 = (r10 - r01) / 4
		// q0 * q1 = (r01 + r10) / 4
		// q0 * q2 = (r02 + r20) / 4
		// q1 * q2 = (r12 + r21) / 4
		// Where r is the real component, and q0..q2 are the imaginary components
		switch(MaxQ_index)
		{
		case 0:
			real = sqrt(q_squared_values[0]);
			imag[0] = (m(2,1) - m(1,2)) / (4 * real);
			imag[1] = (m(0,2) - m(2,0)) / (4 * real);
			imag[2] = (m(1,0) - m(0,1)) / (4 * real);
			break;
		case 1:
			imag[0] = sqrt(q_squared_values[1]);
			real    = (m(2,1) - m(1,2)) / (4 * imag[0]);
			imag[1] = (m(0,1) + m(1,0)) / (4 * imag[0]);
			imag[2] = (m(0,2) + m(2,0)) / (4 * imag[0]);
			break;
		case 2:
			imag[1] = sqrt(q_squared_values[2]);
			real    = (m(0,2) - m(2,0)) / (4 * imag[1]);
			imag[0] = (m(0,1) + m(1,0)) / (4 * imag[1]);
			imag[2] = (m(1,2) + m(2,1)) / (4 * imag[1]);
			break;
		case 3:
			imag[2] = sqrt(q_squared_values[3]);
			real    = (m(1,0) - m(0,1)) / (4 * imag[2]);
			imag[0] = (m(0,1) + m(1,0)) / (4 * imag[2]);
			imag[1] = (m(1,2) + m(2,1)) / (4 * imag[2]);
			break;
		default:
			assert(("FOOBAR IN CONVERTING MATRIX TO QUATERNION!!!"!=NULL));
			break;
		}

		return quaternion<T>(real,imag);
	} /* quaternionFromMatrix() */

	template <typename T>
	std::string inspect(const quaternion<T>& q)
	{
		return "{" + inspect(q.getReal()) + "," + inspect(q.getImag()) + "}";
	}

} // end namespace amethyst

#endif /* !defined(KH_QUATERNION_H) */


