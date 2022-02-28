#pragma once

#include "platform.hpp"
#include "file_descriptor.hpp"
#include <istream>
#include <ostream>
#include <iostream>

namespace amethyst
{
    class fd_ostreambuf;

    /**
     * A simple class for creating an output stream from a file descriptor.
     */
    class fd_ostream : public std::ostream
    {

    public:
        explicit fd_ostream(file_handle_type fd = get_std_handle(1))
            : fd_ostream(file_descriptor(fd))
        {
        }
        fd_ostream(file_descriptor fd);
        fd_ostream(fd_ostream&& old) noexcept;
        fd_ostream& operator=(fd_ostream&& old) noexcept;
        virtual ~fd_ostream();
        fd_ostream(const fd_ostream& old) = delete;
        fd_ostream& operator=(const fd_ostream& old) = delete;
    protected:
        fd_ostreambuf* buf;
    };

    class fd_istreambuf;

    /**
     * A simple class for creating an input stream from a file descriptor.
     * This may be merged with the above stream, but for now is separate to keep the implementation trivial.
     */
    class fd_istream : public std::istream
    {
    public:
        explicit fd_istream(file_handle_type fd = get_std_handle(0))
            : fd_istream(file_descriptor(fd))
        {
        }
        fd_istream(file_descriptor fd);
        fd_istream(fd_istream&& old) noexcept;
        virtual ~fd_istream();
        fd_istream& operator=(fd_istream&& old) noexcept;
        fd_istream(const fd_istream& old) = delete;
        fd_istream& operator=(const fd_istream& old) = delete;
    protected:
        fd_istreambuf* buf;
    };
}