#pragma once

namespace amethyst
{
    template <typename T, T default_value>
    class defaulted_value
    {
    public:
        defaulted_value() = default;
        defaulted_value(const defaulted_value&) = default;
        defaulted_value(const T& val) : m_value(val) { }      
        defaulted_value(T&& val) : m_value(std::move(val)) { }
        defaulted_value(defaulted_value&& v) : m_value(std::move(v.m_value)) { }
        ~defaulted_value() = default;

        defaulted_value& operator=(const defaulted_value&) = default;
        defaulted_value& operator=(defaulted_value&& old) { m_value = std::move(old.m_value); return *this; }
        operator T() const { return m_value; }

        T& value() { return m_value; }
        const T& value() const { return m_value; }
    private:
        T m_value = default_value;
    };

    class ibool : public defaulted_value<bool, false>
    {
        using parent = defaulted_value<bool, false>;
    public:
        using parent::parent;
    };
}