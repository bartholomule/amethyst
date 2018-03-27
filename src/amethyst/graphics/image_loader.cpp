#include "image_loader.hpp"

namespace amethyst
{
    namespace impl
    {
        bool endsWith(const std::string& text, const std::string& search)
        {
            if (search.size() > text.size())
            {
                return false;
            }
            auto loc = text.rfind(search);
            if (loc == std::string::npos)
            {
                return false;
            }
            return (loc + search.size()) == text.size();
        }
    }

    image<double> loadImageD(const std::string& filename)
    {
        auto loader = getImageLoader<double>(filename);
        if (loader)
        {
            return loader->input(filename);
        }
        return {};
    }

    image<uint8_t> loadImageI(const std::string& filename)
    {
        auto loader = getImageLoader<uint8_t>(filename);
        if (loader)
        {
            return loader->input(filename);
        }
        return {};
    }
}

