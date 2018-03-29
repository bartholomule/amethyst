#include "graphics/image_loader.hpp"

int main(int argc, const char** argv)
{
    std::string filename = argv[1];
    try
    {
        auto loader = amethyst::getImageLoader<double>(filename);
        auto img = loader->input(filename);
        loader->output("from_double." + loader->default_extension(), img);
    }
    catch (const std::runtime_error& e)
    {
        std::cout << "Failed 1: " << e.what() << std::endl;
    }


    try
    {
        auto loader = amethyst::getImageLoader<uint8_t>(filename);
        auto img = loader->input(filename);
        loader->output("from_byte." + loader->default_extension(), img);
    }
    catch (const std::runtime_error& e)
    {
        std::cout << "Failed 2: " << e.what() << std::endl;
    }

    return 0;
}