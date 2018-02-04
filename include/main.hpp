// declares the functions, constants, etc. that should exist for all cpp files
//  to use or at least know about

#ifndef MAIN_HPP
#define MAIN_HPP

#include <API.h>
#include <math.h>

// general info
#define TEAM_NAME "1516B"
#define ROBOT_NAME "Robart"

// time it takes for a motor to be updated in milliseconds
#define MOTOR_POLL_RATE 20

// stuff that has to do with autonomous
namespace auton
{
enum AutonID
{
    AUTONID_MIN,
    NOTHING = AUTONID_MIN,
    FORWARD_BACKWARD,
    MG_CONE_LEFT,
    MG_CONE_RIGHT,
    SCORE_STATIONARY,
    AUTONID_MAX = SCORE_STATIONARY
};

// determines what autonomous program to run
extern AutonID autonid;
} // end namespace auto

namespace lcd
{
// controls the lcd screen
void controller(void*);
} // end namespace lcd

// stuff that has to do with motors
namespace motor
{
// does motor initialization stuff
void init();

// indicates a motor direction
enum Direction
{
    STOP = 0,
    // lift/mgl
    UP = 1,
    DOWN = -1,
    // claw
    CLOSE = 1,
    OPEN = -1,
    // twisty boi
    FORWARD = 1,
    BACKWARD = -1
};

// cone lift functions
// max=127, min=0
double getLiftPos();
double getLiftTarget();
void setLiftTarget(double targetPos);
void setLift(int drive);

// mobile goal lift functions
double getMglPos();
double getMglTarget();
void setMglTarget(double targetPos);
void setMgl(int drive);

double getLeftRotations();
double getRightRotations();
void resetDT();
void setLeftDriveTrain(int speed);
void setRightDriveTrain(int speed);

// other general stuff
void setClaw(Direction direction);
void setTwistyBoi(Direction direction);
void setMobileGoalLift(Direction direction);
} // end namespace motor

namespace sensor
{
// initializes all sensors, should be run in initializeIO
void init();
// checks if the lift is fully down
bool isLiftDown();
} // end namespace sensor

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
