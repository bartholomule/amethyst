#include "capabilities.hpp"

namespace amethyst
{
    namespace // anonymous
    {
        void add_to_string_list(std::string& s, const std::string& item)
        {
            if (!s.empty())
            {
                s += ", " + item;
            }
            else
            {
                s += item;
            }
        }

#define ADD_TO_RESULT_LONG(flag, name) \
    do \
    { \
        if (*this & (flag)) \
        { \
            add_to_string_list(retval, name); \
        } \
    } while (0)
#define ADD_TO_RESULT_SHORT(flag, character) \
    do \
    { \
        if (*this & (flag)) \
        { \
            retval += character; \
        } \
    } while (0)

#define CAPABILITIES_DEFINITION(cap_name) \
    cap_name & cap_name::operator|=(const cap_name::capabilities c) \
    { \
        my_capabilities = (cap_name::capabilities)(unsigned(c) | unsigned(my_capabilities)); \
        return *this; \
    } \
 \
    cap_name& cap_name::operator|=(const cap_name& c) \
    { \
        my_capabilities = (cap_name::capabilities)(unsigned(c.my_capabilities) | unsigned(my_capabilities)); \
        return *this; \
    } \
 \
    cap_name& cap_name::operator&=(const cap_name::capabilities c) \
    { \
        my_capabilities = (cap_name::capabilities)(unsigned(c) & unsigned(my_capabilities)); \
        return *this; \
    } \
    cap_name& cap_name::operator&=(const cap_name& c) \
    { \
        my_capabilities = (cap_name::capabilities)(unsigned(c.my_capabilities) & unsigned(my_capabilities)); \
        return *this; \
    } \
 \
    bool operator&(const cap_name& c1, cap_name::capabilities c2) \
    { \
        return unsigned(c1.my_capabilities) & unsigned(c2); \
    } \
    bool operator&(cap_name::capabilities c1, const cap_name& c2) \
    { \
        return unsigned(c1) & unsigned(c2.my_capabilities); \
    } \
    cap_name operator&(const cap_name& c1, const cap_name& c2) \
    { \
        return cap_name(cap_name::capabilities(unsigned(c1.my_capabilities) & unsigned(c2.my_capabilities))); \
    } \
    cap_name operator|(const cap_name& c1, const cap_name& c2) \
    { \
        return cap_name(cap_name::capabilities(unsigned(c1.my_capabilities) | unsigned(c2.my_capabilities))); \
    } \
    cap_name operator~(const cap_name::capabilities& c) \
    { \
        return cap_name(cap_name::capabilities(cap_name::ALL& ~unsigned(c))); \
    } \
 \
    cap_name operator~(const cap_name& c) \
    { \
        return cap_name(cap_name::capabilities(cap_name::ALL& ~unsigned(c.my_capabilities))); \
    } \
 \
    bool operator==(const cap_name& i1, const cap_name& i2) \
    { \
        return unsigned(i1.my_capabilities) == unsigned(i2.my_capabilities); \
    } \
    bool operator!=(const cap_name& i1, const cap_name& i2) \
    { \
        return unsigned(i1.my_capabilities) != unsigned(i2.my_capabilities); \
    }

    } // end anonymous namespace

    std::string intersection_capabilities::to_string() const
    {
        std::string retval;

        ADD_TO_RESULT_LONG(intersection_capabilities::HIT_FIRST, "First Object Hit");
        ADD_TO_RESULT_LONG(intersection_capabilities::HIT_ALL, "All Objects Hit");
        ADD_TO_RESULT_LONG(intersection_capabilities::HIT_CONTAINERS, "Containers Hit");
        ADD_TO_RESULT_LONG(intersection_capabilities::NORMAL_CALCULATION, "Normal Calculation");
        ADD_TO_RESULT_LONG(intersection_capabilities::UV_CALCULATION, "UV Calculation");
        ADD_TO_RESULT_LONG(intersection_capabilities::LOCAL_SYSTEM_CALCULATION, "Local Coord System Calculation");
        ADD_TO_RESULT_LONG(intersection_capabilities::TIME_SAMPLING, "Time Sampling");

        if (retval.empty())
        {
            retval = "NONE";
        }

        return "{" + retval + "}";
    }


