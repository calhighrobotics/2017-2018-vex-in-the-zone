// defines the functions that control various parts of the robot

#include "main.hpp"

// ports that are defined for the robot
#define LIFT_SPOOL_LEFT 1
#define DRIVE_BL 2
#define DRIVE_FL 3
#define LIFT_TOWER 4
#define LIFT_LOCK 5
#define CLAW 6
#define MGL 7
#define DRIVE_FR 8
#define DRIVE_BR 9
#define LIFT_SPOOL_RIGHT 10

// settings for various button-controled parts
#define LIFT_UP_SPEED 127
#define LIFT_DOWN_SPEED (-127)
#define LIFT_SPOOL_UP_SPEED 127
#define LIFT_SPOOL_DOWN_SPEED (-24)
#define CLAW_SPEED 63
#define MGL_SPEED 63
#define LIFT_LOCKED (-95)
#define LIFT_UNLOCKED (-75)

// translates the ternary direction (+up/0/-down) to an actual speed
static int speedControl(Direction direction, int up, int down)
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

void setLift(Direction direction)
{
    int spoolSpeed = speedControl(direction, LIFT_SPOOL_UP_SPEED,
        LIFT_SPOOL_DOWN_SPEED);
    int towerSpeed = speedControl(direction, LIFT_UP_SPEED, LIFT_DOWN_SPEED);
    motorSet(LIFT_SPOOL_LEFT, -spoolSpeed);
    motorSet(LIFT_SPOOL_RIGHT, spoolSpeed);
    motorSet(LIFT_TOWER, towerSpeed);
}

void setClaw(Direction direction)
{
    int speed = speedControl(direction, CLAW_SPEED, -CLAW_SPEED);
    motorSet(CLAW, -speed);
}

void setMobileGoalLift(Direction direction)
{
    int speed = speedControl(direction, MGL_SPEED, -MGL_SPEED);
    motorSet(MGL, speed);
}

void setLiftLock(bool locked)
{
    motorSet(LIFT_LOCK, locked ? LIFT_LOCKED : LIFT_UNLOCKED);
}
