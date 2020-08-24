#include "../../include/events.h"

#ifdef _WIN32

    DWORD WINAPI events_getter(void *args) 
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
                set_states_reading_flag(1);
                HANDLE thread_id1 = CreateThread(NULL, 0, events_getter, NULL, 0, NULL);
                break;

            case DLL_PROCESS_DETACH:
                eventsMainloopKey = false;
                free_events_memory();
                flush_console_input_buffer();
                break;
        }
        return TRUE;
    }

#else

    pthread_t thread_id0;
    pthread_t thread_id1;

    void * events_getter(void *args)
    {
        while (eventsMainloopKey)
            get_events_state();
        return NULL;
    }

    __attribute__((constructor)) void ProcessAttach(void)
    {
        /*
            enable reading flag to avoid conflicts
            xamalk's EventsManager will turn off it after writing all events to the memory
        */
        set_states_reading_flag(2);
        connect_keyboards();
        #ifndef __APPLE__
            pthread_create(&thread_id0, NULL, system_input_thread, NULL);
        #endif
        pthread_create(&thread_id1, NULL, events_getter, NULL);
    }

    __attribute__((destructor)) void ProcessDetach(void)
    {
        eventsMainloopKey = false;
        pthread_join(thread_id1, NULL);
        set_states_reading_flag(1);          //  send signal to system_input_thread that events_getter is done
        #ifndef __APPLE__
            pthread_join(thread_id0, NULL);
        #endif
        free_events_memory();
        disconnect_keyboards();
        flush_console_input_buffer();
    }

#endif
