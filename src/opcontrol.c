// contains code for the driver control period and anything else associated with
//  it

#include "main.h"

// uncomment this line to enable arcade controls
#define TANK_CONTROLS
// used in the joystick function to prevent ghosting
#define THRESHOLD 4
// speed settings for various button-controled parts
#define LIFT_SPEED 127
#define CLAW_SPEED 63
#define MGL_SPEED 63

// returns what was given if the value exceeds the threshold, else 0
// prevents joystick ghosting
int threshold(int value)
{
    return abs(value) > THRESHOLD ? value : 0;
}

// sets the motor at the given channel to the given speed based on whether you
//  want to go forwards or backwards
void buttonControl(unsigned char channel, int speed, bool forwards,
    bool backwards)
{
    if (forwards && !backwards)
    {
        motorSet(channel, speed);
    }
    else if (!forwards && backwards)
    {
        motorSet(channel, -speed);
    }
    else
    {
        motorSet(channel, 0);
    }
}

// main point of execution for the driver control period
void operatorControl()
{
    // goes into an infinite loop, constantly receiving and responding to input
    while (1)
    {
        // gather joystick input
#ifdef TANK_CONTROLS
        // tank controls
        int left = threshold(joystickGetAnalog(1, 3));
        int right = threshold(joystickGetAnalog(1, 2));
#else
        // arcade controls
        int left = threshold(joystickGetAnalog(1, 3)) +
            threshold(joystickGetAnalog(1, 1));
        int right = threshold(joystickGetAnalog(1, 3)) -
            threshold(joystickGetAnalog(1, 1));
#endif
        // set the drive train motors accordingly
        motorSet(DRIVE_FL, -left);
        motorSet(DRIVE_BL, -left);
        motorSet(DRIVE_FR, right);
        motorSet(DRIVE_BR, right);
        // gather button input
        bool liftUp = joystickGetDigital(1, 6, JOY_UP);
        bool liftDown = joystickGetDigital(1, 6, JOY_DOWN);
        bool clawOpen = joystickGetDigital(1, 5, JOY_UP);
        bool clawClose = joystickGetDigital(1, 5, JOY_DOWN);
        bool mglUp = joystickGetDigital(1, 8, JOY_UP);
        bool mglDown = joystickGetDigital(1, 8, JOY_DOWN);
        // set all the other devices accordingly
        buttonControl(LIFT_1, LIFT_SPEED, liftUp, liftDown);
        buttonControl(LIFT_2, -LIFT_SPEED, liftUp, liftDown);
        buttonControl(CLAW, CLAW_SPEED, clawOpen, clawClose);
        buttonControl(MGL_LEFT, MGL_SPEED, mglUp, mglDown);
        buttonControl(MGL_RIGHT, -MGL_SPEED, mglUp, mglDown);
        // wait a bit before receiving input again
        delay(20);
    }
}
