// contains code for the autonomous period and anything else associated with it

#include "main.h"

// angles are in degrees, distances are in 1/16 inches
#define WHEEL_RADIUS 32ul
#define BOT_RADIUS 120ul
// not in parentheses to take advantage of only doing integer arithmetic
#define MOTOR_SPEED 5ul/3ul // rot/s
#define PI 22ul/7ul // overestimated (22/7>PI) because ints always round down
#define CLAW_TIME 150ul // ms

// autonomous plans
static void forwardBackward();
static void scoreMgWithCone();
static void scoreStationary();

// dt functions require a stop() at the end to allow chaining
static void turnCW(unsigned int angle, int turnRadius, int leftPower);
static void turnCCW(unsigned int angle, int turnRadius, int rightPower);
static void straight(unsigned long distance, int power);
static void stop();
// other stuff
static void lift(direction_t direction, unsigned long waitTime);
static void claw(direction_t direction);
static void mgl(direction_t direction, unsigned long waitTime);

// main point of execution for the autonomous period
void autonomous()
{
    switch (autonid)
    {
    case FORWARD_BACKWARD:
        forwardBackward();
        break;
    case SCORE_MG_WITH_CONE:
        scoreMgWithCone();
        break;
    case SCORE_STATIONARY:
        scoreStationary();
        break;
    default:
        ; // should never happen
    }
}

void forwardBackward()
{
    straight(100ul, 95);
    straight(100ul, -95);
    stop();
}

void scoreMgWithCone()
{
    // pick up the cone
    claw(CLOSE);
    lift(UP, 1200ul);
    // drive over to the mobile goal
    straight(950ul, 127);
    stop();
    // put the cone on the mobile goal
    lift(DOWN, 1200ul);
    claw(OPEN);
    straight(32ul, -64);
    // do a 180
    turnCW(180, 0, 127);
    // pick up the mobile goal
    straight(35ul, -64);
    stop();
    mgl(UP, 1300ul);
    // drive over to the white tape
    straight(950ul, -127);
    // align with the 20pt zone
    turnCCW(45, 0, 64);
    straight(500ul, 127);
    turnCW(90, 0, 64);
    // score the mobile goal into the 20pt zone
    straight(530ul, -127);
    stop();
    mgl(DOWN, 1000ul);
    // get out of the bumps to give the driver some extra time
    straight(450ul, 127);
    stop();
}

void scoreStationary()
{
    stop();
    // start on the middle
    // pick up the cone
    claw(CLOSE);
    lift(UP, 4200ul);
    // go up to the stationary goal
    straight(96ul, 64);
    stop();
    // score the preload
    lift(DOWN, 950ul);
    claw(OPEN);
    // back up a bit to fully lower the lift
    straight(64ul, -64);
    stop();
    lift(DOWN, 2800ul);
}

void turnCW(unsigned int angle, int turnRadius, int leftPower)
{
    /*
     * T=time to make a full circle
     * leftSpeed*T = 2*pi*(turnRadius+BOT_RADIUS)
     * rightSpeed*T = 2*pi*(turnRadius-BOT_RADIUS)
     * T/(2*pi) = (turnRadius+BOT_RADIUS)/leftSpeed =
     *    (turnRadius-BOT_RADIUS)/rightSpeed
     * rightSpeed = leftSpeed*(turnRadius-BOT_RADIUS)/(turnRadius+BOT_RADIUS)
     * speed = (power/127)*(2*pi*WHEEL_RADIUS*MOTOR_SPEED)/(1 rotation)
     * (motor speed is proportional to motor power)
     * rightPower = leftPower*(turnRadius-BOT_RADIUS)/(turnRadius+BOT_RADIUS)
     */
    int rightPower = (((int) turnRadius - (int) BOT_RADIUS) * leftPower) /
        ((int) turnRadius + (int) BOT_RADIUS);
    /*
     * leftSpeed*T = 2*PI*(turnRadius+BOT_RADIUS)
     * T = 2*PI*(turnRadius+BOT_RADIUS)/leftSpeed
     * waitTime = ((angle*PI/180 rad)/(2*PI rad))*T
     * = (angle/360)*T
     * = (angle/360)*(2*PI*(turnRadius+BOT_RADIUS))/(leftSpeed)
     * = (angle*PI*(turnRadius+BOT_RADIUS))/(180*leftSpeed)
     * = (angle*PI*(turnRadius+BOT_RADIUS))/
     *       (180*leftPower/127*(2*PI*WHEEL_RADIUS*MOTOR_SPEED/(1 rotation)))
     * = (angle*127*(turnRadius+BOT_RADIUS))/
     *       (leftPower*360*WHEEL_RADIUS*MOTOR_SPEED) (seconds)
     * = (angle*(turnRadius+BOT_RADIUS)*3175)/
     *       (leftPower*WHEEL_RADIUS*MOTOR_SPEED*9) (milliseconds)
     * (rearranged to take advantage of integer arithmetic and constant folding)
     */
    unsigned long waitTime = ((turnRadius + BOT_RADIUS) * 3175ul * angle) /
        (WHEEL_RADIUS * MOTOR_SPEED * 9ul * leftPower);
    setLeftDriveTrain(leftPower);
    setRightDriveTrain(rightPower);
    taskDelay(waitTime);
}

void turnCCW(unsigned int angle, int turnRadius, int rightPower)
{
    int leftPower = (((int) turnRadius - (int) BOT_RADIUS) * rightPower) /
        ((int) turnRadius + (int) BOT_RADIUS);
    unsigned long waitTime = ((turnRadius + BOT_RADIUS) * 3175ul * angle) /
        (WHEEL_RADIUS * MOTOR_SPEED * 9ul * rightPower);
    setLeftDriveTrain(leftPower);
    setRightDriveTrain(rightPower);
    taskDelay(waitTime);
}

void straight(unsigned long distance, int power)
{
    /*
     * speed = (power/127)*(2*PI*WHEEL_RADIUS*MOTOR_SPEED/(1 rotation))
     * = (power*2*PI*WHEEL_RADIUS*MOTOR_SPEED)/127
     * waitTime = distance/speed
     * = (127*distance)/(2*PI*WHEEL_RADIUS*MOTOR_SPEED*power) s
     * = (127000*distance)/(2*PI*WHEEL_RADIUS*MOTOR_SPEED*power) ms
     */
    unsigned long waitTime = (127000ul * distance) /
        (2ul * WHEEL_RADIUS * MOTOR_SPEED * PI * (unsigned long) abs(power));
    setLeftDriveTrain(power);
    setRightDriveTrain(power);
    taskDelay(waitTime);
}

void stop()
{
    setLeftDriveTrain(0);
    setRightDriveTrain(0);
}

void lift(direction_t direction, unsigned long waitTime)
{
    setLiftLock(false);
    setLift(direction);
    taskDelay(waitTime);
    setLift(STOP);
    setLiftLock(true);
}

void claw(direction_t direction)
{
    setClaw(direction);
    taskDelay(CLAW_TIME);
    setClaw(STOP);
}

void mgl(direction_t direction, unsigned long waitTime)
{
    setMobileGoalLift(direction);
    taskDelay(waitTime);
    setMobileGoalLift(STOP);
}
