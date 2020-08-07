#include "../../include/renderer.h"

unsigned char key = 1;

#ifdef _WIN32

    DWORD WINAPI Resizer(void *data) 
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
                switch_screen(true);
                change_cursor_visibility(false);
                get_terminal_size();
                HANDLE thread = CreateThread(NULL, 0, Resizer, NULL, 0, NULL);
                break;

            case DLL_PROCESS_DETACH:
                key = 0;
                switch_screen(false);
                change_cursor_visibility(true);
                free_renderer_memory();
                break;
        }
        return TRUE;
    }

#else

    pthread_t thread_id;

    void * Resizer(void *vargp)
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
        switch_screen(true);
        change_cursor_visibility(false);
        get_terminal_size();
        pthread_create(&thread_id, NULL, Resizer, NULL);
    }

    __attribute__((destructor)) void ProcessDetach(void)
    {
        key = 0;
        pthread_join(thread_id, NULL);
        switch_screen(false);
        change_cursor_visibility(true);
        free_renderer_memory();
    }

#endif
