#include "platform.hpp"
#include "fd_stream.hpp"
#include <streambuf>
#include <cstdio>
#include <ios>

namespace amethyst
{
    /**
     * A private class for handling the streambuf operations.
     */
    class fd_ostreambuf : public std::streambuf
    {
    public:
        fd_ostreambuf(file_descriptor fd)
            : m_fd(fd)
        {
        }
        virtual ~fd_ostreambuf()
        {
        }

        virtual std::streambuf::int_type overflow(std::streambuf::int_type c)
        {
            if (c != EOF)
            {
                char c2 = char(c);
                return (int)m_fd.write(&c2, 1);
            }
            return EOF;
        }
        std::streamsize xsputn(std::streambuf::char_type* s, std::streamsize n)
        {
            return std::streamsize(m_fd.write(s, n));
        }

    private:
        file_descriptor m_fd;
    };

    /*
    * Just like the ostreambuf, this wraps a file handle.  It is (initially) being kept separate so the various pointers don't have to be maintained.
    */
    class fd_istreambuf : public std::streambuf
    {
    public:
        fd_istreambuf(file_descriptor fd)
            : m_fd(fd)
        {
        }

        virtual ~fd_istreambuf()
        {
        }

        virtual std::streamsize xsgetn(char* s, std::streamsize n) override
        {
            ssize_t count = m_fd.read(s, n);
            if (count == 0 && n > 0)
            {
                return std::streambuf::traits_type::eof();
            }
            return std::streamsize(count);
        }

    private:
        file_descriptor m_fd;
    };

    fd_ostream::fd_ostream(file_descriptor descriptor)
        : std::ostream(nullptr)
        , buf(new fd_ostreambuf(descriptor))
    {
        std::ostream::rdbuf(buf);
    }

    fd_ostream::fd_ostream(fd_ostream&& old) noexcept
        : std::ostream(std::move(old))
        , buf(nullptr)
    {
        std::swap(buf, old.buf);
    }

    fd_ostream::~fd_ostream()
    {
        delete buf;
        buf = nullptr;
    }

    fd_ostream& fd_ostream::operator=(fd_ostream&& old) noexcept
    {
        std::swap(old.buf, buf);
        std::ostream::operator=(std::move(old));
        return *this;
    }

    fd_istream::fd_istream(file_descriptor fd)
        : std::istream(nullptr)
        , buf(new fd_istreambuf(fd))
    {
        std::istream::rdbuf(buf);
    }

    fd_istream::fd_istream(fd_istream&& old) noexcept
        : std::istream(std::move(old))
        , buf(nullptr)
    {
        std::swap(buf, old.buf);
    }

    fd_istream::~fd_istream()
    {
        delete buf;
        buf = nullptr;
    }

    fd_istream& fd_istream::operator=(fd_istream&& old) noexcept
    {
        std::swap(old.buf, buf);
        std::istream::operator=(std::move(old));
        return *this;
    }
}

