// defines the functions that control various parts of the robot

#include "main.h"

#define LIFT_SERVO_LOCK (-95)

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
    motorSet(LIFT_SERVO, speed == 0 ? LIFT_SERVO_LOCK : 0);
    motorSet(LIFT_1, -speed);
    motorSet(LIFT_2, speed);
}

void setClaw(int speed)
{
    motorSet(CLAW, -speed);
}

void setMobileGoalLift(int speed)
{
    motorSet(MGL_LEFT, speed);
    motorSet(MGL_RIGHT, -speed);
}
