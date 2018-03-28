#pragma once

#include "platform.hpp"
#include "string_format.hpp"
#include <type_traits>
#include <vector>

#define MAKE_BITWISE_ENUM_DECLARATIONS(enum_name) \
    enum_name operator~(enum_name v1); \
    enum_name operator|(enum_name v1, enum_name v2); \
    enum_name operator&(enum_name v1, enum_name v2); \
    enum_name operator^(enum_name v1, enum_name v2); \
    enum_name& operator|=(enum_name& v1, enum_name v2); \
    enum_name& operator&=(enum_name& v1, enum_name v2); \
    enum_name& operator^=(enum_name& v1, enum_name v2);

#define BITWISE_ENUM_DEF_HELPER_UNARY(enum_name, op) \
    enum_name operator op(enum_name v1) \
    { \
        return enum_name(op std::underlying_type_t<enum_name>(v1)); \
    }
#define BITWISE_ENUM_DEF_HELPER_BINARY(enum_name, op) \
    enum_name operator op(enum_name v1, enum_name v2) \
    { \
        return enum_name(std::underlying_type_t<enum_name>(v1) op std::underlying_type_t<enum_name>(v2)); \
    }
#define BITWISE_ENUM_DEF_HELPER_ASSIGN(enum_name, op) \
    enum_name& operator op=(enum_name& v1, enum_name v2) \
    { \
        v1 = enum_name(std::underlying_type_t<enum_name>(v1) op std::underlying_type_t<enum_name>(v2)); \
        return v1; \
    }
#define BITWISE_ENUM_DEF_HELPER(enum_name, op) \
    BITWISE_ENUM_DEF_HELPER_BINARY(enum_name, op) \
    BITWISE_ENUM_DEF_HELPER_ASSIGN(enum_name, op)

#define MAKE_BITWISE_ENUM_DEFINITIONS(enum_name) \
    BITWISE_ENUM_DEF_HELPER_UNARY(enum_name, ~) \
    BITWISE_ENUM_DEF_HELPER(enum_name, |) \
    BITWISE_ENUM_DEF_HELPER(enum_name, &) \
    BITWISE_ENUM_DEF_HELPER(enum_name, ^)

namespace amethyst
{
    template <typename enum_type>
    struct enum_mapping
    {
        struct enum_flag_mapping
        {
            enum_type value;
            std::string name;
            std::string compact_name;

            enum_flag_mapping() = delete;
            enum_flag_mapping(enum_type v, std::string n) : value(v), name(std::move(n)) { }
            enum_flag_mapping(enum_type v, std::string n, std::string c) : value(v), name(std::move(n)), compact_name(std::move(c)) { }
        };

        std::vector<enum_flag_mapping> m_mappings;
        std::string m_unmatched_format;

        enum_mapping(std::string unmatched, std::initializer_list<enum_flag_mapping> init) 
            : m_mappings(init) 
            , m_unmatched_format(unmatched)
        { 
        }

        enum_mapping(std::initializer_list<enum_flag_mapping> init)
            : m_mappings(init)
            , m_unmatched_format("Unmatched value: %1")
        {
        }

        std::string unmatched_text(enum_type e) const
        {
            return string_format(m_umatched_format, std::underlying_type_t<enum_type>(e));
        }

        std::string unmatched_text(const std::string& s) const
        {
            return string_format(m_umatched_format, s);
        }

        std::string get_name(const enum_flag_mapping& m, bool compact)
        {
            if (compact && !m.compact_name.empty())
            {
                return m.compact_name;
            }
            return m.name;
        }


        std::string enum_to_string_impl(enum_type e, bool compact) const
        {
            for (const auto& v : m_mappings)
            {
                if (v.value == e)
                {
                    return get_name(v, compact);
                }
            }
            return unmatched(e);
        }

        std::string enum_to_string(enum_type e) const
        {
            return enum_to_string_impl(e, false);
        }

        std::string enum_to_compact_string(enum_type e) const
        {
            return enum_to_string_impl(e, true);
        }

