// contains code for the autonomous period and anything else associated with it

#include "main.hpp"

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
static void lift(motor::Direction direction, unsigned long waitTime);
static void claw(motor::Direction direction);
static void mgl(motor::Direction direction, unsigned long waitTime);

// main point of execution for the autonomous period
void autonomous()
{
    switch (auton::autonid)
    {
    case auton::FORWARD_BACKWARD:
        forwardBackward();
        break;
    case auton::SCORE_MG_WITH_CONE:
        scoreMgWithCone();
        break;
    case auton::SCORE_STATIONARY:
        scoreStationary();
        break;
    }
}

void forwardBackward()
{
    straight(100, 95);
    straight(100, -95);
    stop();
}

void scoreMgWithCone()
{
    using namespace motor;
    // pick up the cone
    claw(CLOSE);
    lift(UP, 1200);
    // drive over to the mobile goal
    straight(950 , 127);
    stop();
    // put the cone on the mobile goal
    lift(DOWN, 1200);
    claw(OPEN);
    straight(32, -64);
    // do a 180
    turnCW(180, 0, 127);
    // pick up the mobile goal
    straight(35, -64);
    stop();
    mgl(UP, 1300);
    // drive over to the white tape
    straight(950, -127);
    // align with the 20pt zone
    turnCCW(45, 0, 64);
    straight(500, 127);
    turnCW(90, 0, 64);
    // score the mobile goal into the 20pt zone
    straight(530, -127);
    stop();
    mgl(DOWN, 1000);
    // get out of the bumps to give the driver some extra time
    straight(450, 127);
    stop();
}

void scoreStationary()
{
    using namespace motor;
    // start on the middle
    // pick up the cone
    claw(CLOSE);
    lift(UP, 4200);
    // go up to the stationary goal
    straight(96, 64);
    stop();
    // score the preload
    lift(DOWN, 950);
    claw(OPEN);
    // back up a bit to fully lower the lift
    straight(64, -64);
    stop();
    lift(DOWN, 2800);
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
    motor::setLeftDriveTrain(leftPower);
    motor::setRightDriveTrain(rightPower);
    taskDelay(waitTime);
}

void turnCCW(unsigned int angle, int turnRadius, int rightPower)
{
    int leftPower = (((int) turnRadius - (int) BOT_RADIUS) * rightPower) /
        ((int) turnRadius + (int) BOT_RADIUS);
    unsigned long waitTime = ((turnRadius + BOT_RADIUS) * 3175ul * angle) /
        (WHEEL_RADIUS * MOTOR_SPEED * 9ul * rightPower);
    motor::setLeftDriveTrain(leftPower);
    motor::setRightDriveTrain(rightPower);
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
    motor::setLeftDriveTrain(power);
    motor::setRightDriveTrain(power);
    taskDelay(waitTime);
}

void stop()
{
    motor::setLeftDriveTrain(0);
    motor::setRightDriveTrain(0);
}

void lift(motor::Direction direction, unsigned long waitTime)
{
    motor::setLiftLock(false);
    motor::setLift(direction);
    taskDelay(waitTime);
    motor::setLift(motor::STOP);
    motor::setLiftLock(true);
}

void claw(motor::Direction direction)
{
    motor::setClaw(direction);
    taskDelay(CLAW_TIME);
    motor::setClaw(motor::STOP);
}

void mgl(motor::Direction direction, unsigned long waitTime)
{
    motor::setMobileGoalLift(direction);
    taskDelay(waitTime);
    motor::setMobileGoalLift(motor::STOP);
}