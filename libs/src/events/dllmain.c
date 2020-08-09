#include "../../include/events.h"

#ifdef _WIN32

    DWORD WINAPI events_getter(void *data) 
    {
        while (eventsMainloopKey)
            get_events_state();
        return 0;
    }

    BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
    {
        switch(fdwReason) 
        { 
            case DLL_PROCESS_ATTACH:
                /*
                    enable reading flag to avoid conflicts
                    xamalk's EventsManager will turn off it after writing all events to the memory
                */
                eventsStates.reading = true;
                HANDLE thread0 = CreateThread(NULL, 0, events_getter, NULL, 0, NULL);
                HANDLE thread1 = CreateThread(NULL, 0, get_input_characters, NULL, 0, NULL);
                break;

            case DLL_PROCESS_DETACH:
                eventsMainloopKey = false;
                free_events_memory();
                break;
        }
        return TRUE;
    }

#endif
