/*
 * $Id: string_format.hpp,v 1.6 2007/05/18 16:36:59 kpharris Exp $
 *
 * Part of "Amethyst" a playground for graphics development
 * Copyright (C) 2004 Kevin Harris
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

#if       !defined(AMETHYST__STRING_FORMAT_HPP)
#define            AMETHYST__STRING_FORMAT_HPP

#include "inspect.hpp"
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdlib>

namespace std
{
	template <typename T>
	std::ostream& operator<<(std::ostream& o, const std::vector<T>& t)
	{
		return o << inspect(t);
	}
}

namespace amethyst
{
	template <class T>
	std::string convert_to_string(const T& arg, int base = 0, int width = 0, int precision = -1);


	template <class T1>
	std::string string_format(const std::string& format, const T1& arg1);

	template <class T1, class T2>
	std::string string_format(const std::string& format, const T1& arg1, const T2& arg2);

	template <class T1, class T2, class T3>
	std::string string_format(const std::string& format, const T1& arg1, const T2& arg2, const T3& arg3);

	template <class T1, class T2, class T3, class T4>
	std::string string_format(const std::string& format, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4);

	template <class T1, class T2, class T3, class T4, class T5>
	std::string string_format(const std::string& format, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5);

	template <class T1, class T2, class T3, class T4, class T5, class T6>
	std::string string_format(const std::string& format, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5, const T6& arg6);


	//---------------------------------------------------------------------------

#define amethyst_string_conversion_manip(ostr, base, width, precision) \
    do \
    { \
       if( (base) != 0 ) \
       { \
          (ostr) << std::setbase(base); \
       } \
       if( (width) != 0 ) \
       { \
          (ostr) << std::setw(width); \
       } \
       if( (precision) != -1 ) \
       { \
          (ostr) << std::setprecision(precision); \
       } \
    } \
    while(0)

	template <class T>
	std::string convert_to_string(const T& arg, int base, int width, int precision)
	{
		std::ostringstream ostr;
		amethyst_string_conversion_manip(ostr, base, width, precision);
		ostr << arg;
		return ostr.str();
	}

	template <class T>
	std::string convert_to_string(const T* arg, int base, int width, int precision)
	{
		std::ostringstream ostr;
		amethyst_string_conversion_manip(ostr, base, width, precision);
		if (arg)
		{
			ostr << arg;
		}
		else
		{
			ostr << "<NULL>";
		}
		return ostr.str();
	}

	constexpr size_t bad_arg = size_t(-1);

	// This will be the function that determines the base, width, precision, and
	// argument offset of the next arg to be printed.  Right now, it will only do
	// the base as hex if an [xX] is present before the argument index.
	template <class size_type>
	size_type extract_next_arg(const std::string& format,
		size_type base_offset, size_type& num_eaten,
		int& base, int& width, int& precision)
	{
		width = 0;
		precision = -1;
		num_eaten = 1; // eat at least the percent.

		const char* format_str = format.c_str();

		const char* begin = format_str + base_offset + 1;

		base = 0;
		if ((*begin == 'x') || (*begin == 'X'))
		{
			base = 16;
			++begin;
			++num_eaten;
		}

		char* end = const_cast<char*>(begin);
		long arg_temp = strtol(begin, &end, 10);
		ptrdiff_t arg_length = end - begin;
		if (arg_length > 0)
		{
			num_eaten += arg_length;
			return arg_temp;
		}
		else
		{
			num_eaten = 0;
			return bad_arg;
		}
	}

	template <class T1>
	std::string string_format(const std::string& format, const T1& arg1)
	{
		using std::string;
		string result;

		size_t current_offset = 0;

		if (!format.empty())
		{
			for (size_t next_percent = format.find('%');
				next_percent != string::npos;
				next_percent = format.find('%', next_percent))
			{
				// Copy the stuff straight across.
				result += format.substr(current_offset, next_percent - current_offset);
				current_offset = next_percent;

				size_t diff = format.length() - current_offset;

				if (diff > 1)
				{
					int base, width, precision;
					size_t num_eaten;
					size_t arg_to_use = extract_next_arg(format, current_offset, num_eaten, base, width, precision);

					if (arg_to_use != bad_arg)
					{
						switch (arg_to_use)
						{
						case 1:
							result += convert_to_string(arg1, base, width, precision);
							current_offset += num_eaten;
							break;
						default:
							result += ("[!!Bad Argument (" +
								format.substr(current_offset, num_eaten) +
								")!!]");
							current_offset += num_eaten;
							break;
						}
						next_percent += num_eaten;
					}
					else if (format[current_offset + 1] == '%')
					{
						result += "%";
						current_offset += 2;
						next_percent += 2;
					}
					else
					{
						++next_percent;
						// It'll get output the next loop around.
					}
				}
				else
				{
					++next_percent;
				}
			} // while there are still percents.
			if (current_offset < format.length())
			{
				result += format.substr(current_offset);
			}
		}
		return result;
	}

	template <class T1, class T2>
	std::string string_format(const std::string& format, const T1& arg1, const T2& arg2)
	{
		using std::string;
		string result;

		size_t current_offset = 0;

		if (!format.empty())
		{
			for (size_t next_percent = format.find('%');
				next_percent != string::npos;
				next_percent = format.find('%', next_percent))
			{
				// Copy the stuff straight across.
				result += format.substr(current_offset, next_percent - current_offset);
				current_offset = next_percent;

				size_t diff = format.length() - current_offset;

				if (diff > 1)
				{
					int base, width, precision;
					size_t num_eaten;
					size_t arg_to_use = extract_next_arg(format, current_offset, num_eaten, base, width, precision);

					if (arg_to_use != bad_arg)
					{
						switch (arg_to_use)
						{
						case 1:
							result += convert_to_string(arg1, base, width, precision);
							current_offset += num_eaten;
							break;
						case 2:
							result += convert_to_string(arg2, base, width, precision);
							current_offset += num_eaten;
							break;
						default:
							result += ("[!!Bad Argument (" +
								format.substr(current_offset, num_eaten) +
								")!!]");
							current_offset += num_eaten;
							break;
						}
						next_percent += num_eaten;
					}
					else if (format[current_offset + 1] == '%')
					{
						result += "%";
						current_offset += 2;
						next_percent += 2;
					}
					else
					{
						++next_percent;
						// It'll get output the next loop around.
					}
				}
				else
				{
					++next_percent;
				}
			} // while there are still percents.
			if (current_offset < format.length())
			{
				result += format.substr(current_offset);
			}
		}
		return result;
	}

	template <class T1, class T2, class T3>
	std::string string_format(const std::string& format, const T1& arg1, const T2& arg2, const T3& arg3)
	{
		using std::string;
		string result;

		size_t current_offset = 0;

		if (!format.empty())
		{
			for (size_t next_percent = format.find('%');
				next_percent != string::npos;
				next_percent = format.find('%', next_percent))
			{
				// Copy the stuff straight across.
				result += format.substr(current_offset, next_percent - current_offset);
				current_offset = next_percent;

				ptrdiff_t diff = format.length() - current_offset;

				if (diff > 1)
				{
					int base, width, precision;
					size_t num_eaten;
					size_t arg_to_use = extract_next_arg(format, current_offset, num_eaten, base, width, precision);

					if (arg_to_use != bad_arg)
					{
						switch (arg_to_use)
						{
						case 1:
							result += convert_to_string(arg1, base, width, precision);
							current_offset += num_eaten;
							break;
						case 2:
							result += convert_to_string(arg2, base, width, precision);
							current_offset += num_eaten;
							break;
						case 3:
							result += convert_to_string(arg3, base, width, precision);
							current_offset += num_eaten;
							break;
						default:
							result += ("[!!Bad Argument (" +
								format.substr(current_offset, num_eaten) +
								")!!]");
							current_offset += num_eaten;
							break;
						}
						next_percent += num_eaten;
					}
					else if (format[current_offset + 1] == '%')
					{
						result += "%";
						current_offset += 2;
						next_percent += 2;
					}
					else
					{
						++next_percent;
						// It'll get output the next loop around.
					}
				}
				else
				{
					++next_percent;
				}
			} // while there are still percents.
			if (current_offset < format.length())
			{
				result += format.substr(current_offset);
			}
		}
		return result;
	}

	template <class T1, class T2, class T3, class T4>
	std::string string_format(const std::string& format, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4)
	{
		using std::string;
		string result;

		size_t current_offset = 0;

		if (!format.empty())
		{
			for (size_t next_percent = format.find('%');
				next_percent != string::npos;
				next_percent = format.find('%', next_percent))
			{
				// Copy the stuff straight across.
				result += format.substr(current_offset, next_percent - current_offset);
				current_offset = next_percent;

				ptrdiff_t diff = format.length() - current_offset;

				if (diff > 1)
				{
					int base, width, precision;
					size_t num_eaten;
					size_t arg_to_use = extract_next_arg(format, current_offset, num_eaten, base, width, precision);

					if (arg_to_use != bad_arg)
					{
						switch (arg_to_use)
						{
						case 1:
							result += convert_to_string(arg1, base, width, precision);
							current_offset += num_eaten;
							break;
						case 2:
							result += convert_to_string(arg2, base, width, precision);
							current_offset += num_eaten;
							break;
						case 3:
							result += convert_to_string(arg3, base, width, precision);
							current_offset += num_eaten;
							break;
						case 4:
							result += convert_to_string(arg4, base, width, precision);
							current_offset += num_eaten;
							break;
						default:
							result += ("[!!Bad Argument (" +
								format.substr(current_offset, num_eaten) +
								")!!]");
							current_offset += num_eaten;
							break;
						}
						next_percent += num_eaten;
					}
					else if (format[current_offset + 1] == '%')
					{
						result += "%";
						current_offset += 2;
						next_percent += 2;
					}
					else
					{
						++next_percent;
						// It'll get output the next loop around.
					}
				}
				else
				{
					++next_percent;
				}
			} // while there are still percents.
			if (current_offset < format.length())
			{
				result += format.substr(current_offset);
			}
		}
		return result;
	}

	template <class T1, class T2, class T3, class T4, class T5>
	std::string string_format(const std::string& format, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5)
	{
		using std::string;
		string result;

		size_t current_offset = 0;

		if (!format.empty())
		{
			for (size_t next_percent = format.find('%');
				next_percent != string::npos;
				next_percent = format.find('%', next_percent))
			{
				// Copy the stuff straight across.
				result += format.substr(current_offset, next_percent - current_offset);
				current_offset = next_percent;

				ptrdiff_t diff = format.length() - current_offset;

				if (diff > 1)
				{
					int base, width, precision;
					size_t num_eaten;
					size_t arg_to_use = extract_next_arg(format, current_offset, num_eaten, base, width, precision);

					if (arg_to_use != bad_arg)
					{
						switch (arg_to_use)
						{
						case 1:
							result += convert_to_string(arg1, base, width, precision);
							current_offset += num_eaten;
							break;
						case 2:
							result += convert_to_string(arg2, base, width, precision);
							current_offset += num_eaten;
							break;
						case 3:
							result += convert_to_string(arg3, base, width, precision);
							current_offset += num_eaten;
							break;
						case 4:
							result += convert_to_string(arg4, base, width, precision);
							current_offset += num_eaten;
							break;
						case 5:
							result += convert_to_string(arg5, base, width, precision);
							current_offset += num_eaten;
							break;
						default:
							result += ("[!!Bad Argument (" +
								format.substr(current_offset, num_eaten) +
								")!!]");
							current_offset += num_eaten;
							break;
						}
						next_percent += num_eaten;
					}
					else if (format[current_offset + 1] == '%')
					{
						result += "%";
						current_offset += 2;
						next_percent += 2;
					}
					else
					{
						++next_percent;
						// It'll get output the next loop around.
					}
				}
				else
				{
					++next_percent;
				}
			} // while there are still percents.
			if (current_offset < format.length())
			{
				result += format.substr(current_offset);
			}
		}
		return result;
	}

	template <class T1, class T2, class T3, class T4, class T5, class T6>
	std::string string_format(const std::string& format, const T1& arg1, const T2& arg2, const T3& arg3, const T4& arg4, const T5& arg5, const T6& arg6)
	{
		using std::string;
		string result;

		size_t current_offset = 0;

		if (!format.empty())
		{
			for (size_t next_percent = format.find('%');
				next_percent != string::npos;
				next_percent = format.find('%', next_percent))
			{
				// Copy the stuff straight across.
				result += format.substr(current_offset, next_percent - current_offset);
				current_offset = next_percent;

				ptrdiff_t diff = format.length() - current_offset;

				if (diff > 1)
				{
					int base, width, precision;
					size_t num_eaten;
					size_t arg_to_use = extract_next_arg(format, current_offset, num_eaten, base, width, precision);

					if (arg_to_use != bad_arg)
					{
						switch (arg_to_use)
						{
						case 1:
							result += convert_to_string(arg1, base, width, precision);
							current_offset += num_eaten;
							break;
						case 2:
							result += convert_to_string(arg2, base, width, precision);
							current_offset += num_eaten;
							break;
						case 3:
							result += convert_to_string(arg3, base, width, precision);
							current_offset += num_eaten;
							break;
						case 4:
							result += convert_to_string(arg4, base, width, precision);
							current_offset += num_eaten;
							break;
						case 5:
							result += convert_to_string(arg5, base, width, precision);
							current_offset += num_eaten;
							break;
						case 6:
							result += convert_to_string(arg6, base, width, precision);
							current_offset += num_eaten;
							break;
						default:
							result += ("[!!Bad Argument (" +
								format.substr(current_offset, num_eaten) +
								")!!]");
							current_offset += num_eaten;
							break;
						}
						next_percent += num_eaten;
					}
					else if (format[current_offset + 1] == '%')
					{
						result += "%";
						current_offset += 2;
						next_percent += 2;
					}
					else
					{
						++next_percent;
						// It'll get output the next loop around.
					}
				}
				else
				{
					++next_percent;
				}
			} // while there are still percents.
			if (current_offset < format.length())
			{
				result += format.substr(current_offset);
			}
		}
		return result;
	}

} // namespace amethyst


#endif /* !defined(AMETHYST__STRING_FORMAT_HPP) */
