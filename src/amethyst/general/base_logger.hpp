/*
 * $Id: base_logger.hpp,v 1.1 2006/01/30 11:24:53 kpharris Exp $
 *
 * Part of "Amethyst" -- A playground for graphics development.
 * Copyright (C) 2006 Kevin Harris
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#if       !defined(AMETHYST__BASE_LOGGER_HPP)
#define            AMETHYST__BASE_LOGGER_HPP

#include <memory>
#include <string>
#include "log_formatter.hpp"
#include "stream_reference.hpp"

#if defined(HAVE_CCPP_THREADS)
#include <cc++/thread.h>
#endif // HAVE_CCPP_THREADS

namespace amethyst
{
    class logger;
    typedef std::shared_ptr<logger> logger_ref;

    /**
     *
     * The base logger.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.1 $
     *
     */
    class logger
    {
    public:
        enum log_levels
        {
            LOG_LEVEL_NONE,
            LOG_LEVEL_FATAL_ERROR,
            LOG_LEVEL_ERROR,
            LOG_LEVEL_WARNING,
            LOG_LEVEL_INFO,
            LOG_LEVEL_DEBUG,
            LOG_LEVEL_EXCESSIVE_DEBUG,
            LOG_LEVEL_EVERYTHING
        };

    private:
        log_levels my_level;
    protected:
        std::shared_ptr<log_formatter> formatter;
        output_stream_ref output_stream;

#if defined(HAVE_CCPP_THREADS)
        Mutex output_lock;
#endif // HAVE_CCPP_THREADS

    public:
        /** Default constructor */
        logger();

        logger(const std::shared_ptr<log_formatter>& lf, const output_stream_ref& sr);

        /** Destructor */
        virtual ~logger();

        /** Copy constructor */
        logger(const logger& old);

        /** Assignment operator */
        logger& operator= (const logger& old);

        /** Get the current log level of this logger */
        log_levels get_log_level() const;

        /** Set the current log level of this logger */
        void set_log_level(log_levels level);

        /**
         * Return if the given level would be considered "enabled".  That is, if
         * a message would be logged when supplied with the given level.
         */
        bool level_enabled(log_levels level);

        /** Log a simple message at the current log level. */
        void log(const std::string& message);

        /** Log a simple message */
        void log(log_levels level, const std::string& message);


        void log(log_levels level, const std::string& message,
                 const char* filename,
                 int line_number,
                 const char* function_name);

        void log(log_levels level, const UnformattedMessage& message);

    }; // class logger
} // namespace amethyst


#endif /* !defined(AMETHYST__BASE_LOGGER_HPP) */

