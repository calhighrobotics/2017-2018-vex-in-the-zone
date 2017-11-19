// defines the functions that control various parts of the robot

#include "main.h"

#define LIFT_LOCK (-95)
#define LIFT_UNLOCK 0

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

void setLift(int speed)
{
    motorSet(LIFT_1, speed);
    motorSet(LIFT_2, speed);
}

void setLiftLock(bool locked)
{
    motorSet(LIFT_SERVO, locked ? LIFT_LOCK : LIFT_UNLOCK);
}

void setClaw(int speed)
{
    motorSet(CLAW, speed);
}

void setMobileGoalLift(int speed)
{
    motorSet(MGL_LEFT, speed);
    motorSet(MGL_RIGHT, -speed);
}
