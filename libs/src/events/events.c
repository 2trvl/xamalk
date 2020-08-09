#include "../../include/events.h"

States eventsStates;
Event *events = NULL;
bool eventsMainloopKey = true;

#if defined(_WIN32)

    unsigned char VIRTUAL_KEYS[94] = {
        VK_LBUTTON, VK_RBUTTON, VK_MBUTTON, VK_BACK, VK_TAB, VK_RETURN, VK_SHIFT, VK_CONTROL, 
        VK_MENU, VK_PAUSE, VK_CAPITAL, VK_ESCAPE, VK_SPACE, VK_PRIOR, VK_NEXT, VK_END, VK_HOME, 
        VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN, VK_SNAPSHOT, VK_INSERT, VK_DELETE, 0x30, 0x31, 0x32, 
        0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 
        0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
        0x58, 0x59, 0x5A, VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5,
        VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9, VK_MULTIPLY, VK_ADD, VK_SEPARATOR, VK_SUBTRACT,
        VK_DECIMAL, VK_DIVIDE, VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10,
        VK_F11, VK_F12, VK_NUMLOCK, VK_OEM_PLUS, VK_OEM_COMMA, VK_OEM_MINUS, VK_OEM_PERIOD, VK_OEM_3};

    void get_events_state()
    {

        if (!eventsStates.reading)
        {
            bool buttonPressed = true;
            bool buttonReleased = false;
            bool releaseCondition = true;
            HWND consoleWindow = GetConsoleWindow();
            register unsigned int index = 0, key = events[index].condition.amount;
            
            while (index < eventsStates.amount)
            {
                if (key > 0 && events[index].type)              //  Type of Event: Release
                {
                    bool lastPressedValue = eventsStates.lastPressedVirtualKeys[index][key-1];
                    
                    //  current button pressed right now
                    if (GetAsyncKeyState(events[index].condition.virtualKeys[key-1]) < 0 && (consoleWindow == GetForegroundWindow()))
                    {
                        /*  
                            !(lastPressed(1) -> currentPressed(1)) = false
                            buttonReleased = buttonReleased || false;

                            since the first condition is whether the button was released when it was pressed before is incorrectly
                            we check the second condition: has the button been pressed since the last time
                        */
                        releaseCondition = releaseCondition && lastPressedValue;
                        eventsStates.lastPressedVirtualKeys[index][key-1] = true;
                    }

                    else
                    {
                        //  maybe !(lastPressed(1) -> currentPressed(0)) = true
                        buttonReleased = buttonReleased || lastPressedValue;
                        eventsStates.lastPressedVirtualKeys[index][key-1] = false;
                        //  but if not so conditions are not met
                        if (!lastPressedValue)
                        {
                            releaseCondition = false;
                            goto COUNT;
                        }
                    }
                    --key;
                }

                else if (key > 0 && !events[index].type)        //  Type of Event: Press
                {

                    //  current button pressed right now
                    if (GetAsyncKeyState(events[index].condition.virtualKeys[key-1]) < 0 && (consoleWindow == GetForegroundWindow()))
                    {
                        buttonPressed = buttonPressed && true;
                        eventsStates.lastPressedVirtualKeys[index][key-1] = true;
                    }
                    
                    //  condition that button need to be pressed are not met
                    else
                    {
                        buttonPressed = buttonPressed && false;
                        eventsStates.lastPressedVirtualKeys[index][key-1] = false;
                        goto COUNT;
                    }
                    --key;
                }

                else                                             //  Error: Out of Key Range
                COUNT:{
                    if (events[index].type)
                        eventsStates.status[index] = buttonReleased && releaseCondition;
                    else
                        eventsStates.status[index] = buttonPressed;

                    ++index;
                    buttonPressed = true;
                    buttonReleased = false;
                    releaseCondition = true;
                    key = events[index].condition.amount;
                }
                
            }
        }
    }

    DWORD WINAPI get_input_characters(void *data) 
    {
        DWORD mode;
        HANDLE currentHandle = GetStdHandle(STD_INPUT_HANDLE);
        while (eventsMainloopKey)
        {
            if (GetStdHandle(STD_INPUT_HANDLE)) 
            {
                GetConsoleMode(currentHandle, &mode);
                SetConsoleMode(currentHandle, mode & ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT));  //  off stdin
                getchar();
                SetConsoleMode(currentHandle, mode);
            }
        }
        return 0;
    }

#endif

void exit_events_mainloop()
{
    eventsMainloopKey = false;
}

void free_events_memory()
{
    if (events != NULL)
    {
        for (register unsigned int index = 0; index < eventsStates.amount; ++index)
        {
            if (events[index].condition.virtualKeys != NULL)
            {
                free(events[index].condition.virtualKeys);
                events[index].condition.virtualKeys = NULL;
                free(eventsStates.lastPressedVirtualKeys[index]);
            }
        }

        free(events);
        events = NULL;
        eventsStates.amount = 0;
        free(eventsStates.status);
        free(eventsStates.lastPressedVirtualKeys);
    }
}

void execute_events_actions()
{
    for (register unsigned int index = 0; index < eventsStates.amount; ++index)
    {
        if (eventsStates.status[index])
        {
            if (events[index].actionArguments != NULL)
                events[index].action(events[index].actionArguments);
            else
                ((void (*)())events[index].action)();
        }
    }
}

void events_mainloop(Shortcut *quit)
{
    bind_event(quit, Release, (void (*)(void*))exit_events_mainloop, NULL, eventsStates.amount-1);
    
    #ifdef _WIN32
        HANDLE thread = CreateThread(NULL, 0, get_input_characters, NULL, 0, NULL);
    #endif

    while (eventsMainloopKey)
    {
        get_events_state();
        execute_events_actions();
    }
}

void set_states_reading_flag(bool flag)
{
    eventsStates.reading = flag;
}

void create_events(unsigned char amount)
{
    amount += 1;            //  reserved for quit event
    free_events_memory();
    events = (Event*)malloc(sizeof(Event) * amount);

    eventsStates.amount = amount;
    eventsStates.status = (bool*)malloc(sizeof(bool) * amount);
    eventsStates.lastPressedVirtualKeys = (bool**)malloc(sizeof(bool*) * amount);

    for (register unsigned int index = 0; index < amount; ++index)
    {
        eventsStates.status[index] = false;
        events[index].condition.virtualKeys = NULL;
    }
}

void bind_event(Shortcut *condition, EventType type, void (*action)(void*), void *actionArguments, unsigned char index)
{
    events[index].type = type;
    events[index].action = action;
    events[index].actionArguments = actionArguments;

    events[index].condition.amount = condition->amount;
    events[index].condition.virtualKeys = (unsigned char*)malloc(sizeof(unsigned char) * condition->amount);

    eventsStates.lastPressedVirtualKeys[index] = (bool*)malloc(sizeof(bool) * condition->amount);

    for (register unsigned int key = 0; key < condition->amount; ++key)
    {
        eventsStates.lastPressedVirtualKeys[index][key] = false;
        events[index].condition.virtualKeys[key] = VIRTUAL_KEYS[condition->virtualKeys[key]];
    }
}
