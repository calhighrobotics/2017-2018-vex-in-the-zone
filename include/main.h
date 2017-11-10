// declares the functions, constants, etc. that should exist for all c files to
//  use or at least know about

#ifndef MAIN_H
#define MAIN_H

#include <API.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ROBOT_NAME "Bart Bart"

// ports that are defined for the robot
#define LIFT_1 1
#define DRIVE_BL 2
#define DRIVE_FL 3
#define DRIVE_FR 4
#define DRIVE_BR 5
#define CLAW 6
#define MGL_LEFT 7
#define MGL_RIGHT 8
#define SAD_LONELY_PORT 9 // :(
#define LIFT_2 10

// + forward, - backward
void setLeftDriveTrain(int speed);
void setRightDriveTrain(int speed);
// + up, - down
void setLift(int speed);
// + closes, - opens
void setClaw(int speed);
// + up, - down
void setMobileGoalLift(int speed);

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
