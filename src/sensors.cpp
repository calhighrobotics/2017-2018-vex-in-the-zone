#include "main.hpp"

// digital ports
#define LIFT_LIMIT 2

void sensor::init()
{
    pinMode(LIFT_LIMIT, INPUT);
}

bool sensor::isLiftDown()
{
    return digitalRead(LIFT_LIMIT) == LOW;
}
