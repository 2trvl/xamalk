#include "../../include/events.h"

States eventsStates;
Event *events = NULL;
bool eventsMainloopKey = true;
bool getEventsGlobally = false;

#ifdef _WIN32
    
    /*
        Windows hasn't virtual code for NUMERIC_ENTER, 
        so it settled to VK_RETURN - common ENTER virtual code
    */

    unsigned char VIRTUAL_KEYS[94] = {
        VK_BACK, VK_TAB, VK_RETURN, VK_RETURN, VK_LSHIFT, VK_RSHIFT, VK_LCONTROL, VK_RCONTROL, VK_LMENU, 
        VK_RMENU, VK_PAUSE, VK_CAPITAL, VK_ESCAPE, VK_SPACE, VK_PRIOR, VK_NEXT, VK_END, VK_HOME, VK_LEFT, 
        VK_UP, VK_RIGHT, VK_DOWN, VK_SNAPSHOT, VK_INSERT, VK_DELETE, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 
        0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 
        0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, VK_NUMPAD0, 
        VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4, VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, 
        VK_NUMPAD9, VK_MULTIPLY, VK_ADD, VK_SUBTRACT, VK_DECIMAL, VK_DIVIDE, VK_F1, VK_F2, VK_F3, VK_F4, 
        VK_F5, VK_F6, VK_F7, VK_F8, VK_F9, VK_F10, VK_F11, VK_F12, VK_NUMLOCK, VK_OEM_PLUS, VK_OEM_COMMA, 
        VK_OEM_MINUS, VK_OEM_PERIOD, VK_OEM_3};
    
    void flush_console_input_buffer()
    {
        FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
    }

