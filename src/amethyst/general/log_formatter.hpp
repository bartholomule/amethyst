/*
 * $Id: log_formatter.hpp,v 1.2 2006/04/05 05:57:08 kpharris Exp $
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

#if       !defined(AMETHYST__LOG_FORMATTER_HPP)
#define            AMETHYST__LOG_FORMATTER_HPP

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
	 *
	 * The base class for objects that will do log formatting.
	 *
	 * @author Kevin Harris <kpharris@users.sourceforge.net>
	 * @version $Revision: 1.2 $
	 *
	 */
	class log_formatter
	{

	private:
		formatter_conversion_list* conversion_list;

	protected:
		virtual std::vector<char> do_format_message(const UnformattedMessage& m);
	public:
		/** Default constructor */
		log_formatter();

		/** Destructor */
		virtual ~log_formatter();

		/** Copy constructor */
		log_formatter(const log_formatter& old);

		/** Assignment operator */
		log_formatter& operator= (const log_formatter& old);

		std::vector<char> format_message(const UnformattedMessage& m);

		static std::shared_ptr<log_formatter> create_log_formatter(const std::string& format_string);

	}; // class log_formatter

} // namespace amethyst


#endif /* !defined(AMETHYST__LOG_FORMATTER_HPP) */

