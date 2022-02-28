#pragma once

#include "file_descriptor.hpp"

namespace amethyst
{
    class auto_descriptor : public file_descriptor
    {
    public:
        using file_descriptor::null_descriptor;

        auto_descriptor() = default;
        // Duplicates the descriptor
        auto_descriptor(const auto_descriptor& old);
        auto_descriptor& operator=(const auto_descriptor& old);
        // Takes ownership
        auto_descriptor(auto_descriptor&& old) noexcept;
        explicit auto_descriptor(const file_descriptor& fd);
        explicit auto_descriptor(file_handle_type handle);
        explicit auto_descriptor(file_descriptor&& fd);
        auto_descriptor& operator=(auto_descriptor&& old) noexcept;

        // Auto-closes
        virtual ~auto_descriptor();

        auto_descriptor dup() const;

        void reset(file_handle_type fd) override;

        std::string inspect() const override;
    protected:
        // Copying an auto_descriptor will duplciate the file handle so both copies
        // are still perfectly functional with read/write/close working the same on both.
        file_handle_type copy_descriptor() const override;

    private:
        file_handle_type m_original_descriptor = null_descriptor;
    };
}