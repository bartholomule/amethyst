#include "platform.hpp"
#include "fd_stream.hpp"
#include <streambuf>
#include <cstdio>
#include <ios>

namespace amethyst
{
    file_handle_type get_std_handle(int unix_number)
    {
#ifdef WINDOWS
        switch (unix_number)
        {
        case 0:
            return GetStdHandle(STD_INPUT_HANDLE);
        case 1:
            return GetStdHandle(STD_OUTPUT_HANDLE);
        case 2:
            return GetStdHandle(STD_ERROR_HANDLE);
        }
        return nullptr;
#else
        return unix_number;
#endif
    }

    /**
     *
     * A private class for handling the streambuf operations.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.2 $
     *
     */
    class fd_ostreambuf : public std::streambuf
    {
        friend class fd_ostream;
    public:
        fd_ostreambuf(file_handle_type file_descriptor)
            : m_fd(file_descriptor)
        {
        }

        virtual ~fd_ostreambuf()
        {
        }

        int do_write(const char* data, size_t size)
        {
#if defined(WINDOWS)
            DWORD count_written = 0;
            if (::WriteFile(m_fd, data, DWORD(size), &count_written, nullptr)) {
                return int(count_written);
            }
            return 0;
#else
            return ::write(m_fd, data, size);
#endif
        }

        virtual std::streambuf::int_type overflow(std::streambuf::int_type c)
        {
            if (c != EOF)
            {
                char c2 = char(c);
                return do_write(&c2, 1);
            }
            return EOF;
        }
        std::streamsize xsputn (std::streambuf::char_type* s, std::streamsize n)
        {
            return do_write(s, n);
        }

    private:
        file_handle_type m_fd;
    };

    fd_ostream::fd_ostream(file_handle_type file_descriptor)
        : std::ostream(0)
        , buf(new fd_ostreambuf(file_descriptor))
    {
        std::ostream::rdbuf(buf);
    }

    fd_ostream::fd_ostream(fd_ostream&& old)
        : std::ostream(std::move(old))
        , buf(old.buf)
    {
        old.buf = nullptr;
    }

    fd_ostream::~fd_ostream()
    {
        delete buf;
        buf = nullptr;
    }

    fd_ostream& fd_ostream::operator=(fd_ostream&& old)
    {
        std::swap(old.buf, buf);
        std::ostream::operator=(std::move(old));
        return *this;
    }
}

