#pragma once

#include <stdexcept>

namespace amethyst
{

    /**
     * Thrown to indicate a size error (for assignment of arrays, etc).
     */
    class size_mismatch : public std::runtime_error
    {
    public:
        explicit size_mismatch(const std::string& arg) : std::runtime_error(arg) { }
    };

    using std::out_of_range;
}