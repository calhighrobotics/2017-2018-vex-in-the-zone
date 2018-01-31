// implements the PID controller for the lift

#include "main.hpp"

// uncomment this line to enable printing lift info
//#define LIFT_DEBUG
// proportional, integral, and derivative constants
#define K_P 0
#define K_I 0
#define K_D 0
// maximum error value for the integral term to be allowed to kick in
#define MAX_ERROR 40

// calculates the difference between target and current position
static double calcError(double targetPos);

void init::liftControl(void*)
{
    double lastError;
    double error = 0;
    double integral = 0;
    unsigned long time = millis();
    while (true)
    {
        // calculate the error
        double targetPos = motor::getLiftTarget();
        lastError = error;
        error = calcError(targetPos);
        // update the integral value (sum of all the errors)
        integral += error;
        if (abs(error) > MAX_ERROR)
        {
            // error is outside the range for integral to actually be useful
            integral = 0;
        }
        // calculate the derivative (rate of change of the error)
        double derivative = error - lastError;
        // calculate the target speed
        double targetSpeed = (K_P * error) + (K_I * integral) +
            (K_D * derivative);
#ifdef LIFT_DEBUG
        // print debugging info
        printf("targetPos=%.1f, currentPos=%.1f, targetSpeed=%.1f, p=%.1f, "
            "i=%.1f, d=%.1f\n", targetPos, motor::getLiftPos(), targetSpeed,
            K_P * error, K_I * integral, K_D * derivative);
#endif
        // update the lift motors
        //motor::setLift(targetSpeed);
        // wait a bit before looping again
        taskDelayUntil(&time, MOTOR_POLL_RATE);
    }
}

double calcError(double targetPos)
{
    return targetPos - motor::getLiftPos();
}
