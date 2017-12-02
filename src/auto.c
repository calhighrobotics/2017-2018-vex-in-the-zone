// contains code for the autonomous period and anything else associated with it

#include "main.h"

// angles are in degrees, distances are in 1/16 inches
#define WHEEL_RADIUS 32ul
#define BOT_RADIUS 120ul
// not in parentheses to take advantage of only doing integer arithmetic
#define MOTOR_SPEED 5ul/3ul // rot/s
#define PI 22ul/7ul // overestimated (22/7>PI) because ints always round down
#define CLAW_TIME 100ul // ms

// autonomous plans
static void forwardAndBack();
static void stationaryGoal();

static void turnCW(unsigned int angle, int turnRadius, int leftPower);
static void turnCCW(unsigned int angle, int turnRadius, int rightPower);
static void straight(unsigned long distance, int power);
static void stop();
static void lift(direction_t direction, unsigned long waitTime);
static void claw(direction_t direction);

// main point of execution for the autonomous period
void autonomous()
{
    //forwardAndBack();
    stationaryGoal();
}

void forwardAndBack()
{
    straight(100ul, 95);
    straight(100ul, -95);
    stop();
}

void stationaryGoal()
{
    // start on the middle
    // pick up the cone
    claw(CLOSE);
    lift(UP, 3500ul);
    // go up to the stationary goal
    straight(128ul, 127);
    stop();
    // score the preload
    lift(DOWN, 1100ul);
    claw(OPEN);
    straight(32ul, -127);
    lift(DOWN, 2400ul);
    // back up a bit
    straight(95ul, -127);
    stop();
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
    setLeftDriveTrain(leftPower);
    setRightDriveTrain(rightPower);
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
    taskDelay(waitTime);
}

void turnCCW(unsigned int angle, int turnRadius, int rightPower)
{
    int leftPower = (((int) turnRadius - (int) BOT_RADIUS) * rightPower) /
        ((int) turnRadius + (int) BOT_RADIUS);
    setLeftDriveTrain(leftPower);
    setRightDriveTrain(rightPower);
    unsigned long waitTime = ((turnRadius + BOT_RADIUS) * 3175ul * angle) /
        (WHEEL_RADIUS * MOTOR_SPEED * 9ul * rightPower);
    taskDelay(waitTime);
}

void straight(unsigned long distance, int power)
{
    setLeftDriveTrain(power);
    setRightDriveTrain(power);
    /*
     * speed = (power/127)*(2*PI*WHEEL_RADIUS*MOTOR_SPEED/(1 rotation))
     * = (power*2*PI*WHEEL_RADIUS*MOTOR_SPEED)/127
     * waitTime = distance/speed
     * = (127*distance)/(2*PI*WHEEL_RADIUS*MOTOR_SPEED*power) s
     * = (127000*distance)/(2*PI*WHEEL_RADIUS*MOTOR_SPEED*power) ms
     */
    unsigned long waitTime = (127000ul * distance) /
        (2ul * WHEEL_RADIUS * MOTOR_SPEED * PI * (unsigned long) abs(power));
    taskDelay(waitTime);
}

void stop()
{
    setLeftDriveTrain(0);
    setRightDriveTrain(0);
}

void lift(direction_t direction, unsigned long waitTime)
{
    setLift(direction);
    taskDelay(waitTime);
    setLift(STOP);
}

void claw(direction_t direction)
{
    setClaw(direction);
    taskDelay(CLAW_TIME);
    setClaw(STOP);
}
