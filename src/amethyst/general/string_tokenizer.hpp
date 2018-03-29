/*
 * $Id: string_tokenizer.hpp,v 1.1 2006/03/27 03:44:27 kpharris Exp $
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

#if       !defined(AMETHYST_STRING_TOKENIZER_HPP)
#define            AMETHYST_STRING_TOKENIZER_HPP

#include <vector>
#include <string>

namespace amethyst
{
    namespace tokenizer
    {
        enum delimiter_action_types
        {
            IGNORE_DELIMITERS,
            RETURN_DELIMITERS
        };
        enum token_action_flags
        {
            IGNORE_EMPTY_TOKENS,
            RETURN_EMPTY_TOKENS
        };
    } // namespace tokenizer

    /**
     * Tokenize a string, returning the tokens (and possibly delimiters) in a
     * vector.  This is capable of returning the delimiters, or skipping them,
     * and returning empty tokens or skipping them.
     */
    std::vector<std::string> tokenize(
        const std::string& s,
        const char* delims = "\n\r\t ",
        tokenizer::delimiter_action_types keep_delims = tokenizer::IGNORE_DELIMITERS,
        tokenizer::token_action_flags keep_tokens = tokenizer::IGNORE_EMPTY_TOKENS);
} // namespace amethyst

#endif /* !defined(AMETHYST_STRING_TOKENIZER_HPP) */
