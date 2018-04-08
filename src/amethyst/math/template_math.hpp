#pragma once
namespace amethyst
{
    template <typename T>
    T tfabs(const T& t)
    {
        if (t < 0)
        {
            return -t;
        }
        return t;
    }

    // Integer division where the result is rounded up instead of down.
    template <typename T>
    inline T div_ceil(T x, T y)
    {
        return (x / y) + T((x % y != 0) ? !((x > 0) ^ (y > 0)) : 0);
    }
}
