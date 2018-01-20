// declares the functions, constants, etc. that should exist for all cpp files
//  to use or at least know about

#ifndef MAIN_HPP
#define MAIN_HPP

#include <API.h>

// general info
#define TEAM_NAME "1516B"
#define ROBOT_NAME "Robart"

// time it takes for a motor to be updated
#define MOTOR_POLL_RATE 20 // ms/update

// used by init.cpp to start essential tasks and initialize stuff
// functions with void* parameter run in their own task
namespace init
{
// initializes IMEs
void initIMEs();
// starts up the LCD to do cool stuff, put on its own task by init.cpp
void lcdMain(void*);
} // end namespace init

// stuff that has to do with autonomous
namespace auton
{
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
} // end namespace auto

// stuff that has to do with motors
namespace motor
{
// indicates a motor direction
enum Direction
{
    STOP = 0,
    UP = 1,
    CLOSE = 1,
    DOWN = -1,
    OPEN = -1
};

// other general stuff
void setLeftDriveTrain(int speed);
void setRightDriveTrain(int speed);
void setLift(Direction direction);
void setClaw(Direction direction);
void setMobileGoalLift(Direction direction);
void setLiftLock(bool locked);
} // end namespace motor

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