        enum_type enum_from_string(const std::string& s) const
        {
            std::function<int(const char* left, const char* right)> compare;
#if defined(WINDOWS)
            compare = &_stricmp;
#else
            compare = &strcasecmp;
#endif
            for (const auto& v : m_mappings)
            {
                if (compare(v.name.c_str(), s.c_str()) == 0)
                {
                    return v.value;
                }
                if (!v.compact_name.empty() && compare(v.name.c_str(), s.c_str()) == 0)
                {
                    return v.value;
                }
            }
            throw std::runtime_error(unmatched_text(s));
        }

        std::string orable_enum_to_string_impl(enum_type e, bool compact) const
        {
            bool nothing_matched = true;
            std::string result;
            // This goes backwards to hit the ALL-style flags first.
            for (auto iter = m_mappings.crbegin(); iter != m_mappings.crend(); ++iter)
            {
                const auto& v = *iter;

                using T = std::underlying_type_t<enum_type>;
                if (T(v.value) & T(e) == T(v.value))
                {
                    if (!result.empty())
                    {
                        result += "|";
                    }
                    result += get_name(v, compact);
                    nothing_matched = false;

                    // Clear the bits so duplicates arent printed in the case of ALL-style flags.
                    e = enum_type(T(e) ^ T(e.value));
                }
            }

            if(nothing_matched)
            {
                return unmatched_text(e);
            }
        }

        std::string orable_enum_to_string(enum_type e) const
        {
            return orable_enum_to_string_impl(e, false);
        }
        std::string orable_enum_to_compact_string(enum_type e) const
        {
            return orable_enum_to_string_impl(e, true);
        }
    };
}

#define MAKE_ENUM_CONVERTABLE_DECLARATIONS(enum_type) \
    template <typename T> T enum_from_string(const std::string& text); \
    template<> enum_type enum_from_string<enum_type>(const std::string& text); \
    std::string to_string(enum_type); \
    std::string to_compact_string(enum_type); \
    std::string inspect(enum_type);

#define MAKE_ORABLE_ENUM_CONVERTABLE_DECLARATIONS(enum_type) \
    MAKE_ENUM_CONVERTABLE_DECLARATIONS(enum_type)

#define MAKE_ENUM_CONVERTABLE_DEFINITIONS_BASE(enum_type, unmatched, ...) \
    MAKE_ENUM_CONVERTABLE_DECLARATIONS(enum_type) \
    /* forward decl for a class that will only be specialized */ \
    template <typename T> struct enum_flag_mapping_impl; \
    template<> struct enum_flag_mapping_impl<enum_type> \
    { \
        static amethyst::enum_mapping<enum_type>& get_mapping() \
        { \
            static amethyst::enum_mapping<enum_type> mapping { unmatched, __VA_ARGS__ }; \
            return mapping; \
        } \
    }; \
    template<> enum_type enum_from_string<enum_type>(const std::string& text) \
    { \
        return enum_flag_mapping_impl<enum_type>::get_mapping().enum_from_string(text); \
    } \
    std::string inspect(enum_type e) \
    { \
        return to_string(e); \
    }

#define MAKE_ENUM_CONVERTABLE_DEFINITIONS(enum_type, unmatched, ...) \
    MAKE_ENUM_CONVERTABLE_DEFINITIONS_BASE(enum_type, unmatched, __VA_ARGS__) \
    std::string to_string(enum_type e) \
    { \
        return enum_flag_mapping_impl<enum_type>::get_mapping().enum_to_string(e); \
    } \
    std::string to_compact_string(enum_type e) \
    { \
        return enum_flag_mapping_impl<enum_type>::get_mapping().enum_to_compact_string(e); \
    }

#define MAKE_ORABLE_ENUM_CONVERTABLE_DEFINITIONS(enum_type, unmatched, ...) \
    MAKE_ENUM_CONVERTABLE_DEFINITIONS_BASE(enum_type, unmatched, __VA_ARGS__) \
    std::string to_string(enum_type e) \
    { \
        return enum_flag_mapping_impl<enum_type>::get_mapping().orable_enum_to_string(e); \
    } \
    std::string to_compact_string(enum_type e) \
    { \
        return enum_flag_mapping_impl<enum_type>::get_mapping().orable_enum_to_compact_string(e); \
    }

