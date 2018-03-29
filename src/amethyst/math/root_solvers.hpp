#pragma once

 /*
   root_solvers.h -- Generic (templated) functions for use in solving equations
   either with polynomials, or with unknown functions/functors
   producing data.
 */
/*
   NOTES: All polynomials used in any functions within this file are assumed to
   be in vectors of coefficients where the index into the vector indicates the
   power of corresponding term.
   Written generically:
   poly = c[n] * x^n + c[n-1] * x^(n-1) + ... + c[1] * x + c[0]

   for example:
   c[0] = 1; c[1] = 2; c[2] = -1;
   would be: -1 * x^2 + 2 * x^1 + 1 * x^0
          == -1*x^2 + 2*x + 1
          == -x*x   + 2*x + 1
 */

/*
   Revision History:
    12Aug2000 [Kevin Harris] Wrote the solver for generic functions
    13Aug2000 [Kevin Harris] Wrote the solver for polynomials
 */

/*
   Everything in here is templated so that it could work with any base type
   (built-in or user-defined).

   Useful functions provided here and the minimum amount they need to work (they
   may take more paramaters which have been defaulted [see below]):

   Functions taking function pointers or functors:
    est_deriv: Estimate the derivative of the given function at the given x
       Requires function/functor [dat], location [x]
       Returns the estimated derivative
       [generic]
    est_second_deriv: Estimate the second derivative of the given function at
       the given x
       Requires function/functor [dat], location [x]
       Returns the estimated second derivative
       [generic]
    solve: Search for some (not likely to be all) of the zeros in the equation
       dat(x) = value
       Requires min and max values for a likely (or desired) search range
                [min,max], function/functor [dat], value [value]
       Returns vector of zero points
       [generic]

   Functions working with polynomials:
    horner_polynomial_evaluate: Evaluate a polynomial at the given location
       Requires location [x], coef vector [hp]
       Returns value of the poly at the point
       [poly]
    horner_factor_out_zero: Factor out a term of zero (does a form of
       'synthetic division').
       Requires location [zero], coef vector [hp], reference to store remainder
                value [value]
       Returns the coefficients of the polynomial (quotient) after the division
       [poly]
    horner_deriv_evaluate: Evaluate the polynomial to get the value and the
       value of its derivative at the given point.
       Requires location [x], coef vector [hp], reference to store deriv
                [deriv_value]
       Returns the evaluated value
       [poly]
    remove_useless_poly_zeros: Removes any useless zeros from a polynomial
       (zeroed coefficients in the highest order terms)
       Requires coef vector [hp]
       Returns  vector with no useless zeros (may be identical to hp)
       [poly]
    zero_disk_radius: Returns the radius of the disk, centered at the origin,
       in which all of the zeros of the polynomial will lie (real and
       complex).
       Requires coef vector [hp]
       Returns  radius of disk
       [poly]
    poly_solve: Solve a polynomial to find all (or almost all) of the zeros.
       Requires coef vector [hp]
       Returns  vector of zeros
       [poly]

   Functions working with simple numbers:
    tfabs: absolute value (given any number, it will return a positive number
       of the same magnitude)
       Requires number [b]
       Returns  absolute value
 */
/*
   Preproccessor directives which may alter execution or compile-time options:
   USELESS_DEBUG -- prints out some debug which will be useless for most people
                   unless they are doing debugging and want to know what's
           going on inside *some* of the functions in this file.
   FUNCTIONAL_FUNCTION_TEMPLATE_ADDRESS -- Allows a complex version of the
                   generic solve function to be used.  This version supports
           different functions/functors to be used as 'guessers' when
           solving an equation in an iterative fashion.
   NEWTON_SOLVER -- Instructs the generic solver (if
                   FUNCTIONAL_FUNCTION_TEMPLATE_ADDRESS is not defined) to use
           the newton guesser instead of the default secant guesser.
           The secant guesser is slightly faster due to less evaluation
           of the input function (no derivitive estimate required).
 */

#include "template_math.hpp"

#include <vector>
#if       defined(USELESS_DEBUG)
#include <stdio.h>
#endif /* defined(USELESS_DEBUG) */

namespace amethyst
{

    // A method of estimating the derivitive of a function.
    template <class base_type, class data_fn>
    inline base_type est_deriv(data_fn dat, base_type x, base_type epsilon = base_type(double(1e-13)))
    {
        // The method I will be using to estimate a derivitive will be:
        // F'(x) = F(x+h) - F(x-h)
        //         ---------------
        //               2*h
        return (dat(x + epsilon) - dat(x - epsilon)) / (2 * epsilon);
    }

