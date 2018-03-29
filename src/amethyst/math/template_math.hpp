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
}
