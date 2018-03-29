/*
 * $Id: string_tokenizer.cpp,v 1.2 2011/10/28 00:49:04 kpharris Exp $
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

#include "string_tokenizer.hpp"
#include <cstring>

// Temp (for debugging)
#include "string_format.hpp"
#include <iostream>


namespace amethyst
{
    std::vector<std::string> tokenize(
        const std::string& s,
        const char* delims,
        tokenizer::delimiter_action_types keep_delims,
        tokenizer::token_action_flags keep_empty_tokens )
    {
        std::vector<std::string> results;

        // Empty strings can't be tokenized.
        if (s.empty())
        {
            return results;
        }
        // Null delimiters mean nothing to tokenize.
        if (!delims)
        {
            results.push_back(s);
        }
        else
        {
            size_t length = s.length();
            char const* str = s.c_str();
            char const* str_end = str + length; // include the NULL.
            char const* last_non_delim = NULL;

            char const* ptr;
            for (ptr = str; ptr != str_end; ++ptr)
            {
                const char* token = strchr(delims, *ptr);
                if (token && *token)
                {
                    // We've got a delimiter...
                    if (last_non_delim)
                    {
                        results.push_back(std::string(last_non_delim, ptr));
                        last_non_delim = NULL;
                    }
                    else
                    {
                        // Multiple delimiters in a row...
                        if (keep_empty_tokens == tokenizer::RETURN_EMPTY_TOKENS)
                        {
                            // We can only call it an empty token if it is between
                            // tokens, not at the beginning of the string.
                            if (ptr != str)
                            {
                                results.push_back(std::string());
                            }
                        }
                    }
                    if (keep_delims == tokenizer::RETURN_DELIMITERS)
                    {
                        results.push_back(std::string(token, 1));
                    }
                }
                else // Not one of the delimiters.
                {
                    // If we haven't seen a non-delimiter character, set this one to be it.
                    if (!last_non_delim)
                    {
                        last_non_delim = ptr;
                    }
                }
            }
            // Append any leftover text.
            if (last_non_delim)
            {
                results.push_back(std::string(last_non_delim, ptr));
            }
        }
        return results;
    }
} // namespace amethyst
