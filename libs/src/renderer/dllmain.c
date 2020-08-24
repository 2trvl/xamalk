#include "../../include/renderer.h"

unsigned char key = 1;

#ifdef _WIN32

    DWORD WINAPI resizer(void *args) 
    {
        while (key)
        {
            get_terminal_size();
            time_sleep(200);
        }
        return 0;
    }

    BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
    {
        switch(fdwReason) 
        { 
            case DLL_PROCESS_ATTACH:
                ENABLE_ANSI
                CREATE_ALTERNATE_SCREEN_BUFFER
                get_terminal_size();
                switch_screen(true);
                #ifndef VT
                    change_stdin_visibility(false);    //  on VT it'll block KeyboardInterrupt and other calls
                #endif
                change_cursor_visibility(false);
                HANDLE thread = CreateThread(NULL, 0, resizer, NULL, 0, NULL);
                break;

            case DLL_PROCESS_DETACH:
                key = 0;
                switch_screen(false);
                change_cursor_visibility(true);
                #ifndef VT
                    change_stdin_visibility(true);
                #endif
                free_renderer_memory();
                break;
        }
        return TRUE;
    }

#else

    pthread_t thread_id;

    void * resizer(void *args)
    {
        while (key)
        {
            get_terminal_size();
            time_sleep(200);
        }
        return NULL;
    }

    __attribute__((constructor)) void ProcessAttach(void)
    {
        get_terminal_size();
        switch_screen(true);
        change_stdin_visibility(false);
        change_cursor_visibility(false);
        pthread_create(&thread_id, NULL, resizer, NULL);
    }

    __attribute__((destructor)) void ProcessDetach(void)
    {
        key = 0;
        pthread_join(thread_id, NULL);
        switch_screen(false);
        change_cursor_visibility(true);
        change_stdin_visibility(true);
        free_renderer_memory();
    }

#endif
