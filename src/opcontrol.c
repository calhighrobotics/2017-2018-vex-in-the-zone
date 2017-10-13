/** @file opcontrol.c
 * @brief File for operator control code
 *
 * This file should contain the user operatorControl() function and any functions related to it.
 *
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * PROS contains FreeRTOS (http://www.freertos.org) whose source code may be
 * obtained from http://sourceforge.net/projects/freertos/files/ or on request.
 */

#include "main.h"

// uncomment this line to enable arcade controls
#define TANK_CONTROLS
#define THRESHOLD 4

// returns 0 when -THRESHOLD<=value<=THRESHOLD to prevent joystick ghosting
int threshold(int value)
{
    return abs(value) > THRESHOLD ? value : 0;
}

/*
 * Runs the user operator control code. This function will be started in its own task with the
 * default priority and stack size whenever the robot is enabled via the Field Management System
 * or the VEX Competition Switch in the operator control mode. If the robot is disabled or
 * communications is lost, the operator control task will be stopped by the kernel. Re-enabling
 * the robot will restart the task, not resume it from where it left off.
 *
 * If no VEX Competition Switch or Field Management system is plugged in, the VEX Cortex will
 * run the operator control task. Be warned that this will also occur if the VEX Cortex is
 * tethered directly to a computer via the USB A to A cable without any VEX Joystick attached.
 *
 * Code running in this task can take almost any action, as the VEX Joystick is available and
 * the scheduler is operational. However, proper use of delay() or taskDelayUntil() is highly
 * recommended to give other tasks (including system tasks such as updating LCDs) time to run.
 *
 * This task should never exit; it should end with some kind of infinite loop, even if empty.
 */
void operatorControl()
{
    int left;
    int right;
    bool liftUp;
    bool liftDown;
    while (1)
    {
#ifdef TANK_CONTROLS
        left = threshold(joystickGetAnalog(1, 3));
        right = threshold(joystickGetAnalog(1, 2));
#else
        left = threshold(joystickGetAnalog(1, 3)) + threshold(joystickGetAnalog(1, 1));
        right = threshold(joystickGetAnalog(1, 3)) - threshold(joystickGetAnalog(1, 1));
#endif
        liftUp = joystickGetDigital(1, 6, JOY_UP);
        liftDown = joystickGetDigital(1, 6, JOY_DOWN);
        motorSet(DRIVE_FL, -left);
        motorSet(DRIVE_BL, -left);
        motorSet(DRIVE_FR, right);
        motorSet(DRIVE_BR, right);
        if (liftUp && !liftDown)
        {
            motorSet(LIFT_1, 127);
            motorSet(LIFT_2, -127);
        }
        else if (!liftUp && liftDown)
        {
            motorSet(LIFT_1, -127);
            motorSet(LIFT_2, 127);
        }
        else
        {
            motorSet(LIFT_1, 0);
            motorSet(LIFT_2, 0);
        }
        delay(20);
    }
}
