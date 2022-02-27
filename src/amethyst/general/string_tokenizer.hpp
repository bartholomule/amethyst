#pragma once

#include <vector>
#include <string>

namespace amethyst
{
    namespace tokenizer
    {
        enum class delimiter_action_types
        {
            IGNORE_DELIMITERS,
            RETURN_DELIMITERS
        };
        enum class token_action_flags
        {
            IGNORE_EMPTY_TOKENS,
            RETURN_EMPTY_TOKENS
        };
    }

    /**
     * Tokenize a string, returning the tokens (and possibly delimiters) in a
     * vector.  This is capable of returning the delimiters, or skipping them,
     * and returning empty tokens or skipping them.
     */
    std::vector<std::string> tokenize(
        const std::string& s,
        const char* delims = "\n\r\t ",
        tokenizer::delimiter_action_types keep_delims = tokenizer::delimiter_action_types::IGNORE_DELIMITERS,
        tokenizer::token_action_flags keep_tokens = tokenizer::token_action_flags::IGNORE_EMPTY_TOKENS);
}