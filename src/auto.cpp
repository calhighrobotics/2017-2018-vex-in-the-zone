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
static void scoreMgWithCone(bool left);
static void scoreStationary();

// dt functions require a stop() at the end to allow chaining
static void turnCW(unsigned int angle, int turnRadius, int leftPower);
static void turnCCW(unsigned int angle, int turnRadius, int rightPower);
static void straight(unsigned long distance, int power);
static void stop();
// other stuff
static void lift(double target); // max=127, min=0
static void claw(motor::Direction direction);
static void mgl(double target);

// main point of execution for the autonomous period
void autonomous()
{
    switch (auton::autonid)
    {
    case auton::FORWARD_BACKWARD:
        forwardBackward();
        break;
    case auton::MG_CONE_LEFT:
        scoreMgWithCone(true);
        break;
    case auton::MG_CONE_RIGHT:
        scoreMgWithCone(false);
        break;
    case auton::SCORE_STATIONARY:
        scoreStationary();
        break;
    default:
        ; // just do nothing
    }
}

void forwardBackward()
{
    straight(100, 95);
    straight(100, -95);
    stop();
}

void scoreMgWithCone(bool left)
{
    using namespace motor;
    // start pointed backwards, with the cone in the mgl part
    // pick up the cone
    claw(CLOSE);
    lift(63);
    // drive over to the mobile goal
    straight(500, -127);
    stop();
    // put the cone on the mobile goal
    lift(-31);
    claw(OPEN);
    // pick up the mobile goal
    mgl(63);
    // drive over to the white tape
    straight(740, 127);
    // align with the 20pt zone
    if (left)
    {
        turnCCW(45, 0, 64);
        straight(512, 127);
        turnCCW(90, 0, 64);
    }
    else
    {
        turnCW(45, 0, 64);
        straight(512, 127);
        turnCW(90, 0, 64);
    }
    // score the mobile goal into the 20pt zone
    straight(530, -127);
    stop();
    mgl(0);
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
    lift(126);
    // go up to the stationary goal
    straight(160, 64);
    stop();
    // score the preload
    lift(100);
    claw(OPEN);
    // back up a bit to fully lower the lift
    straight(64, -64);
    stop();
    claw(CLOSE);
    lift(0);
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
    // calculate the arc outside length
    unsigned long distance = (turnRadius + BOT_RADIUS) * angle * PI / 180;
    motor::resetDT();
    motor::setLeftDriveTrain(leftPower);
    motor::setRightDriveTrain(rightPower);
    unsigned long now = millis();
    while (2.0 * WHEEL_RADIUS * PI * motor::getLeftRotations() < distance)
    {
        taskDelayUntil(&now, MOTOR_POLL_RATE);
    }
}

void turnCCW(unsigned int angle, int turnRadius, int rightPower)
{
    // very similar to how turnCW calculates rightPower
    int leftPower = (((int) turnRadius - (int) BOT_RADIUS) * rightPower) /
        ((int) turnRadius + (int) BOT_RADIUS);
    // calculate the outside arc length
    unsigned long distance = (turnRadius + BOT_RADIUS) * angle * PI / 180;
    motor::resetDT();
    motor::setLeftDriveTrain(leftPower);
    motor::setRightDriveTrain(rightPower);
    unsigned long now = millis();
    while (2.0 * WHEEL_RADIUS * PI * motor::getRightRotations() < distance)
    {
        taskDelayUntil(&now, MOTOR_POLL_RATE);
    }
}

void straight(unsigned long distance, int power)
{
    motor::resetDT();
    motor::setLeftDriveTrain(power);
    motor::setRightDriveTrain(power);
    unsigned long now = millis();
    while (2.0 * WHEEL_RADIUS * PI * abs(motor::getLeftRotations()) < distance)
    {
        taskDelayUntil(&now, MOTOR_POLL_RATE);
    }
}

void stop()
{
    motor::setLeftDriveTrain(0);
    motor::setRightDriveTrain(0);
}

void lift(double target)
{
    double error = target - motor::getLiftPos();
    unsigned long now = millis();
    if (error > 0)
    {
        // going up
        while (motor::getLiftPos() < target)
        {
            motor::setLift(127);
            taskDelayUntil(&now, MOTOR_POLL_RATE);
        }
    }
    else if (error < 0)
    {
        // going down
        while (motor::getLiftPos() > target)
        {
            motor::setLift(-127);
            taskDelayUntil(&now, MOTOR_POLL_RATE);
        }
    }
    // stop the lift
    motor::setLift(0);
}

void claw(motor::Direction direction)
{
    motor::setClaw(direction);
    taskDelay(CLAW_TIME);
    motor::setClaw(motor::STOP);
}

void mgl(double target)
{
    double error = target - motor::getMglPos();
    unsigned long now = millis();
    if (error > 0)
    {
        // going up
        while (motor::getMglPos() < target)
        {
            motor::setMgl(63);
            taskDelayUntil(&now, MOTOR_POLL_RATE);
        }
    }
    else if (error < 0)
    {
        // going down
        while (motor::getMglPos() > target)
        {
            motor::setMgl(-63);
            taskDelayUntil(&now, MOTOR_POLL_RATE);
        }
    }
    // stop the lift
    motor::setMgl(0);
}