    // A method of estimating the second derivitive of a function.
    template <class base_type, class data_fn>
    inline base_type est_second_deriv(data_fn dat, base_type x,
                                      base_type epsilon = base_type(double(1e-13)))
    {
        // The method I will be using to estimate a second derivitive will be:
        // F''(x) = F(x + h) - 2 * F(x) + F(x - h)
        //          ------------------------------
        //                       h^2
        return (dat(x + epsilon) - 2 * dat(x) + dat(x - epsilon)) / (epsilon * epsilon);
    }


    // A direct implementation of newton's method
    template <class base_type, class data_fn>
    inline base_type newton_next_guess(base_type x, base_type prev_x,
                                       data_fn dat, base_type value,
                                       base_type epsilon,
                                       base_type val_x, base_type val_prev_x)
    {
        // Newton's method:
        // x = x - F(x) / F'(x)
        return x - val_x / est_deriv(dat, x, epsilon);
    }


    // A method based on newton's method which is slightly faster due to having
    // less evaluations of the function for each iteration of the solver (this is
    // because the derivative is not required).
    template <class base_type, class data_fn>
    inline base_type secant_next_guess(base_type x, base_type prev_x,
                                       data_fn dat, base_type value,
                                       base_type epsilon,
                                       base_type val_x, base_type val_prev_x)
    {
        // Secant method:
        // Xn+1 = Xn - F(Xn) * [ (Xn - Xn-1) / (F(Xn) - F(Xn-1)) ]
        return x - val_x * (x - prev_x) / (val_x - val_prev_x);
    }


    template <class base_type, class data_fn, class guess_fn>
    vector<base_type> simple_roots(base_type min, base_type max, data_fn dat,
                                   base_type value,
                                   guess_fn get_next_guess,
                                   base_type epsilon = base_type(double(1e-13)),
                                   vector<base_type> guesses = vector<base_type>(),
                                   int est_roots = 5,
                                   int max_iterations = 100,
                                   bool sort_roots = true)
    {
        vector<base_type> found_roots;

        base_type x;
        base_type next_x;
        base_type prev_x;

        // Make some initial guesses if they did not provide enough
        int guesses_to_make = est_roots - (int)guesses.size();
        if (guesses_to_make > 0)
        {
            base_type x_per_root = (max - min) / guesses_to_make;

            for (int guess = 0; guess < guesses_to_make; ++guess)
            {
                guesses.push_back(min + x_per_root * guess);
            }
        }

        for (int root_number = 0; root_number < est_roots; ++root_number)
        {
            // Assign some value to x
            x = guesses[root_number];
            prev_x = x - epsilon;

            base_type val_at_x;
            base_type val_prev_x = dat(prev_x) - value;

            for (int iteration = 0; iteration < max_iterations; ++iteration)
            {
                val_at_x = dat(x) - value;
                next_x = get_next_guess(x, prev_x, dat, value, epsilon, val_at_x, val_prev_x);

                // if fabs(val_at_x) < epsilon....
                if ((val_at_x < epsilon) && (val_at_x > -epsilon))
                {
                    // Root at x.
#if       defined(USELESS_DEBUG)
                    printf("Found root at %f, iteration %d, root #%d (shifted=%f)\n",
                           double(x), iteration, root_number, double(val_at_x));
#endif /* defined(USELESS_DEBUG) */
                    found_roots.push_back(x);
                    break; // Don't continue the inner (iteration) loop.
                }

                base_type x_diff = next_x - x;

                // if fabs(x_diff) < epsilon, and we've done more than half of the
                // iterations, we must be stuck...
                if ((x_diff < epsilon) &&
                    (x_diff > -epsilon) &&
                    (iteration > max_iterations / 2))
                {
                    // Not a root, but we are stuck at some kind of local [min/max]ima.
                    // Some kind of 'warping' should be done to find a new spot to
                    // continue.
#if       defined(USELESS_DEBUG)
                    printf("WARP!!! x=%f, Iteration=%d, diff=%1.16f\n", double(x),
                           iteration, double(x_diff));
#endif /* defined(USELESS_DEBUG) */

                    // Some random warping... This may get a useful root, and may not...
                    // It's better than sitting around and letting the iterations run out,
                    // or just bailing.
                    next_x = guesses[root_number] * x - min;
                }
                // Change x.
                prev_x = x;
                val_prev_x = val_at_x;
                x = next_x;
            } // for each iteration
        } // for each root_number


        // Now... go through the roots and make sure they are in the range requested
        for (unsigned root = 0; root < found_roots.size(); ++root)
        {
            if ((found_roots[root] < min) || (found_roots[root] > max))
            {
                found_roots.erase(found_roots.begin() + root);
                // move the root number backwards, as the root needs to be checked
                // again.
                --root;
            }
        }

        if (sort_roots)
        {
            // Sort the roots
            if (found_roots.size() >= unsigned(1))
            {
                // More than one root... They can be sorted.
                // Do a dumb bubble sort.
                base_type temp;
                for (unsigned r1 = 0; r1 < (found_roots.size() - 1); ++r1)
                {
                    for (unsigned r2 = r1 + 1; r2 < found_roots.size(); ++r2)
                    {
                        if (found_roots[r2] < found_roots[r1])
                        {
                            // Swap...
                            temp = found_roots[r1];
                            found_roots[r1] = found_roots[r2];
                            found_roots[r2] = temp;
                        }
                    } // for r2
                } // for r1
            } // if there is more than 1 root
        } // if the roots need sorting

        return found_roots;
    }

