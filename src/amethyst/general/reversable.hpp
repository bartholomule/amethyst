#pragma once

#include <iterator>

namespace amethyst
{
    template <typename T>
    struct reversable
    {
        T& obj;
    };

    template <typename T>
    auto begin(reversable<T>& t)
    {
        using std::begin;
        return begin(t.obj);
    }

    template <typename T>
    auto end(reversable<T>& t)
    {
        using std::end;
        return end(t.obj);
    }

    template <typename T>
    reversable<T> reverse(T& t)
    {
        return { t };
    }
}