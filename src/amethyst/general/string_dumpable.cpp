#include "string_dumpable.hpp"
#include <ostream>

namespace amethyst
{
    string_dumpable::~string_dumpable() = default;

    std::string string_dumpable::to_string(const std::string& base_indentation,
                                           const std::string& level_indentation) const
    {
        return
            base_indentation + name() + "\n" +
            base_indentation + "{\n" +
            internal_members(base_indentation + level_indentation, true) +
            base_indentation + "}"
        ;
    }

    std::ostream& operator<<(std::ostream& o, const string_dumpable& s)
    {
        o << s.to_string();
        return o;
    }

    std::string inspect(const string_dumpable& s)
    {
        return s.to_string();
    }
}
