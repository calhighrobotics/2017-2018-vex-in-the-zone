// contains code for the driver control period and anything else associated with
//  it

#include "main.h"

// uncomment this line to enable arcade controls
#define TANK_CONTROLS
// used in the threshold function to prevent joystick ghosting
#define THRESHOLD 4
// speed settings for various button-controled parts
#define LIFT_SPEED 127
#define CLAW_SPEED 63
#define MGL_SPEED 63
// the time in milliseconds between polls for driver input
#define POLL_SPEED 20ul

// these functions get and respond to driver input for every different part
// operatorControl() should be calling these functions in the order below
static void controlDriveTrain();
static void controlClaw();
static void controlLift();
static void controlMobileGoalLift();

// returns what was given if the value exceeds the threshold, else 0 to prevent
//  joystick ghosting
static int threshold(int value);

// returns given speed based on whether you want to go forwards or backwards
static int speedControl(int speed, bool forwards, bool backwards);

// main point of execution for the driver control period
void operatorControl()
{
    // keeps track of the current time since the last opcontrol loop
    unsigned long time = millis();
    // goes into an infinite loop, constantly receiving and responding to input
    while (1)
    {
        controlDriveTrain();
        controlClaw();
        controlLift();
        controlMobileGoalLift();
        // wait a bit before receiving input again
        taskDelayUntil(&time, POLL_SPEED);
    }
}

void controlDriveTrain()
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
    setLeftDriveTrain(left);
    setRightDriveTrain(right);
}

void controlClaw()
{
    bool clawOpen = joystickGetDigital(1, 5, JOY_UP);
    bool clawClose = joystickGetDigital(1, 5, JOY_DOWN);
    setClaw(speedControl(CLAW_SPEED, clawOpen, clawClose));
}

void controlClaw()
{
    // for the claw, pressing once will open it, pressing once again will
    //  close it continuously to lock onto the cone, and so on
    lastClawPressed = clawPressed;
    clawPressed = joystickGetDigital(1, 8, JOY_RIGHT);
    if (clawPressed && !lastClawPressed)
    {
        // open or close the claw based on the claw's current state
        if (clawOpen)
        {
            // close claw
            setClaw(CLAW_SPEED);
            setClawTimer(CLAW_CLOSE_TIME);
        }
        else
        {
            // open claw
            setClaw(-CLAW_SPEED);
            setClawTimer(CLAW_OPEN_TIME);
        }
        // switch the claw's state between open and closed
        clawOpen = !clawOpen;
    }
    // timer that keeps track of how long the claw should be opened
    // once that timer (clawOpenTime) hits 0, the claw is stopped
    if (clawTimer <= 0)
    {
        setClaw(0);
    }
    else
    {
        clawTimer -= POLL_SPEED;
    }
}

void controlMobileGoalLift()
{
    bool mglUp = joystickGetDigital(1, 8, JOY_UP);
    bool mglDown = joystickGetDigital(1, 8, JOY_DOWN);
    setMobileGoalLift(speedControl(MGL_SPEED, mglUp, mglDown));
}

int threshold(int value)
{
    return abs(value) > THRESHOLD ? value : 0;
}

int speedControl(int speed, bool forwards, bool backwards)
{
    if (forwards && !backwards)
    {
        return speed;
    }
    if (!forwards && backwards)
    {
        return -speed;
    }
    return 0;
}
