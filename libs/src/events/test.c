#include "../../include/events.h"

void example_callback(void *data)
{
    char *string = (char*)data;
    printf("%s\n", string);
}

int main(void)
{
    create_events(4);

    Shortcut hotkey;
    hotkey.amount = 1;
    hotkey.virtualKeys = (unsigned char*)malloc(sizeof(unsigned char) * 2);
    hotkey.virtualKeys[0] = X;
    
    bind_event(&hotkey, Press, example_callback, (void*)"'X' was pressed", 0);
    bind_event(&hotkey, Release, example_callback, (void*)"'X' was released", 1);

    hotkey.amount = 2;
    hotkey.virtualKeys[0] = TYPEWRITER_ADD;
    hotkey.virtualKeys[1] = CTRL;

    bind_event(&hotkey, Press, example_callback, (void*)"'CTRL' + '+' was pressed", 2);
    bind_event(&hotkey, Release, example_callback, (void*)"'CTRL' + '+' was released", 3);

    hotkey.amount = 1;
    hotkey.virtualKeys[0] = Q;
    events_mainloop(&hotkey);
    
    free(hotkey.virtualKeys);
    free_events_memory();
    return 0;
}