#else

    unsigned long get_console_window()
    {
        int envPid = getppid();  //  get pid of bash/zsh and get terminal pid
        #if __APPLE__
            proc_bsdinfo env_process_info;
            proc_pidinfo(envPid, PROC_PIDTBSDINFO, 0, &env_process_info, sizeof(env_process_info));
            return env_process_info.pbi_ppid;
        #elif __unix__
            unsigned long ppid;
            get_proc_stats_ppid(envPid, &ppid);
            return ppid;
        #endif
    }

    static int x11_error_handler(Display *display, XErrorEvent *error)
    {
        printf("X11 error with code was handled: %d", error->error_code);
        return 1;
    }

    unsigned long get_foreground_window()
    {
        int revert;
        Window window;
        XSetErrorHandler(x11_error_handler);
        Display *display = XOpenDisplay(NULL);
        XGetInputFocus(display, &window, &revert);

        Atom type;
        int format;
        unsigned char *propPID = 0;
        unsigned long nitems, bytesAfter, pid = 1;  //  1 is a unix init

        if (!XGetWindowProperty(display,
                                window,
                                XInternAtom(display, "_NET_WM_PID", False),
                                0,
                                1,
                                False,
                                XA_CARDINAL,
                                &type,
                                &format,
                                &nitems,
                                &bytesAfter,
                                &propPID))
        {
            if (propPID)
            {
                pid = *((unsigned long *)propPID);
                XFree(propPID);
            }
        }
        XCloseDisplay(display);
        return pid;
    }

    void flush_console_input_buffer()
    {
        tcflush(STDIN_FILENO, TCIFLUSH);
    }

    #if __APPLE__
        
        /*
            Pause, PrtSc, Insert, NmLk is undefined, settled to 0x36

            keys found in:
            /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks/
            Carbon.framework/Versions/A/Frameworks/HIToolbox.framework/Versions/A/Headers/Events.h
        */

        unsigned char VIRTUAL_KEYS[94] = {
            0x33, 0x30, 0x24, 0x4C, 0x38, 0x3C, 0x3B, 0x3E, 0x3A, 0x3D, 0x36, 0x39, 0x35, 0x31, 0x74, 0x79, 0x77, 
            0x73, 0x7B, 0x7E, 0x7C, 0x7D, 0x36, 0x36, 0x33, 0x1D, 0x12, 0x13, 0x14, 0x15, 0x17, 0x16, 0x1A, 0x1C, 
            0x19, 0x00, 0x0B, 0x08, 0x02, 0x0E, 0x03, 0x05, 0x04, 0x22, 0x26, 0x28, 0x25, 0x2E, 0x2D, 0x1F, 0x23, 
            0x0C, 0x0F, 0x01, 0x11, 0x20, 0x09, 0x0D, 0x07, 0x10, 0x06, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 
            0x59, 0x5B, 0x5C, 0x43, 0x45, 0x4E, 0x41, 0x4B, 0x7A, 0x78, 0x63, 0x76, 0x60, 0x61, 0x62, 0x64, 0x65, 
            0x6D, 0x67, 0x6F, 0x36, 0x18, 0x2B, 0x1B, 0x2F, 0x32};

        void get_async_key_state()
        {
            /*
                Hi, I didn't write this piece of code for macOS because 
                I don't know Objective-C or Swift and APIs for these languages enough to do it. 
                If you know and have free time to contribute to Xamalk, I will appreciate your help very much!
            */
        }

    #elif __unix__

        States keypresses;
        InputDevices keyboards = {0, NULL};

        /*
            keys found in:
            /linux/include/uapi/linux/input-event-codes.h
        */

        unsigned char VIRTUAL_KEYS[94] = {
            14, 15, 28, 96, 42, 54, 29, 97, 56, 100, 119, 58, 1, 57, 104, 109, 107, 102, 105, 103, 106, 108, 210, 110,
            111, 11, 2, 3, 4, 5, 6, 7, 8, 9, 10, 30, 48, 46, 32, 18, 33, 34, 35, 23, 36, 37, 38, 50, 49, 24, 25, 16,
            19, 31, 20, 22, 47, 17, 45, 21, 44, 82, 79, 80, 81, 75, 76, 77, 71, 72, 73, 55, 78, 74, 83, 98, 59, 60, 
            61, 62, 63, 64, 65, 66, 67, 68, 87, 88, 69, 13, 51, 12, 52, 41};

        void connect_keyboards()
        {
            disconnect_keyboards();

            char *path;
            regex_t pattern;
            regcomp(&pattern, ".*-event-kbd", 0);

            DIR *inputDirectory = opendir("/dev/input/by-path/");
            struct dirent *files = readdir(inputDirectory);

            keyboards.fileDescriptors = (int*)malloc(0);
            
            if (inputDirectory != NULL)
            {
                while (files != NULL)
                {
                    if (!regexec(&pattern, files->d_name, 0, NULL, 0))
                    {
                        ++keyboards.amount;

                        path = (char*)malloc(sizeof(char) * (strlen(files->d_name) + 20));
                        strncpy(path, "/dev/input/by-path/", sizeof(char) * 20);
                        strncat(path, files->d_name, sizeof(char) * strlen(files->d_name));

                        keyboards.fileDescriptors = realloc(keyboards.fileDescriptors, sizeof(int) * keyboards.amount);
                        keyboards.fileDescriptors[keyboards.amount-1] = open(path, O_RDONLY | O_NONBLOCK);
                        free(path);
                    }
                    files = readdir(inputDirectory);
                }
                closedir(inputDirectory);
            }

            else 
            {
                perror("xamalk.events: Cannot open /dev/input/by-path/, run program as root");
                exit(EXIT_FAILURE);
            }

            if (!keyboards.amount)
            {
                perror("xamalk.events: No keyboards found, connect a keyboard to your PC");
                exit(EXIT_FAILURE);
            }
        }

        void disconnect_keyboards()
        {
            if (keyboards.amount)
            {
                for (register unsigned char index = 0; index < keyboards.amount; ++index)
                    close(keyboards.fileDescriptors[index]);
                
                keyboards.amount = 0;
                free(keyboards.fileDescriptors);
                keyboards.fileDescriptors = NULL;
            }
        }

        void * system_input_thread(void *args)
        {
            keypresses.amount = 0;
            keypresses.reading = false;
            keypresses.lastPressedVirtualKeys = NULL;
            keypresses.status = (bool*)malloc(sizeof(bool) * 256);

            for (register unsigned short index = 0; index < 256; ++index)
                keypresses.status[index] = false;
            
            //  false will cause an error for python, so I'm using conditional expression
            unsigned char value = (eventsStates.reading==2) ? 1 : 0;
            //  disable reading flag if it was settled by events_mainloop
            set_states_reading_flag(value);

            InputEvent event;
            memset(&event, 0, sizeof(event));

            while (eventsMainloopKey)
            {
                for (register unsigned char index = 0; index < keyboards.amount; ++index)
                {
                    if (read(keyboards.fileDescriptors[index], &event, sizeof(event)) != -1)
                        if (event.type == EV_KEY)
                        {
                            if (event.value)
                                keypresses.status[event.code] = true;
                            else
                                keypresses.status[event.code] = false;
                        }
                }
            }

            while (!eventsStates.reading);     //  waiting while get_events_state is using, then free memory

            free(keypresses.status);
            keypresses.status = NULL;
            return NULL;
        }

        bool get_async_key_state(unsigned char code)
        {
            return keypresses.status[code];
        }

        static void get_proc_stats_ppid(int envPid, unsigned long *ppid)
        {
            int pathLength = (int)floor(log10(abs(envPid))) + 13;
            char *path = (char*)malloc(sizeof(char) * pathLength);
            snprintf(path, pathLength, "/proc/%d/stat", envPid);

            FILE *proc_stats = fopen(path, "r");
            free(path);

            if (!proc_stats)
            {
                perror("xamalk.events: Cannot open /proc/PID/stat, run program as root");
                exit(EXIT_FAILURE);
            }

            unsigned char spaces = 0;

            while (spaces < 3)
            {
                if (fgetc(proc_stats) == 32)
                    ++spaces;
            }

            fscanf(proc_stats, "%lld ", ppid);
            fclose(proc_stats);
        }

    #endif
