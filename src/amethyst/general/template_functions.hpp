#pragma once

/**
 * template_functions.hpp -- Functions which do not exist in all compilers,
 * and I have found to be important.
 */

/*
   Revision History:
   02Nov1999 Created this file.
   ??Jun2001 Added the clamp, and sort functions.
   08Jan2002 Changed all references to objects in namespace std to be fully
   qualified without polluting the global namespace. Also added
   comments where they did not exist.
   01Mar2003 Changed comments to work with both doc++ and ccdoc.
 */

#include <ostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <list>

namespace std // for ADL
{
    /**
     * Send a pair to a stream with trivial formatting < a,b >
     */
    template <class T1, class T2>
    ostream& operator<<(ostream& o, const pair<T1, T2>& p)
    {
        o << "< " << p.first << "," << p.second << " >";
        return o;
    }

    /**
     * Send a vector to a stream with simple formatting {  } or { a } or
     * { a, b, ..., d }
     */
    template <class T>
    ostream& operator<<(ostream& o, const vector<T>& vt)
    {
        int i;
        o << "{ ";
        // output all of the elements that need a trailing comma (if any)
        for (i = 0; i < int(vt.size()) - 1; ++i)
        {
            o << vt[i];
            o << ", ";
        }
        // print out the remaining elements (if any)
        if (i < int(vt.size()))
        {
            o << vt[i];
        }
        o << " }";
        return o;
    }
}

namespace amethyst
{
    /**
     * Return the max of two items (by means of the > operator)
     * Some implementations have 'max' or 'MAX' defined as a macro, but it is
     * sometimes useful to take the address of such a function.
     * USE THIS INSTEAD (assuming that your compiler can take the address of a
     * templated function).
     */
    template <class T>
    T my_max(const T& t1, const T& t2)
    {
        if (t1 > t2) {
            return t1;
        }
        else {
            return t2;
        }
    }

    /**
     * Return the min of two items (by means of the < operator)
     * Some implementations have 'min' or 'MIN' defined as a macro, but it is
     * sometimes useful to take the address of such a function.
     * USE THIS INSTEAD (assuming that your compiler can take the address of a
     * templated function).
     */
    template <class T>
    T my_min(const T& t1, const T& t2)
    {
        if (t1 < t2) {
            return t1;
        }
        else {
            return t2;
        }
    }


    /**
     * Return the absolute value of an item (uses the < operator and a conversion
     * to the required type from an int)
     */
    template <class T>
    T my_abs(const T& t1)
    {
        T ret_val = t1;
        if (t1 < T(0))
        {
            ret_val *= T(-1);
        }
        return ret_val;
    }

    /**
     * Return the sign of a given item (by use of the < operator and a conversion
     * from int)
     */
    template <class T>
    int my_sign(const T& t)
    {
        if (t < T(0)) {
            return -1;
        }
        else {
            return 1;
        }
    }

    /**
     * Return the square of an item (by use of the binary * operator)
     */
    template <class T>
    inline T my_sqr(const T& t)
    {
        return t * t;
    }

    /**
     * Swap two items (the assignment operator MUST be functional)
     */
    template <class T>
    void my_swap(T& t1, T& t2)
    {
        T temp = t1;
        t1 = t2;
        t2 = temp;
    }

    /**
     * Fill a vector with a given value.  This is intended for compilers that do
     * not already provide this.
     */
    template <class T>
    void fill_vector(std::vector<T>& vec, const T& val)
    {
        typename std::vector<T>::iterator i;
        for (i = vec.begin(); i != vec.end(); ++i) {
            *i = val;
        }
    }

    /**
     * Clamp a value to be between a min and a max (uses the > and < operators)
     */
    template <class T>
    void clamp(T& t, T tmin, T tmax)
    {
        if (t < tmin) {
            t = tmin;
        }
        else if (t > tmax) {
            t = tmax;
        }
    }

    /**
     * Functions to allow sorting of containers even when the stl algorithm does
     * not provide this equally for all types (namely lists).
     */
     /** Sort any random container. */
    template <class container>
    void sort(container& c)
    {
        /// An external sort function exists in namespace std (through algorithm)
        std::sort(c.begin(), c.end());
    }

    /** Sort the specific container list. */
    template <class T>
    void sort(std::list<T>& l)
    {
        /// An external sort function does not exist.  Call the member sort function.
        l.sort();
    }

    template <typename Container, typename T>
    Container append_copy(Container c, T&& t)
    {
        c.emplace_back(std::forward<T>(t));
        return c;
    }
}