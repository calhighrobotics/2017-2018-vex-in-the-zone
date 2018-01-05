// declares the functions, constants, etc. that should exist for all c files to
//  use or at least know about

#ifndef MAIN_H
#define MAIN_H

#include <API.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define ROBOT_NAME "Robart"

typedef enum Direction
{
    STOP = 0,
    UP = 1,
    CLOSE = 1,
    DOWN = -1,
    OPEN = -1
}
direction_t;

typedef enum AutonID
{
    AUTONID_MIN,
    FORWARD_BACKWARD = AUTONID_MIN,
    SCORE_MG_WITH_CONE,
    SCORE_STATIONARY,
    AUTONID_MAX = SCORE_STATIONARY,
    AUTONID_COUNT
}
autonid_t;

// determines what autonomous program to run
extern autonid_t autonid;

// starts up the LCD to do cool stuff on its own task
void lcdMain();

// + forward, - backward
void setLeftDriveTrain(int speed);
void setRightDriveTrain(int speed);
// + up, - down
void setLift(direction_t direction);
// + closes, + opens
void setClaw(direction_t direction);
// + up, - down
void setMobileGoalLift(direction_t direction);
// true locks, false unlocks
void setLiftLock(bool locked);

// these last 4 functions down here are what PROS uses internally to do cool
//  stuff so it's not recommended to call them within the actual code

// main point of execution for the autonomous period
void autonomous();

// pre-initialization code, mostly just setting default pin modes and port
//  states and stuff
void initializeIO();

// initialization code, usually for initializing sensors, LCDs, globals, IMEs,
//  and other stuff
void initialize();

// main point of execution for the driver control period
void operatorControl();

#ifdef __cplusplus
}
#endif
#endif // MAIN_H
