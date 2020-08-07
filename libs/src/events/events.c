#include "../../include/events.h"

Event *events = NULL;
Shortcut events_states;

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
        for (register unsigned int index = 0; index < events_states.amount; ++index)
        {
            if (true)
                events_states.virtual_keys[index] = true;
            else
                events_states.virtual_keys[index] = false;
        }
    }

#endif

bool events_mainloop_key = true;

void exit_events_mainloop()
{
    events_mainloop_key = false;
}

void free_events_memory()
{
    if (events != NULL)
    {
        free(events);
        events = NULL;
        events_states.amount = 0;
        free(events_states.virtual_keys);
        events_states.virtual_keys = NULL;
    }
}

void execute_events_actions()
{
    for (register unsigned int index = 0; index < events_states.amount; ++index)
    {
        if (events_states.virtual_keys[index])
            events[index].action();
    }
}

void events_mainloop(Shortcut *quit)
{
    bind_event(quit, Release, exit_events_mainloop, -1);

    while (events_mainloop_key)
    {
        get_events_state();
        execute_events_actions();
    }
}

void create_events(unsigned char amount)
{
    amount += 1;            //  reserved for quit event
    free_events_memory();
    events = (Event*)malloc(sizeof(Event) * amount);
    events_states.amount = amount;
    events_states.virtual_keys = (unsigned char*)malloc(sizeof(unsigned char) * amount);
}

void bind_event(Shortcut *condition, EventState state, void (*action)(), char index)
{
    events[index].state = state;
    events[index].action = action;
    events[index].condition.amount = condition->amount;

    for (register unsigned int key = 0; key < condition->amount; ++key)
        events[index].condition.virtual_keys[key] = VIRTUAL_KEYS[condition->virtual_keys[key]];
}
