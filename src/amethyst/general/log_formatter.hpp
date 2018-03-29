#pragma once

#include <string>
#include <vector>
#include <memory>

namespace amethyst
{

    struct UnformattedMessage
    {
        UnformattedMessage(const std::string& message,
                           const char* filename,
                           int line_number,
                           const char* function_name,
                           const std::string& backtrace);

        std::string message;
        const char* filename;
        int line_number;
        const char* function_name;
        std::string backtrace;
    };

    // Forward decl for a class that will act as a formatting list.
    class formatter_conversion_list;

    /**
     * The base class for objects that will do log formatting.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     */
    class log_formatter
    {

    private:
        std::unique_ptr<formatter_conversion_list> conversion_list;

    protected:
        virtual std::vector<char> do_format_message(const UnformattedMessage& m);
    public:
        log_formatter();
        virtual ~log_formatter() = default;
        log_formatter(const log_formatter& old);
        log_formatter& operator= (const log_formatter& old);

        std::vector<char> format_message(const UnformattedMessage& m);

        static std::shared_ptr<log_formatter> create_log_formatter(const std::string& format_string);
    };
}