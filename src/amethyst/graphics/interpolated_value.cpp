#include "interpolated_value.hpp"

namespace amethyst
{
    MAKE_ENUM_CONVERTABLE_DEFINITIONS(endpoint_action, "invalid endpoint action: %1",
        { endpoint_action::stop, "STOP" },
        { endpoint_action::continue_forever, "CONTINUE" }
    );
}