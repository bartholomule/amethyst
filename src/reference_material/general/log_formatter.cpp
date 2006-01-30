/*
 * $Id: log_formatter.cpp,v 1.1 2006/01/30 11:24:53 kpharris Exp $
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

#include "log_formatter.hpp"
#include <vector>

#include <sys/types.h>
#include <unistd.h>

#include "string_format.hpp"

namespace amethyst
{

	UnformattedMessage::UnformattedMessage(const std::string& _message,
		const char* _filename,
		int _line_number,
		const char* _function_name,
		const std::string& _backtrace) :
		message(_message),
		filename(_filename),
		line_number(_line_number),
		function_name(_function_name),
		backtrace(_backtrace)
	{
	}

	namespace conversion
	{
		enum format_entry_type
			{
				FORMATTER_STRING,
				FORMATTER_MESSAGE,
				FORMATTER_FILENAME,
				FORMATTER_FILELINE,
				FORMATTER_FUNCTION,
				FORMATTER_BACKTRACE,
				// These aren't done yet.
				FORMATTER_THREADID,
				FORMATTER_PID,
				FORMATTER_RUNTIME,
				// These will need to be tweaked at some point.  The important ones
				// (at least initially) are listed above.  We can stick useful
				// strftime formatting junk in the data field...
				FORMATTER_DATE,
				FORMATTER_TIME
			};
		struct formatter_conversion_entry
		{
			format_entry_type type;
			std::string data; // Only used if the type is FORMATTER_STRING
		};
	}

	class formatter_conversion_list
	{
	public:
		typedef std::vector<conversion::formatter_conversion_entry> data_type;
		typedef data_type::const_iterator const_iterator;
		data_type data;
		const_iterator begin() const { return data.begin(); }
		const_iterator end() const { return data.end(); }
	};

	//--------------------------------------------
	// Default constructor for class log_formatter
	//--------------------------------------------
	log_formatter::log_formatter() :
		conversion_list(new formatter_conversion_list)
	{

	} // log_formatter()

	//-----------------------------------
	// Destructor for class log_formatter
	//-----------------------------------
	log_formatter::~log_formatter()
	{
		delete conversion_list;
	} // ~log_formatter()

	//-----------------------------------------
	// Copy constructor for class log_formatter
	//-----------------------------------------
	log_formatter::log_formatter(const log_formatter& old) :
		conversion_list(new formatter_conversion_list(*old.conversion_list))
	{

	} // log_formatter(log_formatter)

	//--------------------------------------------
	// Assignment operator for class log_formatter
	//--------------------------------------------
	log_formatter& log_formatter::operator= (const log_formatter& old)
	{
		// Generic check for self-assignment
		if( &old != this)
		{
			delete conversion_list;
			conversion_list = new formatter_conversion_list(*old.conversion_list);
		}
		return (*this);
	} // log_formatter::operator=(log_formatter)

	std::string log_formatter::format_message(const UnformattedMessage& m)
	{
		// We will probably want more junk in here...
		return do_format_message(m);
	}

	std::string log_formatter::do_format_message(const UnformattedMessage& m)
	{
		std::string result;
		for( formatter_conversion_list::const_iterator iter = conversion_list->begin();
			  iter != conversion_list->end();
			  ++iter )
		{
			switch(iter->type)
			{
			case conversion::FORMATTER_STRING:
				result += iter->data;
				break;
			case conversion::FORMATTER_FILENAME:
				result += m.filename;
				break;
			case conversion::FORMATTER_FILELINE:
				result += string_format("%1", m.line_number);
				break;
			case conversion::FORMATTER_FUNCTION:
				result += m.function_name;
				break;
			case conversion::FORMATTER_BACKTRACE:
				result += m.backtrace;
				break;
			case conversion::FORMATTER_PID:
				result += string_format("%1", ::getpid());
				break;
			case conversion::FORMATTER_THREADID:
			case conversion::FORMATTER_RUNTIME:
				result += "<NOT IMPLEMENTED>";
				break;
			case conversion::FORMATTER_DATE:
			case conversion::FORMATTER_TIME:
				result += "<DATE OR TIME>";
				break;
			}
		}
		return result;
	}

} // namespace amethyst

