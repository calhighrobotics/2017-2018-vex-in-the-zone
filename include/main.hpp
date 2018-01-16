// declares the functions, constants, etc. that should exist for all cpp files
//  to use or at least know about

#ifndef MAIN_HPP
#define MAIN_HPP

#include <API.h>

#define TEAM_NAME "1516B"
#define ROBOT_NAME "Robart"

enum Direction
{
    STOP = 0,
    UP = 1,
    CLOSE = 1,
    DOWN = -1,
    OPEN = -1
};

enum AutonID
{
    AUTONID_MIN,
    FORWARD_BACKWARD = AUTONID_MIN,
    SCORE_MG_WITH_CONE,
    SCORE_STATIONARY,
    AUTONID_MAX = SCORE_STATIONARY
};

// determines what autonomous program to run
extern AutonID autonid;

// starts up the LCD to do cool stuff, put on its own task by init.cpp
void lcdMain(void*);

// + forward, - backward
void setLeftDriveTrain(int speed);
void setRightDriveTrain(int speed);
// + up, - down
void setLift(Direction direction);
// + closes, - opens
void setClaw(Direction direction);
// + up, - down
void setMobileGoalLift(Direction direction);
// true locks, false unlocks
void setLiftLock(bool locked);

// these last 4 functions down here are what PROS uses internally to do cool
//  stuff so it's not recommended to call them within the actual code
extern "C"
{
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
} // end extern "C"

#endif // MAIN_HPP
