// defines the functions that control various parts of the robot

#include "main.hpp"

// ports that are defined for the robot
#define TWISTY_BOI 1
#define MGL_LEFT 2
#define DRIVE_LEFT 3
#define LIFT_BL 4
#define LIFT_TL 5
#define LIFT_BR 6
#define LIFT_TR 7
#define DRIVE_RIGHT 4
#define MGL_RIGHT 9
#define CLAW 10

// amount of ports on the cortex
#define PORT_COUNT 10

// IME network
#define IME_RIGHT 0
#define IME_LEFT 1
#define IME_MGL 2
#define IME_LIFT 3
#define IME_COUNT 4 // number of IEMs

// settings for various button-controled parts
#define CLAW_SPEED 63
#define TB_SPEED 63

#define MAX_POS 127.0
#define MIN_POS 0.0
#define COUNTS_PER_REV_TORQUE 627.2 // IME counts per rev in high torque mode
#define LIFT_MAX_REVS 6.0
#define MGL_MAX_REVS 3.0
#define SLEW_RATE 10 // limit for how large the difference in motor drive can be

static double liftTarget = 0;
// protects liftTarget from being accessed by two tasks at the same time
static Mutex liftTargetMutex;

static double mglTarget = 0;
static Mutex mglTargetMutex;

// requested motor values
static int requested[PORT_COUNT] = {}; // zero-initialized
static Mutex requestedMutex[PORT_COUNT];

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

// requests a new drive speed to the slew rate manager
static void motorRequest(int port, int drive)
{
    mutexTake(requestedMutex[port], -1);
    requested[port] = drive;
    mutexGive(requestedMutex[port]);
}

// declared in main.hpp

void motor::init()
{
    // create mutexes
    liftTargetMutex = mutexCreate();
    mglTargetMutex = mutexCreate();
    for (int port = 0; port < PORT_COUNT; ++port)
    {
        requestedMutex[port] = mutexCreate();
    }
    // initialize IMEs
    int imeCount = imeInitializeAll();
    if (imeCount != IME_COUNT)
    {
        printf("ERROR: INCORRECT NUMBER OF IMES INITIALIZED (%d, expected %d)\n"
            "EXPECT UNRELIABLE BEHAVIOR\n", imeCount, IME_COUNT);
    }
    else
    {
        imeReset(IME_RIGHT);
        imeReset(IME_LEFT);
        imeReset(IME_MGL);
        imeReset(IME_LIFT);
        //print("IMEs ok!");
    }
}

void motor::slewRateManager(void*)
{
    // closed loop updating the motors
    unsigned long now = millis();
    while (true)
    {
        // go through each port
        for (int port = 0; port < PORT_COUNT; ++port)
        {
            // see if we need to change the motor value
            int drive = motorGet(port);
            mutexTake(requestedMutex[port], -1);
            int req = requested[port];
            mutexGive(requestedMutex[port]);
            if (req != drive)
            {
                if (req > drive)
                {
                    // increasing
                    if (req - drive > SLEW_RATE)
                    {
                        // limit the increase by slew rate
                        drive += SLEW_RATE;
                    }
                    else
                    {
                        drive = req;
                    }
                }
                else if (req < drive)
                {
                    // decreasing
                    if (req - drive < -SLEW_RATE)
                    {
                        // limit the increase by slew rate
                        drive -= SLEW_RATE;
                    }
                    else
                    {
                        drive = req;
                    }
                }
                // finally, set the motor
                //motorSet(port, drive);
            }
        }
        taskDelayUntil(&now, MOTOR_POLL_RATE);
    }
}

double motor::getLiftPos()
{
    int counts;
    imeGet(IME_LIFT, &counts);
    return MAX_POS / (LIFT_MAX_REVS * COUNTS_PER_REV_TORQUE) * -counts;
}

double motor::getLiftTarget()
{
    mutexTake(liftTargetMutex, -1);
    double target = liftTarget;
    mutexGive(liftTargetMutex);
    return target;
}

void motor::setLiftTarget(double targetPos)
{
    // max=127, min=0
    if (targetPos > MAX_POS)
    {
        targetPos = MAX_POS;
    }
    else if (targetPos < MIN_POS)
    {
        targetPos = MIN_POS;
    }
    mutexTake(liftTargetMutex, -1);
    liftTarget = targetPos;
    mutexGive(liftTargetMutex);
}

void motor::setLift(int drive)
{
    // don't go any lower if the lift is already down
    if (drive < 0 && sensor::isLiftDown())
    {
        drive = 0;
    }
    if (drive > 0 && getLiftPos() >= MAX_POS)
    {
        drive = 0;
    }
    motorSet(LIFT_BL, -drive);
    motorSet(LIFT_TL, -drive);
    motorSet(LIFT_BR, drive);
    motorSet(LIFT_TR, drive);
}

double motor::getMglPos()
{
    int counts;
    imeGet(IME_MGL, &counts);
    return MAX_POS / (MGL_MAX_REVS * COUNTS_PER_REV_TORQUE) * -counts;
}

double motor::getMglTarget()
{
    mutexTake(mglTargetMutex, -1);
    double target = mglTarget;
    mutexGive(mglTargetMutex);
    return target;
}

void motor::setMglTarget(double targetPos)
{
    // max=127, min=0
    if (targetPos > MAX_POS)
    {
        targetPos = MAX_POS;
    }
    else if (targetPos < MIN_POS)
    {
        targetPos = MIN_POS;
    }
    mutexTake(mglTargetMutex, -1);
    mglTarget = targetPos;
    mutexGive(mglTargetMutex);
}

void motor::setMgl(int drive)
{
    motorSet(MGL_LEFT, drive);
    motorSet(MGL_RIGHT, -drive);
}

void motor::setLeftDriveTrain(int speed)
{
    motorSet(DRIVE_LEFT, speed);
}

void motor::setRightDriveTrain(int speed)
{
    motorSet(DRIVE_RIGHT, -speed);
}

void motor::setLift(Direction direction)
{
    /*int spoolSpeed = speedControl(direction, LIFT_SPOOL_UP_SPEED,
        LIFT_SPOOL_DOWN_SPEED);
    int towerSpeed = speedControl(direction, LIFT_UP_SPEED, LIFT_DOWN_SPEED);
    motorSet(LIFT_SPOOL_LEFT, -spoolSpeed);
    motorSet(LIFT_SPOOL_RIGHT, spoolSpeed);
    motorSet(LIFT_TOWER, towerSpeed);*/
}

void motor::setClaw(Direction direction)
{
    int speed = speedControl(direction, CLAW_SPEED, -CLAW_SPEED);
    motorSet(CLAW, speed);
}

void motor::setTwistyBoi(Direction direction)
{
    int speed = speedControl(direction, TB_SPEED, -TB_SPEED);
    motorSet(TWISTY_BOI, speed);
}

void motor::setMobileGoalLift(Direction direction)
{
    /*int speed = speedControl(direction, MGL_SPEED, -MGL_SPEED);
    motorSet(MGL, speed);*/
}
