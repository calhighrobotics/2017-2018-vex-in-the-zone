// contains code for the driver control period and anything else associated with
//  it

#include "main.hpp"

// uncomment this line to enable arcade controls
#define TANK_CONTROLS
// uncomment this line to enable autonomous button
#define AUTON_DEBUG
// used in the threshold function to prevent joystick ghosting
#define THRESHOLD 4

// these functions get and respond to driver input for every different part
// operatorControl() should be calling these functions in the order below
static void controlDriveTrain();
static void controlLift();
static void controlClaw();
static void controlTwistyBoi();
static void controlMobileGoalLift();
#ifdef AUTON_DEBUG
static void controlAutonomous();
#endif // AUTON_DEBUG

// returns what was given if the value exceeds the threshold, else 0 to prevent
//  joystick ghosting
static int threshold(int value);

// merges two binary directions (up/down) into a ternary direction (+up/0/-down)
static motor::Direction direction(bool up, bool down);

// main point of execution for the driver control period
void operatorControl()
{
    // keeps track of the current time since the last opcontrol loop
    unsigned long time = millis();
    // goes into an infinite loop, constantly receiving and responding to input
    while (1)
    {
        controlDriveTrain();
        controlLift();
        controlClaw();
        controlTwistyBoi();
        controlMobileGoalLift();
#ifdef AUTON_DEBUG
        controlAutonomous();
#endif
        // wait a bit before receiving input again
        taskDelayUntil(&time, MOTOR_POLL_RATE);
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
    motor::setLeftDriveTrain(left);
    motor::setRightDriveTrain(right);
}

void controlLift()
{
    bool liftUp = joystickGetDigital(1, 6, JOY_UP);
    bool liftDown = joystickGetDigital(1, 6, JOY_DOWN);
    if (liftUp && !liftDown)
    {
        motor::setLift(127);
    }
    else if (!liftUp && liftDown)
    {
        motor::setLift(-127);
    }
    else
    {
        motor::setLift(0);
    }
}

void controlClaw()
{
    bool clawClose = joystickGetDigital(1, 5, JOY_UP);
    bool clawOpen = joystickGetDigital(1, 5, JOY_DOWN);
    motor::setClaw(direction(clawClose, clawOpen));
}

void controlTwistyBoi()
{
    bool forward = joystickGetDigital(1, 8, JOY_LEFT);
    bool backward = joystickGetDigital(1, 8, JOY_RIGHT);
    motor::setTwistyBoi(direction(forward, backward));
}

void controlMobileGoalLift()
{
    bool mglUp = joystickGetDigital(1, 8, JOY_UP);
    bool mglDown = joystickGetDigital(1, 8, JOY_DOWN);
    motor::setMobileGoalLift(direction(mglUp, mglDown));
}

#ifdef AUTON_DEBUG
void controlAutonomous()
{
    if (joystickGetDigital(1, 7, JOY_LEFT))
    {
        autonomous();
    }
}
#endif // AUTON_DEBUG

int threshold(int value)
{
    return abs(value) > THRESHOLD ? value : 0;
}

motor::Direction direction(bool up, bool down)
{
    return (motor::Direction) (up - down);
}