    //
    // At least egcs-2.91.66 cannot correctly support taking the address of certain
    // templated functions for use as default parameters for templated parameters
    // (possibly many more situations).  It is likely that many more compilers
    // cannot support it.  Until they do, there is this slightly simpler version
    // which only supports a single solver function.
    //
#if !defined(FUNCTIONAL_FUNCTION_TEMPLATE_ADDRESS)
    //
    // Given the desired interval, attempt to find roots (x) within that interval
    // for the equation dat(x)=value.  Since they did not provide a solver method,
    // use the simple_roots method with the newton solver.
    //
    template <class base_type, class data_fn>
    vector<base_type> solve(base_type min, base_type max, data_fn dat,
                            base_type value,
                            base_type epsilon = base_type(double(1e-13)),
                            const vector<base_type>& guesses = vector<base_type>(),
                            int num_roots = 5)
    {
        return simple_roots<base_type, data_fn>(min, max, dat, value,
#if       defined(AMETHYST_SECANT_SOLVER)
                                                &secant_next_guess<base_type, data_fn>,
#else
                                                &newton_next_guess<base_type, data_fn>,
#endif /* defined(AMETHYST_SECANT_SOLVER) */

                                                epsilon, guesses, num_roots);
    }
#else
    //
    // Given the desired interval, attempt to find roots (x) within that interval
    // for the equation dat(x)=value.  Solutions are found by using the solve_fn
    // method.
    // I don't know if this is exactly correct, according to the C++ standard, but
    // it looks loke it should work if the compiler is functional.
    //
    template <class base_type, class data_fn, class guess_fn, class solve_fn>
    vector<base_type> solve(base_type min, base_type max, data_fn dat,
                            base_type value,
                            guess_fn get_next_guess = & newton_next_guess<base_type, data_fn>,
                            base_type epsilon = base_type(double(1e-13)),
                            solve_fn method = & simple_roots<base_type, data_fn, guess_fn>,
                            const vector<base_type>& guesses = vector<base_type>(),
                            int num_roots = 5)
    {
        return method(min, max, dat, value, get_next_guess, epsilon, guesses, num_roots);
    }
#endif /* !defined(FUNCTIONAL_FUNCTION_TEMPLATE_ADDRESS) */


    //
    // Evaluate a polynomial (hp) using Horner's algorithm.  This is the minimal
    // way to evaluate a polynomial (in terms of adds and multiplies).
    //
    template <class base_type>
    base_type horner_polynomial_evaluate(base_type x, const vector<base_type>& hp)
    {
        base_type retval = 0;
        int order = int(hp.size()) - 1;

        if (order >= 0)
        {
            retval = hp[order];

            for (int i = order - 1; i >= 0; --i)
            {
                retval = retval * x + hp[i];
            }
        }
        return retval;
    }

    //
    // Factor out a term of zero from a polynomial.
    // The returned vector is the polynomial r(x):
    // hp(x) = (x - zero) * r(x) + value
    //
    // r(x) = hp(x) - hp(zero)
    //        ----------------
    //            x - zero
    // With the value being set to the value of the polynomial at the 'zero' point
    // (should be zero if it is a true zero, will be something else if the 'zero'
    // is not a true zero).
    //
    template <class base_type>
    vector<base_type> horner_factor_out_zero(base_type zero,
                                             const vector<base_type>& hp,
                                             base_type& value)
    {
        vector<base_type> r;
        int terms = int(hp.size());
        int new_terms = terms - 1;
        value = base_type(0);

        if (terms > 1)
        {
            r.resize(new_terms, base_type(0));

            r[new_terms - 1] = hp[new_terms];
            int i;
            for (i = new_terms - 1; i > 0; --i)
            {
                r[i - 1] = hp[i] + zero * r[i];
            }
            if (i >= 0)
            {
                value = hp[0] + zero * r[0];
            }
        }
        return r;
    }

