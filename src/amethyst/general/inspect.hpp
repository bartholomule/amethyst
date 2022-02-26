#pragma once

#include <string>
#include <type_traits>
#include <set>
#include <map>
#include <vector>
#include <list>
#include <memory>
#include <functional>
#include <ostream>

namespace amethyst
{
    namespace pod
    {
        // Returns "true" or "false"
        std::string inspect(bool b);

        // All integral types supported by std::to_string
        template <typename T>
        inline typename std::enable_if<std::is_arithmetic<T>::value, std::string>::type inspect(T i)
        {
            return std::to_string(i);
        }

        // Converts to a quoted string or returns <nullptr>
        // Example:
        //   inspect("abcd") => std::string("\"abcd\"")
        //   inspect((char*)nullptr) => std::string("<nullptr>")
        std::string inspect(const char* text);
    }

    using pod::inspect;

    template <typename Iterator>
    std::string inspectRange(Iterator first, Iterator last, const std::string& head, const std::string& foot, const std::string& joiner)
    {
        using ::amethyst::pod::inspect;
        std::string result;
        result += head;

        if (first != last)
        {
            result += inspect(*first);
            for (++first; first != last; ++first)
            {
                result += joiner + inspect(*first);
            }
        }

        result += foot;
        return result;
    }
}

namespace std // for ADL.
{
    using ::amethyst::pod::inspect;

    std::string inspect(const std::string& s);

    template <typename K, typename V, typename S>
    std::string inspect(const std::map<K, V, S>& m)
    {
        using ::amethyst::pod::inspect;
        std::string result;
        result += "{ ";

        auto first = m.begin();
        auto last = m.end();
        if (first != last)
        {
            result += inspect(first->first) + " => " + inspect(first->second);
            for (++first; first != last; ++first)
            {
                result += ", " + inspect(first->first) + " => " + inspect(first->second);
            }
        }

        result += " }";
        return result;
    }

    template <typename T, typename A>
    std::string inspect(const std::vector<T, A>& vec)
    {
        return amethyst::inspectRange(vec.begin(), vec.end(), "[ ", " ]", ", ");
    }

    template <typename T, typename A>
    std::string inspect(const std::set<T, A>& container)
    {
        return amethyst::inspectRange(container.begin(), container.end(), "| ", " |", ", ");
    }

    template <typename T, typename A>
    std::string inspect(const std::list<T, A>& container)
    {
        return amethyst::inspectRange(container.begin(), container.end(), "< ", " >", ", ");
    }

    template <typename F, typename S>
    std::string inspect(const std::pair<F, S>& p)
    {
        return "( " + inspect(p.first) + ", " + inspect(p.second) + " )";
    }

    template <typename T>
    std::string inspect(const std::unique_ptr<T>& sp)
    {
        if (sp) {
            return inspect(*sp);
        }
        return "<nullptr>";
    }

    template <typename T>
    std::string inspect(const std::shared_ptr<T>& sp)
    {
        if (sp) {
            return inspect(*sp);
        }
        return "<nullptr>";
    }

    // Just reports if the string has a value or not.  This is provided
    // separately from the generic container version below (which this could
    // also match) to allow for implicit conversions and avoid template bloat.
    // This is useful for dumping sensitive data.  It will return <empty> or
    // <value present>.
    std::string inspectEmptiness(const std::string& s);

    template <typename Fn>
    inline std::string inspectEmptiness(const std::function<Fn>& fn)
    {
        if (!fn) {
            return "<null function>";
        }
        return "<valid function>";
    }

    // Allow anything with an empty() function returning something convertible
    // to bool to be inspected.
    template <typename Container>
    inline auto inspectEmptiness(const Container& c)->
    typename std::enable_if<
        std::is_convertible<
            decltype(c.empty()),
            bool
            >::value,
        std::string
        >::type
    {
        if (c.empty()) {
            return "<empty>";
        }
        return "<value present>";
    }
}

namespace amethyst
{
    // bring these back in for implicit conversions.
    using std::inspectEmptiness;
}


// TEMPORARY HACK!
namespace std
{
    template <typename T>
    std::ostream& operator<<(std::ostream& o, const std::shared_ptr<T>& p)
    {
        if (p)
            o << *p;
        else
            o << "<nullptr>";
        return o;
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& o, const std::unique_ptr<T>& p)
    {
        if (p)
            o << *p;
        else
            o << "<nullptr>";
        return o;
    }
}
