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

// IME network
#define IME_LEFT 0
#define IME_RIGHT 1
#define IME_LIFT 2
#define IME_MGL 3
#define IME_COUNT 4 // number of IEMs

// settings for various button-controled parts
#define LIFT_UP_SPEED 127
#define LIFT_DOWN_SPEED (-127)
#define LIFT_SPOOL_UP_SPEED 127
#define LIFT_SPOOL_DOWN_SPEED (-24)
#define CLAW_SPEED 63
#define MGL_SPEED 63
#define LIFT_LOCKED (-95)
#define LIFT_UNLOCKED (-75)

// converts a Direction to an actual speed
static int speedControl(motor::Direction direction, int up, int down)
{
    if (direction == motor::UP)
    {
        return up;
    }
    if (direction == motor::DOWN)
    {
        return down;
    }
    return 0;
}

// declared in main.hpp

void init::initIMEs()
{
    int IMECount = imeInitializeAll();
    if (IMECount != IME_COUNT)
    {
        print("ERROR: INCORRECT NUMBER OF IMES INITIALIZED\n"
            "EXPECT UNRELIABLE BEHAVIOR\n");
    }
    imeReset(IME_LEFT);
    imeReset(IME_RIGHT);
    imeReset(IME_LIFT);
    imeReset(IME_MGL);
}

void motor::setLeftDriveTrain(int speed)
{
    motorSet(DRIVE_FL, speed);
    motorSet(DRIVE_BL, speed);
}

void motor::setRightDriveTrain(int speed)
{
    motorSet(DRIVE_FR, -speed);
    motorSet(DRIVE_BR, -speed);
}

void motor::setLift(Direction direction)
{
    int spoolSpeed = speedControl(direction, LIFT_SPOOL_UP_SPEED,
        LIFT_SPOOL_DOWN_SPEED);
    int towerSpeed = speedControl(direction, LIFT_UP_SPEED, LIFT_DOWN_SPEED);
    motorSet(LIFT_SPOOL_LEFT, -spoolSpeed);
    motorSet(LIFT_SPOOL_RIGHT, spoolSpeed);
    motorSet(LIFT_TOWER, towerSpeed);
}

void motor::setClaw(Direction direction)
{
    int speed = speedControl(direction, CLAW_SPEED, -CLAW_SPEED);
    motorSet(CLAW, -speed);
}

void motor::setMobileGoalLift(Direction direction)
{
    int speed = speedControl(direction, MGL_SPEED, -MGL_SPEED);
    motorSet(MGL, speed);
}

void motor::setLiftLock(bool locked)
{
    motorSet(LIFT_LOCK, locked ? LIFT_LOCKED : LIFT_UNLOCKED);
}