    //
    // Evaluate the polynomial and it's derivative at the given point (this is for
    // use in something like newton's method).
    //
    template <class base_type>
    base_type horner_deriv_evaluate(base_type x, const vector<base_type>& hp,
                                    base_type& deriv_value)
    {
        // Zero out the initial values.
        base_type retval = deriv_value = 0;
        int terms = hp.size();
        if (terms > 0)
        {
            base_type deriv = 0;
            retval = hp[terms - 1];
            for (int i = terms - 2; i >= 0; --i)
            {
                deriv = retval + x * deriv;
                retval = hp[i] + x * retval;
            }
            deriv_value = deriv;
        }
        return retval;
    }

    //
    // Remove useless (leading) zeros on a polynomial.  These are all zeros of
    // terms for which there are no higher order terms containing non-zero
    // coefficients
    //
    template <class base_type>
    vector<base_type> remove_useless_poly_zeros(const vector<base_type>& hp,
                                                base_type epsilon = base_type(double(1e-13)))
    {
        int i = hp.size();
        while ((i > 0) && (tfabs(hp[i - 1]) < epsilon))
        {
            --i;
        }
        return vector<base_type>(hp.begin(), hp.begin() + i);
    }


    //
    // Find the disk which contains all zeros for the given polynomial (this will
    // find the radius of the disk centered at the origin which contains all real
    // and complex zeros).
    //
    template <class base_type>
    base_type zero_disk_radius(const vector<base_type>& hp)
    {
        // The radius is given by:
        // rad = 1 + |An|^-1 * max |Ak|
        //                   0<=k<n
        // Where Ax is the coeficient #x in the polynomial.
        base_type radius = base_type(0);
        if (hp.size() > 0)
        {
            base_type high_order_term = tfabs(hp[hp.size() - 1]);

            base_type largest_low_order = 0;

            if (hp.size() > 1)
            {
                for (unsigned i = 0; i < (hp.size() - 1); ++i)
                {
                    base_type cur_term = tfabs(hp[i]);
                    if (cur_term > largest_low_order)
                    {
                        largest_low_order = cur_term;
                    }
                }
            }
            else
            {
                largest_low_order = high_order_term;
            }
            radius = 1 + largest_low_order / high_order_term;
        }
        return radius;
    }

    //
    // Find all (possibly not all if things go bad) of the zeros for the given
    // polynomial.  This uses a form of newton's method and horner's algorithm to
    // find the zeros.
    //
    template <class base_type>
    vector<base_type> poly_solve(const vector<base_type>& hp,
                                 int max_iterations = 50,
                                 base_type epsilon = base_type(double(1e-13)),
                                 int tries_per_root = 5)
    {
        vector<base_type> zero_vec;
        vector<base_type> working_vec(remove_useless_poly_zeros(hp, epsilon));
        base_type disk_radius = zero_disk_radius(working_vec);

#if       defined(USELESS_DEBUG)
        printf("Radius is %f\n", disk_radius);
#endif /* defined(USELESS_DEBUG) */

        base_type min = -disk_radius;
        base_type max = disk_radius;
        base_type space_per_try = (2 * disk_radius) / (tries_per_root - 1);

        int tries_left;

        // Remove any zeros from the end of the working vec (all high-order zeros).
        while ((working_vec.size() > 0) &&
               (tfabs(working_vec[working_vec.size() - 1]) < epsilon))
        {
            working_vec.resize(working_vec.size() - 1);
        }

        // As long as there is still a polynomial...
        while (working_vec.size() > 1)
        {
            base_type x = max;
            base_type new_x = min;
            bool found_solution = false;

            // Make 'tries_per_root' attempts at finding a root for the currrent
            // working poly before giving up.
            for (tries_left = tries_per_root - 1;
                 (tries_left >= 0) && (!found_solution);
                 --tries_left)
            {
                new_x = space_per_try * tries_left + min;

                for (int i = 0; i < max_iterations; ++i)
                {
                    x = new_x;
                    base_type x_value;
                    base_type deriv_value;
                    x_value = horner_deriv_evaluate(x, working_vec, deriv_value);
                    new_x = x - x_value / deriv_value;

                    base_type xdiff = x - new_x;

                    if (tfabs(xdiff) < epsilon)
                    {
                        found_solution = true;
                        break;
                    }
                } // for all iterations
            } // while there are more tries for the root...

            // If there was not a zero found...
            if (tfabs(x - new_x) > epsilon)
            {
                // Couldn't find a zero... Terminate.
#if       defined(USELESS_DEBUG)
                printf("non-zero termination!!!\n");
#endif /* defined(USELESS_DEBUG) */
                break;
            }

            // Add the zero to the collection, and factor it out of the polynomial.
            zero_vec.push_back(x);
            working_vec = horner_factor_out_zero(x, working_vec, new_x);
        } // While there is still some polynomial left to work with...


        return zero_vec;
    }
}

