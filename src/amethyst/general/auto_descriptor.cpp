#include "auto_descriptor.hpp"
#include <sstream>

namespace amethyst
{
    auto_descriptor::auto_descriptor(file_handle_type fd)
        : file_descriptor(fd)
        , m_original_descriptor(fd)
    {
    }

    auto_descriptor::auto_descriptor(const file_descriptor& old)
        : file_descriptor(old)
        , m_original_descriptor(get())
    {
    }

    auto_descriptor::auto_descriptor(file_descriptor&& fd)
        : file_descriptor(std::move(fd))
        , m_original_descriptor(get())
    {
    }

    auto_descriptor::auto_descriptor(const auto_descriptor& old)
        : file_descriptor(old.dup_descriptor())
        , m_original_descriptor(old.m_original_descriptor)
    {
    }
    auto_descriptor::auto_descriptor(auto_descriptor&& old) noexcept
        : file_descriptor(std::move(old))
        , m_original_descriptor(get())
    {
    }

    auto_descriptor::~auto_descriptor()
    {
        this->close();
    }

    auto_descriptor& auto_descriptor::operator=(auto_descriptor&& old) noexcept
    {
        if (&old != this)
        {
            m_original_descriptor = old.m_original_descriptor;
            this->swap(std::move(old));
            old.close();
        }
        return *this;
    }

    auto_descriptor& auto_descriptor::operator=(const auto_descriptor& old)
    {
        if (&old != this)
        {
            this->reset(dup_descriptor());
            m_original_descriptor = old.get();
        }
        return *this;
    }

    auto_descriptor auto_descriptor::dup() const
    {
        auto_descriptor res(dup_descriptor());
        res.m_original_descriptor = m_original_descriptor;
        return res;
    }

    void auto_descriptor::reset(file_handle_type fd)
    {
        this->close();
        m_original_descriptor = fd;
        file_descriptor::reset(fd);
    }

    file_handle_type auto_descriptor::copy_descriptor() const
    {
        return dup_descriptor();
    }

    std::string auto_descriptor::inspect() const
    {
        file_handle_type current = get();

        // TODO: Replace this with a format, once a performant format is in place.
        std::ostringstream oss;
        oss << "auto_descriptor(";
        if (current != null_descriptor)
        {
            oss << current;
        }
        else
        {
            oss << "null_descriptor";
        }

        if (current != m_original_descriptor && m_original_descriptor != null_descriptor)
        {
            oss << " [original=" << m_original_descriptor << "]";
        }
        oss << ")";

        return oss.str();
    }
}