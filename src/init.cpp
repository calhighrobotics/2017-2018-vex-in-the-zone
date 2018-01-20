// contains initialization of globals and stuff

#include "main.hpp"

// declared in main.hpp
auton::AutonID auton::autonid = SCORE_STATIONARY;

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
    init::initIMEs();
    taskCreate(init::lcdMain, TASK_DEFAULT_STACK_SIZE, NULL,
        TASK_PRIORITY_DEFAULT - 1);
}
