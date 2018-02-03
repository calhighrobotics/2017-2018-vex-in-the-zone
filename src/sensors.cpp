#include "main.hpp"

// digital ports
#define LIFT_LIMIT 2

bool sensor::isLiftDown()
{
    return digitalRead(LIFT_LIMIT);
}
