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

// amount of PID modules that exist
#define PID_MODULE_COUNT 2

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
// task that acts as a slew rate manager
void slewRateManager(void*);

// indicates a motor direction
enum Direction
{
    STOP = 0,
    // lift/mgl
    UP = 1,
    DOWN = -1,
    // claw
    OPEN = 1,
    CLOSE = -1,
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

// other general stuff
void setLeftDriveTrain(int speed);
void setRightDriveTrain(int speed);
void setLift(Direction direction);
void setClaw(Direction direction);
void setTwistyBoi(Direction direction);
void setMobileGoalLift(Direction direction);
} // end namespace motor

namespace pid
{
// PID settings for a single module
struct Module
{
    // function for setting motors
    typedef void (*motorfunc_t)(int);
    // function for getting a position
    typedef double (*posfunc_t)();
    // constructor
    Module(motorfunc_t setMotors, posfunc_t getTargetPos, posfunc_t getPos,
        double kp, double ki, double kd, double maxError, bool debug = false)
        : setMotors(setMotors), getTargetPos(getTargetPos), getPos(getPos),
        kp(kp), ki(ki), kd(kd), maxError(maxError), lastError(0), error(0),
        integral(0), debug(debug) {}
    // sets the motors
    const motorfunc_t setMotors;
    // gets the target position
    const posfunc_t getTargetPos;
    // gets the current position
    const posfunc_t getPos;
    // proportional, integral, and derivative constants
    const double kp;
    const double ki;
    const double kd;
    // absolute value of error should be lower than this for the integral term
    //  to be allowed to kick in
    const double maxError;
    // runtime variables
    double lastError;
    double error;
    double integral;
    // true if debug info should be printed
    const bool debug;
};

// uses a PID loop to control a PID module
void controller(pid::Module* modules);
} // end namespace pid

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
