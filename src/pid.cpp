// implements the PID controller for the lift

#include "main.hpp"

// runs PID code for a single module
static void update(pid::Module& module);

void pid::controller(pid::Module* modules)
{
    unsigned long now = millis();
    while (true)
    {
        // update each PID module sequentially
        for (size_t i = 0; i < PID_MODULE_COUNT; ++i)
        {
            update(modules[i]);
        }
        // wait a bit before looping again
        taskDelayUntil(&now, MOTOR_POLL_RATE);
    }
}

void update(pid::Module& module)
{
    // calculate the error
    double targetPos = module.getTargetPos();
    module.lastError = module.error;
    module.error = targetPos - module.getPos();
    // update the integral value (sum of all the errors)
    module.integral += module.error;
    if (abs(module.error) > module.maxError)
    {
        // error is outside the range for integral to actually be useful
        module.integral = 0;
    }
    // calculate the derivative (rate of change of the error)
    double derivative = module.error - module.lastError;
    // calculate the output motor power
    int drive = (module.kp * module.error) + (module.ki * module.integral) +
        (module.kd * derivative);
    // print debug info
    if (module.debug)
    {
        printf("targetPos=%.1f, currentPos=%.1f, drive=%d, p=%.1f, i=%.1f, "
            "d=%.1f\n", targetPos, module.getPos(), drive,
            module.kp * module.error, module.ki * module.integral,
            module.kd * derivative);
    }
    // update the motors
    module.setMotors(drive);
}
