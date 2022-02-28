#include "file_descriptor.hpp"
#include <sstream>

#if defined(POSIX)
#include <sys/types.h>
#include <sys/stat.h>
#endif

namespace amethyst
{
    namespace
    {
#if defined(WINDOWS)
        DWORD get_handle_number(int unix_number)
        {
            switch (unix_number)
            {
            case 0:
                return STD_INPUT_HANDLE;
            case 1:
                return STD_OUTPUT_HANDLE;
            case 2:
                return STD_ERROR_HANDLE;
            }
            return DWORD(-1);
        }
#endif
    }

    file_handle_type get_std_handle(int unix_number)
    {
#ifdef WINDOWS
        return ::GetStdHandle(get_handle_number(unix_number));
#else
        return unix_number;
#endif
    }

    std::string handle_to_string(file_handle_type fd)
    {
        if (fd == file_descriptor::null_descriptor)
        {
            return "null_descriptor";
        }
        std::ostringstream oss;
        oss << fd;
        return oss.str();
    }

    file_handle_type dup(file_handle_type fd)
    {
        file_handle_type res = file_descriptor::null_descriptor;
#if defined(POSIX)
        res = ::dup(fd);
#else
        HANDLE hdup = file_descriptor::null_descriptor;
        if (::DuplicateHandle(::GetCurrentProcess(), res, ::GetCurrentProcess(), &hdup, 0, FALSE, DUPLICATE_SAME_ACCESS))
        {
            res = hdup;
        }
#endif
        return res;
    }

    file_handle_type dup2(file_handle_type fd, int target)
    {
#if defined(POSIX)
        return ::dup2(fd, target);
#else
        DWORD stdhandle = get_handle_number(target);
        HANDLE duped = dup(fd);
        if (SetStdHandle(stdhandle, duped))
        {
            return duped;
        }
        CloseHandle(duped);
        return file_descriptor::null_descriptor;
#endif
    }

#if defined(WINDOWS)
    const file_handle_type file_descriptor::null_descriptor = INVALID_HANDLE_VALUE;
#else
    const file_handle_type file_descriptor::null_descriptor = -1;
#endif

    file_descriptor::file_descriptor(file_handle_type handle)
        : m_fd(handle)
    {
    }

    file_descriptor::~file_descriptor()
    {
    }

    file_descriptor::file_descriptor(const file_descriptor& old)
        : m_fd(old.copy_descriptor())
    {
    }

    file_descriptor::file_descriptor(file_descriptor&& old) noexcept
        : m_fd(null_descriptor)
    {
        this->swap(std::move(old));
    }

    file_descriptor& file_descriptor::operator=(const file_descriptor& old)
    {
        if (&old != this)
        {
            this->reset(old.copy_descriptor());
        }
        return *this;
    }

    file_descriptor& file_descriptor::operator=(file_descriptor&& old) noexcept
    {
        if (&old != this)
        {
            this->reset(null_descriptor);
            this->swap(std::move(old));
        }
        return *this;
    }

    void file_descriptor::swap(file_descriptor&& old)
    {
        std::swap(m_fd, old.m_fd);
    }

    void file_descriptor::reset(file_handle_type handle)
    {
        m_fd = handle;
    }

    file_descriptor::operator safe_bool() const
    {
        if (m_fd != null_descriptor)
        {
            return &file_descriptor::m_fd;
        }
        return nullptr;
    }

    bool file_descriptor::operator!() const
    {
        return m_fd == null_descriptor;
    }

    file_handle_type file_descriptor::get() const
    {
        return m_fd;
    }

    file_handle_type file_descriptor::release()
    {
        file_handle_type result = null_descriptor;
        std::swap(result, m_fd);
        return result;
    }

    void file_descriptor::close()
    {
        if (m_fd != null_descriptor)
        {
#if defined(WINDOWS)
            ::CloseHandle(m_fd);
#else
            ::close(m_fd);
#endif
            m_fd = null_descriptor;
        }
    }

    void file_descriptor::sync()
    {
        if (m_fd != null_descriptor)
        {
#if defined(POSIX)
            struct stat st;
            fstat(m_fd, &st);
            if (S_ISREG(st.st_mode))
            {
                ::fsync(m_fd);
            }
#else
            ::FlushFileBuffers(m_fd);
#endif
        }
    }

    namespace
    {
        const int OPERATION_ERROR = -1;
        const int ERROR_NO_ERROR = 0;

        template <typename F, typename datatype>
        ssize_t do_unninterupted_operation(F f, file_handle_type fd, datatype data, size_t size)
        {
            ssize_t result = OPERATION_ERROR;
            while (result == OPERATION_ERROR)
            {
                errno = ERROR_NO_ERROR;
                result = f(fd, data, size);
                if (result == OPERATION_ERROR && errno != EINTR)
                {
                    break;
                }
            }
            return result;
        }

        int read_wrapper(file_handle_type fd, char* dest, size_t size)
        {
#if defined(POSIX)
            return ::read(fd, dest, size);
#else
            DWORD count_read = 0;
            if (::ReadFile(fd, dest, DWORD(size), &count_read, nullptr))
            {
                return int(count_read);
            }
            // DWORD code = ::GetLastError();
            // TODO: Properly handle ERROR_IO_PENDING

            // Just set this to something for now.
            errno = EIO;

            return OPERATION_ERROR;
#endif
        }

        int write_wrapper(file_handle_type fd, const char* source, size_t size)
        {
#if defined(POSIX)
            return ::write(fd, source, size);
#else
            DWORD count_written = 0;
            if (::WriteFile(fd, source, DWORD(size), &count_written, nullptr)) {
                return int(count_written);
            }
            // TODO: properly handle errors on windows.
            errno = EIO;
            return OPERATION_ERROR;
#endif
        }
    }

    ssize_t file_descriptor::read(char* data, size_t size)
    {
        if (m_fd != null_descriptor)
        {
            return (ssize_t)do_unninterupted_operation(&read_wrapper, m_fd, data, size);
        }
        return ssize_t(OPERATION_ERROR);
    }

    ssize_t file_descriptor::write(const char* data, size_t size)
    {
        if (m_fd != null_descriptor)
        {
            return (ssize_t)do_unninterupted_operation(&write_wrapper, m_fd, data, size);
        }
        return ssize_t(OPERATION_ERROR);
    }

    file_handle_type file_descriptor::dup_descriptor() const
    {
        if (m_fd != null_descriptor)
        {
            return dup(m_fd);
        }
        return null_descriptor;
    }

    file_handle_type file_descriptor::copy_descriptor() const
    {
        return m_fd;
    }

    std::string file_descriptor::inspect() const
    {
        if (m_fd == null_descriptor)
        {
            return "file_descriptor(null_descriptor)";
        }
        std::ostringstream oss;
        oss << "file_descriptor(" << m_fd << ")";
        return oss.str();
    }

    std::string inspect(const file_descriptor& fd)
    {
        return fd.inspect();
    }
}