#endif

void get_events_state()
{
    if (!eventsStates.reading)
    {
        bool buttonPressed = true;
        bool buttonReleased = false;
        bool releaseCondition = true;
        #ifdef _WIN32
            HWND consoleWindow = GetConsoleWindow();
        #else
            unsigned long consoleWindow = get_console_window();
        #endif
        register unsigned int index = 0, key = events[0].condition.amount;
        
        while (index < eventsStates.amount)
        {
            if (key > 0 && events[index].type)              //  Type of Event: Release
            {
                bool lastPressedValue = eventsStates.lastPressedVirtualKeys[index][key-1];
                
                //  current button pressed right now
                #ifdef _WIN32
                if (GetAsyncKeyState(events[index].condition.virtualKeys[key-1]) < 0 && 
                (consoleWindow == GetForegroundWindow() || getEventsGlobally))
                #else
                if (get_async_key_state(events[index].condition.virtualKeys[key-1]) &&
                (consoleWindow == get_foreground_window() || getEventsGlobally))
                #endif
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
                #ifdef _WIN32
                if (GetAsyncKeyState(events[index].condition.virtualKeys[key-1]) < 0 && 
                (consoleWindow == GetForegroundWindow() || getEventsGlobally))
                #else
                if (get_async_key_state(events[index].condition.virtualKeys[key-1]) &&
                (consoleWindow == get_foreground_window() || getEventsGlobally))
                #endif
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
                if (index < eventsStates.amount)
                    key = events[index].condition.amount;
            }
        }
    }
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

void exit_events_mainloop()
{
    eventsMainloopKey = false;
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
    
    #if !defined(_WIN32) && !defined(__APPLE__)
        pthread_t thread_id0;
        set_states_reading_flag(1);  //  to avoid conflicts
        pthread_create(&thread_id0, NULL, system_input_thread, NULL);
    #endif

    while (eventsMainloopKey)
    {
        get_events_state();
        execute_events_actions();
    }

    #if !defined(_WIN32) && !defined(__APPLE__)
        set_states_reading_flag(1);     //  send signal to system_input_thread that events_getter is done
        pthread_join(thread_id0, NULL);
    #endif
}

void set_events_listener_flag(bool globally)
{
    getEventsGlobally = globally;
}

void set_states_reading_flag(unsigned char reading)
{
    eventsStates.reading = reading;
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
