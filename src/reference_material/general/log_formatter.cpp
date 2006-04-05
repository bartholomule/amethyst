/*
 * $Id: log_formatter.cpp,v 1.2 2006/04/05 05:59:35 kpharris Exp $
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
#include <time.h>

#include <iostream>

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

	std::ostream& operator<<(std::ostream&o, const UnformattedMessage& m)
	{
		o << string_format( "{ message=\"%1\" filename=\"%2\" line_number=%3 function_name=\"%4\" backtrace=\"%5\" }",
			m.message,
			m.filename,
			m.line_number,
			m.function_name,
			m.backtrace )
		  << std::endl;
		return o;
	}

	namespace conversion
	{
		enum format_entry_type
			{
				FORMATTER_STRING,
				FORMATTER_MESSAGE,  // %m
				FORMATTER_FILENAME, // %F
				FORMATTER_FILELINE, // %L
				FORMATTER_FUNCTION, // %M
				FORMATTER_BACKTRACE,
				// These aren't done yet.
				FORMATTER_THREADID, // %t
				FORMATTER_PID,      // %P
				FORMATTER_RUNTIME,  // runtime in ms.
				// These will need to be tweaked at some point.  The important ones
				// (at least initially) are listed above.  We can stick useful
				// strftime formatting junk in the data field...
				FORMATTER_DATETIME
			};
		enum format_justification
			{
				FORMAT_JUSTIFY_LEFT,
				FORMAT_JUSTIFY_RIGHT,
				FORMAT_JUSTIFY_CENTER // This is not standard for any form of format specifier.  I'm adding it because it could be useful.
			};
		struct formatter_conversion_entry
		{
			format_entry_type type;
			std::vector<char> data; // Only used if the type is FORMATTER_STRING
			int field_width_minimum;
			int field_width_maximum;
			format_justification justification;

			formatter_conversion_entry():
				field_width_minimum(0)
				, field_width_maximum(0)
				, justification(FORMAT_JUSTIFY_RIGHT)
			{
			}
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

	std::vector<char> log_formatter::format_message(const UnformattedMessage& m)
	{
		// We will probably want more junk in here...
		return do_format_message(m);
	}

	namespace // anonymous
	{
		void append_text(std::vector<char>& vec, const char* text, int size = -1)
		{
			if( size < 0 )
			{
				vec.insert(vec.end(), text, text + strlen(text));
			}
			else
			{
				vec.insert(vec.end(), text, text + size);
			}
		}
		void append_text(std::vector<char>& vec, const std::string& str)
		{
			append_text(vec, str.c_str(), str.length());
		}
	} // end anonymous namespace

	std::vector<char> log_formatter::do_format_message(const UnformattedMessage& m)
	{
		std::vector<char> result;
		if( !conversion_list )
		{
			std::cerr << "Have NULL conversion list." << std::endl;
			return result;
		}
		for( formatter_conversion_list::const_iterator iter = conversion_list->begin();
			iter != conversion_list->end();
			++iter )
		{
			std::vector<char> formatted_entry;
			switch(iter->type)
			{
			case conversion::FORMATTER_STRING:
				formatted_entry.insert(formatted_entry.end(), iter->data.begin(), iter->data.end());
				break;
			case conversion::FORMATTER_MESSAGE:
				formatted_entry.insert(formatted_entry.end(), m.message.begin(), m.message.end());
				break;
			case conversion::FORMATTER_FILENAME:
				if( m.filename )
				{
					formatted_entry.insert(formatted_entry.end(), m.filename, m.filename + strlen(m.filename));
				}
				else
				{
					append_text(formatted_entry, "<NULL>");
				}
				break;
			case conversion::FORMATTER_FILELINE:
				append_text(formatted_entry, string_format("%1", m.line_number));
				break;
			case conversion::FORMATTER_FUNCTION:
				if( m.function_name )
				{
					append_text(formatted_entry, m.function_name);
				}
				else
				{
					append_text(formatted_entry, "<NULL>");
				}
				break;
			case conversion::FORMATTER_BACKTRACE:
				append_text(formatted_entry, m.backtrace);
				break;
			case conversion::FORMATTER_PID:
				append_text(formatted_entry, string_format("%1", ::getpid()));
				break;
			case conversion::FORMATTER_THREADID:
			case conversion::FORMATTER_RUNTIME:
				append_text(formatted_entry, "<NOT IMPLEMENTED>");
				break;
			case conversion::FORMATTER_DATETIME:
				{
					time_t t = time(NULL);
					struct tm tmstruct;
					struct tm* foo = gmtime_r(&t, &tmstruct);

					std::vector<char> format_buffer;
					if( !iter->data.empty() )
					{
						format_buffer = iter->data;
						format_buffer.push_back('\0');
					}
					else
					{
						const char default_format[] = "%Y-%m-%dT%H:%M:%SZ\0";
						format_buffer.insert(format_buffer.end(), default_format, default_format + strlen(default_format) + 1);
					}
					std::vector<char> time_buffer(1024);
					size_t count = strftime(&time_buffer[0], time_buffer.size(), &format_buffer[0], &tmstruct);
					formatted_entry.insert(formatted_entry.end(), time_buffer.begin(), time_buffer.begin() + count);
				}
				break;
			default:
				std::cerr << string_format("Hit unknown conversion type (%1)", int(iter->type)) << std::endl;
				break;
			}

			// Adjust the output for any field width specifiers.
			int min_difference = iter->field_width_minimum - int(formatted_entry.size());
			if( min_difference > 0 )
			{
				std::vector<char> foo(min_difference, ' ');
				if( iter->justification == conversion::FORMAT_JUSTIFY_RIGHT )
				{
					// Insert items to the left...
					formatted_entry.insert(formatted_entry.begin(), foo.begin(), foo.end());
				}
				else if( iter->justification == conversion::FORMAT_JUSTIFY_CENTER )
				{
					// Insert half on each side...
					formatted_entry.insert(formatted_entry.begin(), foo.begin(), foo.begin() + min_difference / 2);
					formatted_entry.insert(formatted_entry.end(), foo.begin(), foo.begin() + min_difference - (min_difference / 2));
				}
				else
				{
					// Append items to the right...
					formatted_entry.insert(formatted_entry.end(), foo.begin(), foo.end());
				}
			}
			int max_difference = int(formatted_entry.size()) - iter->field_width_maximum;
			if( (iter->field_width_maximum > 0)  && (max_difference > 0) )
			{
				// erase anything that's too large.
				if( iter->justification == conversion::FORMAT_JUSTIFY_RIGHT )
				{
					// Delete items from the left...
					formatted_entry.erase(formatted_entry.begin(), formatted_entry.begin() + max_difference);
				}
				else if( iter->justification == conversion::FORMAT_JUSTIFY_CENTER )
				{
					// Delete half on each side...
					formatted_entry.erase(formatted_entry.begin() + formatted_entry.size() - max_difference / 2, formatted_entry.end());
					formatted_entry.erase(formatted_entry.begin(), formatted_entry.begin() + max_difference / 2);
				}
				else
				{
					// delete items from the right...
					formatted_entry.erase(formatted_entry.begin() + formatted_entry.size() - max_difference, formatted_entry.end());
				}
			}
			result.insert(result.end(), formatted_entry.begin(), formatted_entry.end());
		}
		return result;
	}

	namespace // anonymous
	{

		/*
		  # Available conversion specifiers:
# %c - The component (e.g. owcimomd)
# %d - The date. May be followed by a date format specifier enclosed between
#      braces. For example, %d{%H:%M:%S} or %d{%d %b %Y %H:%M:%S}. If no date
#      format specifier is given then ISO8601 format is assumed.
#      For more information of the date format specifiers, lookup the
#      documentation for the strftime() function found in the <ctime> header.
#      The only addition is %Q, which is the number of milliseconds.
# %F - The file name
# %l - The filename and line number. e.g. file.cpp(100)
# %L - The line number
# %M - The method name where the logging request was issued (only works on
#      C++ compilers which support __PRETTY_FUNCTION__ or C99's __func__)
# %m - The message
# %e - The message as XML CDATA. This includes the "<![CDATA[" and ending "]]>"
# %n - The platform dependent line separator character (\n) or characters (\r\n)
# %p - category, aka level, aka priority
# %r - The number of milliseconds elapsed since the start of the application
#      until the creation of the logging event
# %t - Thread id
# %P - Process id
# %% - %
# \n - newline
# \t - tab
# \r - linefeed
# \\ - \
# \x<hexDigits> - The character represented in hexadecimal.
#
#   It is possible to change the minimum field width, the maximum field width
# and justification. The optional format modifier is placed between the
# percent sign and the conversion character.
#   The first optional format modifier is the left justification flag which is
# the minus (-) character. The optional minimum field width modifier follows.
# It is an integer that represents the minimum number of characters to
# output. If the data item requires fewer characters, it is padded with
# spaces on either the left or the right, according to the justification
# flag. If the data item is larger than the minimum field width, the field
# is expanded to accommodate the data.
#   The maximum field width modifier is designated by a period followed by a
# decimal constant. If the data item is longer than the maximum field, then
# the extra characters are removed from the beginning of the data item
# (by default), or from the end if the left justification flag was specified.
#
# Examples:
#   Log4j TTCC layout:
#   "%r [%t] %-5p %c - %m"
#
#   Similar to TTCC, but with some fixed size fields:
#   "%-6r [%15.15t] %-5p %30.30c - %m"

		*/
		conversion::formatter_conversion_entry parse_percent_region(std::string::const_iterator& p, std::string::const_iterator end)
		{
			conversion::formatter_conversion_entry retval;
			retval.justification = conversion::FORMAT_JUSTIFY_RIGHT;
			retval.field_width_minimum = 0;
			retval.field_width_maximum = 0;
			++p;
			// Process the sign and field width.
			if( p != end )
			{
				if( *p  == '-' )
				{
					retval.justification = conversion::FORMAT_JUSTIFY_LEFT;
					++p;
				}
				else if( *p == '+' )
				{
					retval.justification = conversion::FORMAT_JUSTIFY_RIGHT;
					++p;
				}
				else if( *p == '/' )
				{
					retval.justification = conversion::FORMAT_JUSTIFY_CENTER;
					++p;
				}
				for(; (p != end) && isdigit(*p); ++p )
				{
					retval.field_width_minimum = (retval.field_width_minimum * 10) + (*p - '0');
				}
				if( (p != end) && (*p == '.') )
				{
					++p;
					for(; (p != end) && isdigit(*p); ++p )
					{
						retval.field_width_maximum = (retval.field_width_maximum * 10) + (*p - '0');
					}
				}
			}
			// Process the rest of the % option...
			if( p != end )
			{
				switch(*p)
				{
				case 'm':
					retval.type = conversion::FORMATTER_MESSAGE;
					break;
				case 'F':
					retval.type = conversion::FORMATTER_FILENAME;
					break;
				case 'L':
					retval.type = conversion::FORMATTER_FILELINE;
					break;
				case 'M':
					retval.type = conversion::FORMATTER_FUNCTION;
					break;
				case 't':
					retval.type = conversion::FORMATTER_THREADID;
					break;
				case 'd':
					{
						std::string::const_iterator i = p + 1;
						std::vector<char> buffer;
						if( *i == '{' )
						{
							for( ++i; (i != end) && (*i != '}'); ++i )
							{
								buffer.push_back(*i);
							}
							if( i != end )
							{
								retval.type = conversion::FORMATTER_DATETIME;
								retval.data = buffer;
								++i;
								p = i;
							}
							else
							{
								// It's actually an error, but for now, we'll just copy the string across as text.
								retval.type = conversion::FORMATTER_STRING;
								retval.data.clear();
								retval.data.push_back('%');
								retval.data.insert(retval.data.end(), p, end);
							}
						}
						else
						{
							retval.type = conversion::FORMATTER_DATETIME;
							retval.data.clear();
						}
					}
					break;
				case 'P':
					retval.type = conversion::FORMATTER_PID;
					break;
				case '%':
					retval.type = conversion::FORMATTER_STRING;
					retval.data = std::vector<char>(1, '%');
					break;
				default:
					// format string ERROR.
					retval.type = conversion::FORMATTER_STRING;
					retval.data = std::vector<char>(1, '%');
					retval.data.push_back(*p);
					break;
				}
				++p;
			}
			return retval;
		}

		std::vector<char> parse_backslash_region(std::string::const_iterator& p, std::string::const_iterator end)
		{
			std::vector<char> retvec;
			++p;
			if( p != end )
			{
				switch(*p)
				{
				case '\\':
					++p;
					retvec.push_back('\\');
					break;
				case 'n':
					++p;
					retvec.push_back('\n');
					break;
				case 'r':
					++p;
					retvec.push_back('\r');
					break;
				case 't':
					++p;
					retvec.push_back('\t');
					break;
				case 'x':
					{
						std::string::const_iterator i = p;
						int foo = 0;
						bool have_xdigits = false;
						for(++i; i != end && isxdigit(*i); ++i)
						{
							int current = tolower(*i) - '0';
							if( current < 0 || current > 9 )
							{
								current = (current + '0') - 'a' + 10;
							}
							foo = (foo << 4) + current;
							have_xdigits = true;
						}
						if( have_xdigits )
						{
							retvec.push_back(char(foo));
							p = i;
						}
						else
						{
							// An error in their format string... Just copy the character
							// into the output (although we should signal an error).
							retvec.push_back('\\');
							retvec.push_back(*p);
							++p;
						}
					}
					break;
				default:
					// An error in their format string... Just copy the character
					// into the output (although we should signal an error).
					retvec.push_back('%');
					retvec.push_back(*p);
					++p;
					break;
				}
			}
			return retvec;
		}

		conversion::formatter_conversion_entry create_solid_entry(const std::vector<char>& vec)
		{
			conversion::formatter_conversion_entry last_solid_message;
			last_solid_message.type = conversion::FORMATTER_STRING;
			last_solid_message.data = vec;
			last_solid_message.field_width_minimum = vec.size();
			last_solid_message.field_width_maximum = vec.size();
			return last_solid_message;
		}

		std::vector<conversion::formatter_conversion_entry> parse_format_string(const std::string& format_string)
		{
			std::vector<conversion::formatter_conversion_entry> retval;

			std::vector<char> current_message;
			for( std::string::const_iterator current = format_string.begin(); current != format_string.end(); )
			{
				char c = *current;
				switch(c)
				{
				case '%':
					{
						conversion::formatter_conversion_entry entry = parse_percent_region(current, format_string.end());
						if( entry.type == conversion::FORMATTER_STRING )
						{
							current_message.insert(current_message.end(), entry.data.begin(), entry.data.end());
						}
						else
						{
							// convert the existing message to an entry...
							conversion::formatter_conversion_entry last_solid_message = create_solid_entry(current_message);
							current_message.clear();

							// Append the current text...
							retval.push_back(last_solid_message);
							// Append the next data.
							retval.push_back(entry);
						}
					}
					// No need to increment the iterator.  We're alredy one past the backslashed data.
					break;
				case '\\':
					{
						std::vector<char> backslashed_data = parse_backslash_region(current, format_string.end());
						current_message.insert(current_message.end(), backslashed_data.begin(), backslashed_data.end());
						// No need to increment the iterator.  We're alredy one past the backslashed data.
					}
					break;
				default:
					current_message.push_back(c);
					++current;
					break;
				}
			}
			// If there is some trailing text, add it.
			if( !current_message.empty() )
			{
				retval.push_back(create_solid_entry(current_message));
			}

			return retval;
		}
	} // end anonymous namespace

	rc_pointer<log_formatter> log_formatter::create_log_formatter(const std::string& format_string)
	{
		rc_pointer<log_formatter> formatter(new log_formatter);

		formatter->conversion_list->data = parse_format_string(format_string);
		return formatter;
	}

} // namespace amethyst

