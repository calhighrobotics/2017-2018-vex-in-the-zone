// contains code for the autonomous period and anything else associated with it

#include "main.h"

#define WHEEL_RADIUS 32ul // 1/16 in
#define BOT_RADIUS 120ul // 1/16 in
// not in parentheses to take advantage of only doing integer arithmetic
#define MOTOR_SPEED 5ul/3ul // rot/s
#define PI 22ul/7ul // overestimated (22/7>PI) because ints always round down
#define CLAW_TIME 100ul // ms

// angles are in degrees, distances are in 1/16 inches
static void turnCW(unsigned int angle, int turnRadius, int leftPower);
static void turnCCW(unsigned int angle, int turnRadius, int rightPower);
static void straight(unsigned long distance, int power);
static inline void lift(direction_t direction, unsigned long waitTime);
static inline void claw(direction_t direction);

// main point of execution for the autonomous period
void autonomous()
{
    // start on very left of the 5pt zone
    // pick up the preload and lift it a couple inches off the ground
    claw(CLOSE);
    lift(UP, 100ul);
    // turn 45* clockwise, powered mostly by the right side
        // this should give the robot a clear shot to go straight for the mg
    turnCW(45, BOT_RADIUS, 127);
    // go forward with the preload until some distance away from the mg
    straight(755ul, 127);
    // lift the preload while still going towards the mg
    // once the lift is up, the robot should be right infront of the mg
    // lower the preload onto the mg
    lift(DOWN, 100ul);
    // back up a bit
    straight(64, 64);
    // turn 180*
    // back up the same distance to secure the mg on the mgl
    // lift up the mgl a bit
    // go forward until the center of the robot is at the 5pt zone line
    // turn 135* clockwise
    // go backward 1 tile diagonal along the 5pt zone line until at the middle
    // turn 90* clockwise
    // go backward until the back wheel is over the 20pt zone bump
    // lower the mgl to score an easy 20 points
    // go forward the same distance and stop
}

void turnCW(unsigned int angle, int turnRadius, int leftPower)
{
    unsigned long before = millis();
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
     *       (180*leftPower*(2*PI*WHEEL_RADIUS*MOTOR_SPEED/(1 rotation)))
     * = (angle*127*(turnRadius+BOT_RADIUS))/
     *       (leftPower*360*WHEEL_RADIUS*MOTOR_SPEED) (seconds)
     * = (angle*(turnRadius+BOT_RADIUS)*3175)/
     *       (leftPower*WHEEL_RADIUS*MOTOR_SPEED*9) (milliseconds)
     * (rearranged to take advantage of integer arithmetic and constant folding)
     */
    unsigned long waitTime = ((turnRadius + BOT_RADIUS) * 3175ul * angle) /
        (WHEEL_RADIUS * MOTOR_SPEED * 9ul * leftPower);
    unsigned long after = millis();
    taskDelay(waitTime - (after - before));
}

void turnCCW(unsigned int angle, int turnRadius, int rightPower)
{
    unsigned long before = millis();
    int leftPower = (((int) turnRadius - (int) BOT_RADIUS) * rightPower) /
        ((int) turnRadius + (int) BOT_RADIUS);
    setLeftDriveTrain(leftPower);
    setRightDriveTrain(rightPower);
    unsigned long waitTime = ((turnRadius + BOT_RADIUS) * 3175ul * angle) /
        (WHEEL_RADIUS * MOTOR_SPEED * 9ul * rightPower);
    unsigned long after = millis();
    taskDelay(waitTime - (after - before));
}

void straight(unsigned long distance, int power)
{
    unsigned long before = millis();
    setLeftDriveTrain(power);
    setRightDriveTrain(power);
    /*
     * speed = (power/127)*(2*PI*WHEEL_RADIUS*MOTOR_SPEED/(1 rotation))
     * = (power*2*PI*WHEEL_RADIUS*MOTOR_SPEED)/127
     * waitTime = speed/distance
     * = (2*PI*WHEEL_RADIUS*MOTOR_SPEED*power)/(127*distance) (seconds)
     * = (2000*PI*WHEEL_RADIUS*MOTOR_SPEED*power)/(127*distance) (milliseconds)
     */
    unsigned long waitTime = (2000ul * WHEEL_RADIUS * MOTOR_SPEED * PI *
            (unsigned long) abs(power)) /
        (127ul * distance);
    unsigned long after = millis();
    taskDelay(waitTime - (after - before));
}

void lift(direction_t direction, unsigned long waitTime)
{
    setLift(direction);
    taskDelay(waitTime);
    setLift(0);
}

void claw(direction_t direction)
{
    setClaw(direction);
    taskDelay(CLAW_TIME);
    setClaw(0);
}
