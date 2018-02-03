// contains initialization of globals and stuff

#include "main.hpp"

// declared in main.hpp
auton::AutonID auton::autonid = SCORE_STATIONARY;

// PID data for lift and mgl
static pid::Module pidModules[PID_MODULE_COUNT] =
{
    pid::Module(motor::setLift, motor::getLiftTarget, motor::getLiftPos,
        /*pid*/ 0, 0, 0, /*maxError=*/20),
    pid::Module(motor::setMgl, motor::getMglTarget, motor::getMglPos,
        /*pid*/ 0, 0, 0, /*maxError=*/20)
};

// pre-initialization code, mostly just setting default pin modes and port
//  states and stuff
void initializeIO()
{
    // automatically resets the cortex if bad stuff happens, e.g. a static shock
    watchdogInit();
}

// initialization code, usually for initializing sensors, LCDs, globals, IMEs,
//  and other stuff
void initialize()
{
    setTeamName(TEAM_NAME);
    motor::init();
    taskCreate(init::lcdMain, TASK_DEFAULT_STACK_SIZE, NULL,
        TASK_PRIORITY_DEFAULT - 1);
    taskCreate(reinterpret_cast<TaskCode>(pid::controller),
        TASK_DEFAULT_STACK_SIZE, static_cast<void*>(pidModules),
        TASK_PRIORITY_DEFAULT - 1);
    taskCreate(reinterpret_cast<TaskCode>(motor::slewRateManager),
        TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT + 1);
}
