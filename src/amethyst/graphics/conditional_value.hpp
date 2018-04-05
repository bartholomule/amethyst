#pragma once
#include <cassert>

namespace amethyst
{
    template <class T>
    struct nonzero
    {
        bool operator()(const T& value) const
        {
            return !!value;
        }
    };
    /**
     *
     * A conditional value -- A value that has a precondition upon access.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.3 $
     *
     */
    template <class value_type,
              class test_type = bool,
              class condition_test = nonzero<test_type>>
    class conditional_value
    {
    public:
        conditional_value() = default;
        explicit conditional_value(value_type val)
            : value(val)
        {
        }
        conditional_value(value_type val, test_type test_val)
            : value(val)
            , test_value(test_val)
        {
        }
        virtual ~conditional_value() = default;
        conditional_value(const conditional_value& old) = default;
        conditional_value& operator=(const conditional_value& old) = default;

        value_type operator*() const;
        value_type& operator*();

        const value_type* operator->() const;
        value_type* operator->();

        test_type get_test_value() const { return test_value; }
        void set_test_value(const test_type& test_val ) const { test_value = test_val; }

        bool do_test() const { return test(test_value); }

        const value_type& peek_at_value() const { return value; }

    private:
        value_type value;
        test_type test_value = test_type(0);
        condition_test test;
    };

    template <class value_type, class test_type, class condition_test>
    value_type conditional_value<value_type, test_type, condition_test>::operator*() const
    {
        // FIXME!
        assert(test(test_value));
        return value;
    }

    template <class value_type, class test_type, class condition_test>
    value_type& conditional_value<value_type, test_type, condition_test>::operator*()
    {
        // FIXME!
        assert(test(test_value));
        return value;
    }

    template <class value_type, class test_type, class condition_test>
    const value_type* conditional_value<value_type, test_type, condition_test>::operator->() const
    {
        // FIXME!
        assert(test(test_value));
        return &value;
    }

    template <class value_type, class test_type, class condition_test>
    value_type* conditional_value<value_type, test_type, condition_test>::operator->()
    {
        // FIXME!
        assert(test(test_value));
        return &value;
    }
}