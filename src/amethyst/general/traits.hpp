#pragma once

#include <type_traits>
#include <cstdint>

namespace amethyst
{
    // standard int/char size manipulations
    constexpr size_t min_int_bytes = 1;
    constexpr size_t max_int_bytes = 8;
    template <size_t bytes> struct char_of_size { };
    template <size_t bytes> struct int_of_size { };
    template <size_t bytes> struct uint_of_size { };
    template <typename T> struct bigger_int { using type = typename int_of_size<(sizeof(T) == max_int_bytes) ? max_int_bytes : sizeof(T) * 2>::type; };
    template <typename T> struct bigger_uint { using type = typename uint_of_size<(sizeof(T) == max_int_bytes) ? max_int_bytes : sizeof(T) * 2>::type; };
    template <typename T> struct smaller_int { using type = typename int_of_size<(sizeof(T) == min_int_bytes) ? min_int_bytes : sizeof(T) / 2>::type; };
    template <typename T> struct smaller_uint { using type = typename uint_of_size<(sizeof(T) == min_int_bytes) ? min_int_bytes : sizeof(T) / 2>::type; };
    template <> struct int_of_size<1> { using type = int8_t; };
    template <> struct int_of_size<2> { using type = int16_t; };
    template <> struct int_of_size<4> { using type = int32_t; };
    template <> struct int_of_size<8> { using type = int64_t; };
    template <> struct uint_of_size<1> { using type = uint8_t; };
    template <> struct uint_of_size<2> { using type = uint16_t; };
    template <> struct uint_of_size<4> { using type = uint32_t; };
    template <> struct uint_of_size<8> { using type = uint64_t; };
    template <> struct char_of_size<1> { using type = char; };
    template <> struct char_of_size<2> { using type = char16_t; };
    template <> struct char_of_size<4> { using type = char32_t; };
    // wchar_t is an independent type from all of the other chars, but can have the same size as one.
    // On windows, this will be char16_t, and on linux will be char32_t
    using wchar_equivalent = char_of_size<sizeof(wchar_t)>::type;

    // Compile-time unit tests
    static_assert(std::is_same<typename int_of_size<1>::type, int8_t>::value, "size failure");
    static_assert(std::is_same<typename int_of_size<2>::type, int16_t>::value, "size failure");
    static_assert(std::is_same<typename int_of_size<4>::type, int32_t>::value, "size failure");
    static_assert(std::is_same<typename int_of_size<8>::type, int64_t>::value, "size failure");
    static_assert(std::is_same<typename uint_of_size<1>::type, uint8_t>::value, "size failure");
    static_assert(std::is_same<typename uint_of_size<2>::type, uint16_t>::value, "size failure");
    static_assert(std::is_same<typename uint_of_size<4>::type, uint32_t>::value, "size failure");
    static_assert(std::is_same<typename uint_of_size<8>::type, uint64_t>::value, "size failure");
    static_assert(std::is_same<typename bigger_int<int8_t>::type, int16_t>::value, "size failure");
    static_assert(std::is_same<typename bigger_int<int16_t>::type, int32_t>::value, "size failure");
    static_assert(std::is_same<typename bigger_int<int32_t>::type, int64_t>::value, "size failure");
    static_assert(std::is_same<typename bigger_int<int64_t>::type, int64_t>::value, "size failure");
    static_assert(std::is_same<typename bigger_uint<uint8_t>::type, uint16_t>::value, "size failure");
    static_assert(std::is_same<typename bigger_uint<uint16_t>::type, uint32_t>::value, "size failure");
    static_assert(std::is_same<typename bigger_uint<uint32_t>::type, uint64_t>::value, "size failure");
    static_assert(std::is_same<typename bigger_uint<uint64_t>::type, uint64_t>::value, "size failure");
    static_assert(std::is_same<typename smaller_int<int8_t>::type, int8_t>::value, "size failure");
    static_assert(std::is_same<typename smaller_int<int16_t>::type, int8_t>::value, "size failure");
    static_assert(std::is_same<typename smaller_int<int32_t>::type, int16_t>::value, "size failure");
    static_assert(std::is_same<typename smaller_int<int64_t>::type, int32_t>::value, "size failure");
    static_assert(std::is_same<typename smaller_uint<uint8_t>::type, uint8_t>::value, "size failure");
    static_assert(std::is_same<typename smaller_uint<uint16_t>::type, uint8_t>::value, "size failure");
    static_assert(std::is_same<typename smaller_uint<uint32_t>::type, uint16_t>::value, "size failure");
    static_assert(std::is_same<typename smaller_uint<uint64_t>::type, uint32_t>::value, "size failure");

    namespace traits
    {
        // A type which only exists to be a valid type for type substitution.
        enum class enabler_t {};

        // A wrapper around std::enable_if to remove the need for the typename keyword, a
        // normally-useless type specifier, and the ::type at the end.
        // Example:
        // template <typename T, typename E = enable_if<false>>
        // void foo(const T& t) { ... }
        // foo(1); // foo was deleted because of SFINAE
        // template <typename T, typename E = enable_if<true>>
        // void bar(const T& t) { ... }
        // bar(2); // okay.
        template <bool condition_v>
        using enable_if_t = typename std::enable_if<condition_v, enabler_t>::type;
        // Inverted version of enable_if
        template <bool condition_v>
        using enable_unless_t = typename std::enable_if<!condition_v, enabler_t>::type;
        // Shortcut for enable_if<Something<...>::value>
        template <typename condition_t>
        using enable_if_v = typename std::enable_if<condition_t::value, enabler_t>::type;
        // Shortcut for enable_unless
        template <typename condition_t>
        using enable_unless_v = typename std::enable_if<!condition_t::value, enabler_t>::type;

