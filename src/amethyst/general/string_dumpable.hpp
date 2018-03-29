#pragma once
#include <string>
#include <iosfwd>

namespace amethyst
{
    /**
     *
     * The base class for string dumpable objects.  Anything derived
     * from this can have its members converted (one way) to a string.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.1 $
     *
     */
    class string_dumpable
    {
    public:
        string_dumpable() = default;
        virtual ~string_dumpable();

        virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const = 0;

        virtual std::string to_string(const std::string& base_indentation = "",
                                      const std::string& level_indentation = "  ") const;

        virtual std::string name() const = 0;

    };

    std::ostream& operator<<(std::ostream& o, const string_dumpable& s);
    std::string inspect(const string_dumpable& s);
}