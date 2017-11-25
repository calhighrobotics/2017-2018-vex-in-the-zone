// defines the functions that control various parts of the robot

#include "main.h"

// settings for various button-controled parts
#define LIFT_UP_SPEED 127
#define LIFT_DOWN_SPEED (-LIFT_UP_SPEED * 3 / 4)
#define CLAW_SPEED 63
#define MGL_SPEED 63
#define LIFT_LOCK (-95)
#define LIFT_UNLOCK (-75)

// translates the ternary direction (+up/0/-down) to an actual speed
static inline int speedControl(int direction, int up, int down)
{
    return (direction > 0) ? up : (direction < 0) ? down : 0;
}

void setLeftDriveTrain(int speed)
{
    motorSet(DRIVE_FL, speed);
    motorSet(DRIVE_BL, speed);
}

void setRightDriveTrain(int speed)
{
    motorSet(DRIVE_FR, -speed);
    motorSet(DRIVE_BR, -speed);
}

void setLift(int direction)
{
    int speed = speedControl(direction, LIFT_UP_SPEED, LIFT_DOWN_SPEED);
    motorSet(LIFT_1, speed);
    motorSet(LIFT_2, speed);
}

void setClaw(int direction)
{
    int speed = speedControl(direction, CLAW_SPEED, -CLAW_SPEED);
    motorSet(CLAW, speed);
}

void setMobileGoalLift(int direction)
{
    int speed = speedControl(direction, MGL_SPEED, -MGL_SPEED);
    motorSet(MGL_LEFT, speed);
    motorSet(MGL_RIGHT, -speed);
}

void setLiftLock(bool locked)
{
    motorSet(LIFT_SERVO, locked ? LIFT_LOCK : LIFT_UNLOCK);
}
