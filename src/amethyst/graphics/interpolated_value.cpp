#include "interpolated_value.hpp"

namespace amethyst
{
    MAKE_ENUM_CONVERTABLE_DEFINITIONS(endpoint_action,
        { endpoint_action::stop, "STOP" },
        { endpoint_action::continue_forever, "CONTINUE" }
    );
}