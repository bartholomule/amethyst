#include "string_tokenizer.hpp"
#include <cstring>

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
                        if (keep_empty_tokens == tokenizer::token_action_flags::RETURN_EMPTY_TOKENS)
                        {
                            // We can only call it an empty token if it is between
                            // tokens, not at the beginning of the string.
                            if (ptr != str)
                            {
                                results.push_back(std::string());
                            }
                        }
                    }
                    if (keep_delims == tokenizer::delimiter_action_types::RETURN_DELIMITERS)
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
}
