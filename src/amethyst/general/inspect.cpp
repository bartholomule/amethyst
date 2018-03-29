#include "inspect.hpp"

namespace amethyst
{
    namespace pod_inspector
    {
        std::string inspect(bool b)
        {
            if (b)
            {
                return "true";
            }
            return "false";
        }

        std::string inspect(const char* text)
        {
            if (text)
            {
                return "\"" + std::string(text) + "\"";
            }
            return "<nullptr>";
        }
    }
}

namespace std
{
    std::string inspect(const std::string& s)
    {
        return "\"" + s + "\"";
    }

    std::string inspectEmptiness(const std::string& s)
    {
        if (s.empty())
        {
            return "<empty>";
        }
        return "<value present>";
    }
}
