/*
 * $Id: base_logger.cpp,v 1.4 2008/06/16 10:17:47 kpharris Exp $
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

#include "base_logger.hpp"

namespace amethyst
{
	//------------------------------------------
	// Default constructor for class logger
	//------------------------------------------
	logger::logger() :
		formatter(log_formatter::create_log_formatter("%m"))
		, output_stream(stream_stdout)
#if defined(HAVE_CCPP_THREADS)
		, output_lock("default logger")
#endif // HAVE_CCPP_THREADS
	{
	} // logger()

	logger::logger(const std::shared_ptr<log_formatter>& lf, const output_stream_ref& sr) :
		formatter(lf)
		, output_stream(sr)
#if defined(HAVE_CCPP_THREADS)
		, output_lock("formatted logger")
#endif // HAVE_CCPP_THREADS
	{
	}

	//---------------------------------
	// Destructor for class logger
	//---------------------------------
	logger::~logger()
	{
	} // ~logger()

	//---------------------------------------
	// Copy constructor for class logger
	//---------------------------------------
	logger::logger(const logger& old) :
		formatter(old.formatter),
		output_stream(old.output_stream)
#if defined(HAVE_CCPP_THREADS)
		, output_lock()
#endif // HAVE_CCPP_THREADS
	{
	} // logger(logger)

	//------------------------------------------
	// Assignment operator for class logger
	//------------------------------------------
	logger& logger::operator= (const logger& old)
	{
		// Generic check for self-assignment
		if( &old != this)
		{
			formatter = old.formatter;
			output_stream = old.output_stream;
#if defined(HAVE_CCPP_THREADS)
			output_lock = Mutex();
#endif // HAVE_CCPP_THREADS
		}
		return (*this);
	} // logger::operator=(logger)

	logger::log_levels logger::get_log_level() const
	{
		return my_level;
	}

	void logger::set_log_level(logger::log_levels level)
	{
		my_level = level;
	}

	bool logger::level_enabled(logger::log_levels level)
	{
		return level <= my_level;
	}

	void logger::log(const std::string& message)
	{
		return log(my_level, message);
	}

	void logger::log(logger::log_levels level, const std::string& message)
	{
		if( level_enabled(level) )
		{
			log(level, UnformattedMessage(message, NULL, -1, NULL, std::string()));
		}
	}

	void logger::log(log_levels level, const std::string& message,
		const char* filename,
		int line_number,
		const char* function_name)
	{
		if( level_enabled(level) )
		{
			log(level, UnformattedMessage(message, filename, line_number, function_name, std::string()));
		}
	}

	void logger::log(log_levels level, const UnformattedMessage& message)
	{
		if( level_enabled(level) )
		{
#if defined(HAVE_CCPP_THREADS)
			MutexLock ml(output_lock);
#endif // HAVE_CCPP_THREADS
			std::vector<char> formatted_text = formatter->format_message(message);
			output_stream->write(&formatted_text[0], formatted_text.size())  << std::endl;
		}
	}

} // namespace amethyst

