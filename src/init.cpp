// contains initialization of globals and stuff

#include "main.hpp"

// declared in main.hpp
auton::AutonID auton::autonid = auton::SCORE_STATIONARY;

// pre-initialization code, mostly just setting default pin modes and port
//  states and stuff
void initializeIO()
{
}

// initialization code, usually for initializing sensors, LCDs, globals, IMEs,
//  and other stuff
void initialize()
{
    setTeamName(TEAM_NAME);
    // do all the LCD stuff on a separate thread
    taskCreate(init::lcdMain, TASK_DEFAULT_STACK_SIZE, NULL,
        TASK_PRIORITY_DEFAULT - 1);
}
