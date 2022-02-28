#pragma once
#include "platform.hpp"
#include <string>

namespace amethyst
{
#if defined(WINDOWS)
    using file_handle_type = HANDLE;
#else
    using file_handle_type = int;
#endif

    // 0 = stdin, 1 = stdout, 2 = stderr
    file_handle_type get_std_handle(int unix_number);

    file_handle_type dup(file_handle_type fd);
    // Works like the POSIX dup2.  On windows only 0,1,2 are supported numbers.
    file_handle_type dup2(file_handle_type fd, int target);

    std::string handle_to_string(file_handle_type fd);

    // A wrapper for a file descriptor.
    // It is not considered safe to just open a file and throw the descriptor in one of these instances,
    // because they are not auto-closed.  Use auto_descriptor for that instead.
    class file_descriptor
    {
    public:
        static const file_handle_type null_descriptor;

        file_descriptor() : file_descriptor(null_descriptor) { }
        explicit file_descriptor(file_handle_type handle);
        virtual ~file_descriptor();
        file_descriptor(const file_descriptor& old);
        file_descriptor(file_descriptor&& old) noexcept;
        file_descriptor& operator=(const file_descriptor& old);
        file_descriptor& operator=(file_descriptor&& old) noexcept;

        virtual void swap(file_descriptor&& old);
        virtual void reset(file_handle_type handle);

        typedef const file_handle_type file_descriptor::* safe_bool;
        operator safe_bool() const;
        bool operator!() const;

        file_handle_type get() const;
        file_handle_type release();
        void close();
        void sync();

        // Uninterrupted versions of ::read and ::write
        ssize_t read(char* data, size_t size);
        ssize_t write(const char* data, size_t size);

        virtual std::string inspect() const;

    protected:
        // Duplicate the existing file handle.
        file_handle_type dup_descriptor() const;

        // Used when a copy of a file_descriptor is being made.
        // For the base class, this will return the original descriptor.
        virtual file_handle_type copy_descriptor() const;
    private:
        mutable file_handle_type m_fd = null_descriptor;
    };

    std::string inspect(const file_descriptor& fd);
}