// contains the code that controls the LCD screen

#include "main.hpp"

// what port the LCD screen goes into
#define LCD_PORT uart1
// amount of milliseconds between each LCD update
#define LCD_POLL_SPEED 100ul

// the different types of things the LCD can do
enum LoopState
{
    // select the autonomous program
    AUTON_SELECT,
    // display primary/backup battery voltage
    DISPLAY_BATTERY,
    // control the lift from the LCD
    LIFT_CONTROL
};

// tracks the state of the buttons
class ButtonState
{
public:
    ButtonState(FILE* lcdPort): lcdPort(lcdPort), current(0), previous(0) {}

    // polls all the lcd buttons
    void poll()
    {
        previous = current;
        current = lcdReadButtons(lcdPort);
    }
    // checks if a button was pressed
    bool pressed(unsigned int button) const
    {
        return current & button;
    }
    // checks if a button was just pressed
    bool justPressed(unsigned int button) const
    {
        return pressed(button) && !(previous & button);
    }

private:
    FILE* lcdPort;
    unsigned int current;
    unsigned int previous;
};

// corresponds to state_t enum
// takes a reference to the ButtonState, returns what the LoopState should be
//  changed to
static LoopState autonSelect(const ButtonState& buttons);
static LoopState displayBattery(const ButtonState& buttons);
static LoopState liftControl(const ButtonState& buttons);

// declared in main.hpp
void lcd::controller(void*)
{
    lcdInit(LCD_PORT);
    lcdClear(LCD_PORT);
    lcdSetBacklight(LCD_PORT, false);
    // the action that should be taken, kinda like a state machine
    LoopState loopState = LIFT_CONTROL;
    // tells loop functions what buttons are being pressed
    ButtonState buttons(LCD_PORT);
    // used for timing cyclic delays
    unsigned long time = millis();
    while (true)
    {
        buttons.poll();
        // do a different loop action based on loopState
        switch (loopState)
        {
        case AUTON_SELECT:
            loopState = autonSelect(buttons);
            break;
        case DISPLAY_BATTERY:
            loopState = displayBattery(buttons);
            break;
        case LIFT_CONTROL:
            loopState = liftControl(buttons);
            break;
        }
        // wait a bit before receiving input again
        taskDelayUntil(&time, LCD_POLL_SPEED);
    }
}

LoopState autonSelect(const ButtonState& buttons)
{
    // so we don't have to type "auton::" 5 billion times
    using namespace auton;
    // used for printing the name of an autonomous program
    static const char* autonNames[AUTONID_MAX + 1] =
    {
        "Nothing",
        "Forward+Backward",
        "MG+Cone Left",
        "MG+Cone Right",
        "Score Stationary"
    };
    // see if the left/right buttons were just pressed
    bool left = buttons.justPressed(LCD_BTN_LEFT);
    bool right = buttons.justPressed(LCD_BTN_RIGHT);
    // if left, go up the autonNames list
    if (left && !right)
    {
        autonid = (AutonID) (autonid - 1);
        if (autonid < AUTONID_MIN || autonid > AUTONID_MAX)
        {
            // go back to the end of the list
            autonid = AUTONID_MAX;
        }
    }
    // if right, go down the autonNames list
    else if (!left && right)
    {
        autonid = (AutonID) (autonid + 1);
        if (autonid < AUTONID_MIN || autonid > AUTONID_MAX)
        {
            // go back to the start of the list
            autonid = AUTONID_MIN;
        }
    }
    lcdSetText(LCD_PORT, 1, ROBOT_NAME " will do:");
    lcdSetText(LCD_PORT, 2, autonNames[autonid]);
    // if auton selected or enabled by comp switch, start displaying battery
    if (buttons.justPressed(LCD_BTN_CENTER))
    {
        return DISPLAY_BATTERY;
    }
    return AUTON_SELECT;
}

LoopState displayBattery(const ButtonState& buttons)
{
    lcdPrint(LCD_PORT, 1, "Primary: %.1fV", powerLevelMain() / 1000.0f);
    lcdPrint(LCD_PORT, 2, "Backup:  %.1fV", powerLevelBackup() / 1000.0f);
    if (buttons.justPressed(LCD_BTN_CENTER))
    {
        return LIFT_CONTROL;
    }
    return DISPLAY_BATTERY;
}

LoopState liftControl(const ButtonState& buttons)
{
    lcdPrint(LCD_PORT, 1, "lift pos = %.1f", motor::getLiftPos());
    lcdSetText(LCD_PORT, 2, "v              ^");
    if (buttons.pressed(LCD_BTN_LEFT))
    {
        motor::setLift(-127);
    }
    else if (buttons.pressed(LCD_BTN_RIGHT))
    {
        motor::setLift(127);
    }
    else if (!isJoystickConnected(1))
    {
        motor::setLift(0);
    }
    if (buttons.justPressed(LCD_BTN_CENTER))
    {
        return AUTON_SELECT;
    }
    return LIFT_CONTROL;
}