        // Functionally equivalent to std::true_type
        struct false_t { static const bool value = false; };
        struct true_t { static const bool value = true; };

        // Will not be a functional type if any of the parameters are invalid.
        // SFINAE will remove the type if not all template parameters are valid.
        template <typename... Ts>
        struct all_parameters_are_valid : public true_t { };

        // Useful for testing a free binary operator, or in some cases a member operator.
        //
        // Name will be used for the defined traits name.
        // OP is the operator to use (eg. <<, +, %, etc).
        // REFSPEC can be &, const&, or empty, depending on the variant needed.
#define DEFINE_TRAITS_FOR_OPERATOR(NAME, OP, REFSPEC) \
        template <typename left_t, typename right_t> \
        struct NAME \
        { \
        private: \
            template <typename l_t, typename r_t> \
            static auto NAME##testfn(int) -> decltype(std::declval<l_t REFSPEC>() OP std::declval<r_t>(), amethyst::traits::true_t()); \
            template <typename l_t, typename r_t> \
            static auto NAME##testfn(...) -> amethyst::traits::false_t; \
        public: \
            static const bool value = decltype(NAME##testfn<left_t, typename std::remove_cv<right_t>::type>(0))::value; \
        }

        // Test for a unary operator.
        // Example:
        //   DEFINE_TRAITS_FOR_UNARY_OPERATOR(HasUnaryOperatorStar, *, const&);
        //   static_assert(HasUnaryOperatorStar<typename std::vector<int>::const_iterator>::value, "Iterators should have operator*");
#define DEFINE_TRAITS_FOR_UNARY_OPERATOR(NAME, OP, REFSPEC) \
        template <typename left_t, typename... Ts> \
        struct NAME \
        { \
        private: \
            template <typename l_t, typename... r_tts> \
            static auto NAME##testfn(int) -> decltype(OP std::declval<l_t REFSPEC>(std::declval<r_tts>()...), amethyst::traits::true_t()); \
            template <typename l_t, typename... r_tts> \
            static auto NAME##testfn(...) -> amethyst::traits::false_t; \
        public: \
            static const bool value = decltype(NAME##testfn<left_t, typename std::remove_cv<Ts>::type...>(0))::value; \
        }

        // Similar to DEFINE_TRAITS_FOR_OPERATOR, but tests a type for a member
        // function.
        //
        // MEMFUN is a non-static member function which will be testable for
        // any number of arguments.
        //
        // Example:
        //   DEFINE_TRAITS_FOR_MEMBER(HasAppendFunction, append, &);
        //   static_assert(HasAppendFunction<std::string, std::string>::value, "Yippie");
#define DEFINE_TRAITS_FOR_MEMBER(NAME, MEMFUN, REFSPEC)   \
        template <typename left_t, typename... Ts> \
        struct NAME \
        { \
        private: \
            template <typename L, typename... TTs> \
            static auto NAME##testfn(int) -> decltype(std::declval<L REFSPEC>().MEMFUN(std::declval<TTs>()...), amethyst::traits::true_t()); \
            template <typename L, typename... TTs> \
            static auto NAME##testfn(...) -> amethyst::traits::false_t; \
        public: \
            static const bool value = decltype(NAME##testfn<left_t, typename std::remove_cv<Ts>::type...>(0))::value; \
        }

        DEFINE_TRAITS_FOR_UNARY_OPERATOR(has_operator_star_t, *, const&);
        DEFINE_TRAITS_FOR_UNARY_OPERATOR(has_preincrement_t, ++, &);
        template <typename T> struct has_postincrement_t { static const bool value = has_preincrement_t<T, int>::value; };
        DEFINE_TRAITS_FOR_UNARY_OPERATOR(has_predecrement_t, --, &);
        template <typename T> struct has_postdecrement_t { static const bool value = has_predecrement_t<T, int>::value; };

        DEFINE_TRAITS_FOR_OPERATOR(has_multiply_t, *, const&);
        DEFINE_TRAITS_FOR_OPERATOR(has_divide_t, /, const&);
        DEFINE_TRAITS_FOR_OPERATOR(has_add_t, +, const&);
        DEFINE_TRAITS_FOR_OPERATOR(has_subtract_t, -, const&);
        DEFINE_TRAITS_FOR_OPERATOR(has_lshift_t, <<, const&);
        DEFINE_TRAITS_FOR_OPERATOR(has_rshift_t, >>, const&);

        DEFINE_TRAITS_FOR_MEMBER(has_size_function_t, size, const&);
        DEFINE_TRAITS_FOR_MEMBER(has_begin_function_t, begin,);
        DEFINE_TRAITS_FOR_MEMBER(has_end_function_t, begin,);
        DEFINE_TRAITS_FOR_MEMBER(has_const_begin_function_t, begin, const&);
        DEFINE_TRAITS_FOR_MEMBER(has_const_end_function_t, begin, const&);

    }
}