#pragma once
/*
  Revision History:
  04Apr2000 Moved the decls for the jacobi, chebyshev, and conjugate_gradient
            solvers into here from the old utils.h file.
  10Apr2000 Added some include so that I can put this in my graph_math library.
  24May2001 Changed these to be templated...
*/
#include "coord3.hpp"
#include "mat_4x4.hpp"

/* functions to solve a system of equations (3x3 placed in a 4x4 matrix) */
template <class T>
coord3<T> jacobi(const matrix_4x4<T>& non_diagonal,
		 const matrix_4x4<T>& inverted_diagonal,
		 const coord3<T>& right_hand_sides,
		 int max_iterations, T epsilon)
{
  matrix_4x4<T> iteration_matrix = -1 * inverted_diagonal * non_diagonal;
  coord3<T> adjustment_vector = inverted_diagonal * right_hand_sides;
  coord3<T> previous_solution(1,1,1);
  coord3<T> solution(right_hand_sides);
  coord3<T> temp_coord;
  T squared_distance = epsilon * 1000;
  T epsilon_squared = epsilon * epsilon;
  int iteration;
  T barf_homogeneous;
  for(iteration = 0;
      iteration < max_iterations && squared_distance > epsilon_squared;
      ++iteration)
  {
    previous_solution = solution;
    solution = multiply_without_homogeneous_divide(iteration_matrix, solution, barf_homogeneous)
      + adjustment_vector;
    temp_coord = solution - previous_solution;
    squared_distance = dotprod(temp_coord, temp_coord);
  }
#if    defined(KH_DEBUG_IT)
  cout << "solution = " << solution << " after " << iteration << " iterations." << endl;
  printf("jacobi finished after %d iterations (error %1.27f)\n", iteration, sqrt(squared_distance));
#endif /* defined(KH_DEBUG_IT) */
  return(solution);
}


/* The next few functions are for the chebyshev method */
template <class T>
inline void extrapolation(T gamma,
			  const matrix_4x4<T>& iteration_matrix,
			  const coord3<T>& correction_vector,
			  coord3<T>& u,
			  coord3<T>& v)
{
  T barf;
  v = gamma * correction_vector + (1 - gamma) * u;
  //  v = gamma * iteration_matrix * u + v;
  v = multiply_without_homogeneous_divide(iteration_matrix * gamma, u, barf) + v;
}

template <class T>
inline void cheb(T p, T gamma, const matrix_4x4<T>& iteration_matrix,
		 const coord3<T>& correction_vector, coord3<T>& u, coord3<T>& v)
{
  T barf;
  u = p * gamma * correction_vector + p*(1 - gamma) * v + (1 - p) * u;
  //  u = p * gamma * iteration_matrix * v + u;
  u = multiply_without_homogeneous_divide(p * gamma * iteration_matrix, v, barf) + u;
}


template <class T>
coord3<T> chebyshev_acceleration(const matrix_4x4<T>& working_iteration_matrix,
				 const coord3<T>& correction_vector,
				 const coord3<T>& initial_guess,
				 T min_eigen,
				 T max_eigen,
				 int max_iterations,
				 T epsilon)
{
  coord3<T> working_guess = initial_guess; // u
  coord3<T> previous_guess;                // v
  coord3<T> temp_coord;
  T temp;
  T gamma = 2/T(2 - max_eigen - min_eigen);
  temp = T(1/2) * (max_eigen - min_eigen) / (2 - max_eigen - min_eigen);
  T alpha = temp * temp;
  T p;
  int iteration;
  //  T epsilon_squared = epsilon*epsilon;

  extrapolation(gamma, working_iteration_matrix, correction_vector, working_guess, previous_guess);
  p = 1/(1 - 2*alpha);
  cheb(p, gamma, working_iteration_matrix, correction_vector, working_guess, previous_guess);
  for(iteration = 3; iteration < max_iterations; iteration += 2)
  {
    p = (1 - p * alpha);
    cheb(p, gamma, working_iteration_matrix, correction_vector, previous_guess, working_guess);

    p = 1/(1 - p * alpha);
    cheb(p, gamma, working_iteration_matrix, correction_vector, working_guess, previous_guess);

    temp_coord = working_guess - previous_guess;
    //    if(dotprod(temp_coord,temp_coord) < epsilon_squared)
    if((fabs(temp_coord.x()) +
	fabs(temp_coord.y()) +
	fabs(temp_coord.z())) < epsilon)
      break;
  }
#if    defined(KH_DEBUG_IT)
  T squared_distance = dotprod(temp_coord, temp_coord);
  printf("chebyshev finished after %d iterations (error %1.27f)\n", iteration, sqrt(squared_distance));
#endif /* defined(KH_DEBUG_IT) */

  return(working_guess);
}

template <class T>
coord3<T> chebyshev(const matrix_4x4<T>& non_diagonal,
		    const matrix_4x4<T>& inverted_diagonal,
		    const coord3<T>& right_hand_sides,
		    int max_iterations, T epsilon)
{
  matrix_4x4<T> iteration_matrix = -1 * inverted_diagonal * non_diagonal;
  coord3<T> adjustment_vector = inverted_diagonal * right_hand_sides;
  coord3<T> initial_solution(0,0,0);
  return(chebyshev_acceleration(iteration_matrix,
				adjustment_vector,
				initial_solution,
				4, -4,
				max_iterations,
				epsilon));
}
/* End of chebyshev stuff */

/* This next one is incredibly fast.  It does only one matrix multiply per
   iteration, and doesn't take very many iterations to converge.

   NOTE:  This method assumes the matrix it is given is symmetric positive
   definite (SPD {A = transpose(A), transpose(x)*A*x > 0 for all x vectors}).
*/
template <class T>
coord3<T> conjugate_gradient(const coord3<T>& initial_guess,
			     const matrix_4x4<T>& m,
			     const coord3<T>& rhs,
			     int max_iterations,
			     T epsilon)
{
  coord3<T> x = initial_guess;
  coord3<T> r = rhs - m * initial_guess;
  coord3<T> v = r;
  T c = dotprod(r,r);
  int iteration;
  T epsilon_squared = epsilon * epsilon * epsilon;
  for(iteration = 1; iteration <= max_iterations; ++iteration)
  {
    if(dotprod(v,v) < epsilon_squared)
    {
      break;
    }
    coord3<T> z = m * v;
    T t = c / dotprod(v,z);
    x = x + t * v;
    r = r - t * z;
    T d = dotprod(r,r);
    if(d < epsilon_squared)
    {
      break;
    }
    v = r + (d/c) * v;
    c = d;
  }
#if    defined(KH_DEBUG_IT)
  printf("finished in %d iterations\n", iteration);
#endif /* defined(KH_DEBUG_IT) */
  return(x);
}
