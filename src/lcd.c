// contains the code that controls the LCD screen

#include "main.h"

// what port the LCD screen goes into
#define LCD uart1
// amount of milliseconds between each LCD update
#define LCD_POLL_SPEED 100ul

// declared in main.h
autonid_t autonid = SCORE_STATIONARY;

// used for printing the name of an autonomous program
static const char* autonNames[AUTONID_COUNT] =
{
    "Forward+Backward",
    "Score MG w/Cone",
    "Score Stationary"
};

// the different types of things the LCD can do
typedef enum State
{
    // select the autonomous program
    AUTON_SELECT,
    // display primary/backup battery voltage
    DISPLAY_BATTERY
}
state_t;

// tells lcdLoop() what LCD action it should take
static state_t loopState = AUTON_SELECT;

// called in a task loop to control the LCD and respond to input
static void lcdLoop();

// corresponds to state_t enum
static void autonSelect();
static void displayBattery();

// declared in main.h
void lcdMain()
{
    lcdInit(LCD);
    lcdClear(LCD);
    taskRunLoop(lcdLoop, LCD_POLL_SPEED);
}

void lcdLoop()
{
    switch (loopState)
    {
    case AUTON_SELECT:
        autonSelect();
        break;
    case DISPLAY_BATTERY:
        displayBattery();
        break;
    }
}

void autonSelect()
{
    static int buttonState = 0;
    static int lastButtonState;
    lastButtonState = buttonState;
    buttonState = lcdReadButtons(LCD);
    // see if the left/right buttons were just pressed
    bool left = buttonState & LCD_BTN_LEFT &&
        !(lastButtonState & LCD_BTN_LEFT);
    bool right = buttonState & LCD_BTN_RIGHT &&
        !(lastButtonState & LCD_BTN_RIGHT);
    // if left, go up the autonNames list
    if (left && !right)
    {
        autonid = (autonid_t) (autonid - 1);
        if (autonid < AUTONID_MIN || autonid > AUTONID_MAX)
        {
            // go back to the end of the list
            autonid = AUTONID_MAX;
        }
    }
    // if right, go down the autonNames list
    else if (!left && right)
    {
        autonid = (autonid_t) (autonid + 1);
        if (autonid < AUTONID_MIN || autonid > AUTONID_MAX)
        {
            // go back to the start of the list
            autonid = AUTONID_MIN;
        }
    }
    lcdSetText(LCD, 1, ROBOT_NAME " will do:");
    lcdSetText(LCD, 2, autonNames[autonid]);
    // if auton selected or enabled by comp switch, start displaying battery
    if (buttonState & LCD_BTN_CENTER || isEnabled())
    {
        loopState = DISPLAY_BATTERY;
    }
}

void displayBattery()
{
    lcdPrint(LCD, 1, "Primary: %.1fV", powerLevelMain() / 1000.0f);
    lcdPrint(LCD, 2, "Backup:  %.1fV", powerLevelBackup() / 1000.0f);
}