    std::string intersection_capabilities::to_string_short() const
    {
        std::string retval;

        ADD_TO_RESULT_SHORT(intersection_capabilities::HIT_FIRST, "F");
        ADD_TO_RESULT_SHORT(intersection_capabilities::HIT_ALL, "A");
        ADD_TO_RESULT_SHORT(intersection_capabilities::HIT_CONTAINERS, "C");
        ADD_TO_RESULT_SHORT(intersection_capabilities::NORMAL_CALCULATION, "N");
        ADD_TO_RESULT_SHORT(intersection_capabilities::UV_CALCULATION, "U");
        ADD_TO_RESULT_SHORT(intersection_capabilities::LOCAL_SYSTEM_CALCULATION, "L");
        ADD_TO_RESULT_SHORT(intersection_capabilities::TIME_SAMPLING, "T");

        return retval;
    }

    CAPABILITIES_DEFINITION(intersection_capabilities);

    std::string object_capabilities::to_string() const
    {
        std::string retval;

        ADD_TO_RESULT_LONG(object_capabilities::NOT_FINITE, "Infinite");
        ADD_TO_RESULT_LONG(object_capabilities::BOUNDABLE, "Boundable");
        ADD_TO_RESULT_LONG(object_capabilities::MOVABLE, "Movable");
        ADD_TO_RESULT_LONG(object_capabilities::POLYGONIZATION, "Polygonization");
        ADD_TO_RESULT_LONG(object_capabilities::CONTAINER, "Container");
        ADD_TO_RESULT_LONG(object_capabilities::IMPLICIT, "Implicit Surface");

        if (retval.empty())
        {
            retval = "NONE";
        }

        return "{" + retval + "}";
    }


    std::string object_capabilities::to_string_short() const
    {
        std::string retval;

        ADD_TO_RESULT_SHORT(object_capabilities::NOT_FINITE, "I");
        ADD_TO_RESULT_SHORT(object_capabilities::BOUNDABLE, "B");
        ADD_TO_RESULT_SHORT(object_capabilities::MOVABLE, "M");
        ADD_TO_RESULT_SHORT(object_capabilities::POLYGONIZATION, "P");
        ADD_TO_RESULT_SHORT(object_capabilities::CONTAINER, "C");
        ADD_TO_RESULT_SHORT(object_capabilities::IMPLICIT, "F");
        return retval;
    }

    CAPABILITIES_DEFINITION(object_capabilities);

    std::string material_capabilities::to_string() const
    {
        std::string retval;

        ADD_TO_RESULT_LONG(material_capabilities::DIFFUSE, "Diffuse");
        ADD_TO_RESULT_LONG(material_capabilities::EMISSIVE, "Emissive");
        ADD_TO_RESULT_LONG(material_capabilities::REFLECTIVE, "Reflective");
        ADD_TO_RESULT_LONG(material_capabilities::TRANSMISSIVE, "Transmissive");

        if (retval.empty())
        {
            retval = "NONE";
        }

        return "{" + retval + "}";
    }


    std::string material_capabilities::to_string_short() const
    {
        std::string retval;

        ADD_TO_RESULT_SHORT(material_capabilities::DIFFUSE, "D");
        ADD_TO_RESULT_SHORT(material_capabilities::EMISSIVE, "E");
        ADD_TO_RESULT_SHORT(material_capabilities::REFLECTIVE, "R");
        ADD_TO_RESULT_SHORT(material_capabilities::TRANSMISSIVE, "T");

        return retval;
    }

    CAPABILITIES_DEFINITION(material_capabilities);
}
