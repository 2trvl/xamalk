#ifndef EVENTS_H
#define EVENTS_H

#if defined(_MSC_VER)
    #define EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
    #define EXPORT __attribute__((visibility("default")))
#else
    #define EXPORT
#endif

#if defined(__unix__) && !defined(__APPLE__)
    #define CONNECT_KEYBOARDS connect_keyboards();
    #define DISCONNECT_KEYBOARDS disconnect_keyboards();
#else
    #define CONNECT_KEYBOARDS 
    #define DISCONNECT_KEYBOARDS 
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef _WIN32
    #include <Windows.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <pthread.h>
    #include <X11/Xlib.h>
    #include <X11/Xatom.h>
    #include <sys/types.h>

    #if __APPLE__
        #include <libproc.h>
        #include <Carbon/Carbon.h>
    #elif __unix__
        #include <math.h>
        #include <fcntl.h>
        #include <regex.h>
        #include <dirent.h>
        #include <stdint.h>
        #include <string.h>
        #include <sys/stat.h>
    #endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum _EventType
{
    Press, 
    Release
} EventType;

typedef struct _Shortcut
{
    unsigned char amount;
    unsigned char *virtualKeys;
} Shortcut;

typedef struct _Event
{
    EventType type;
    Shortcut condition;
    void *actionArguments;
    void (*action)(void*);
} Event;

typedef struct _States
{
    bool *status;
    unsigned char amount;
    unsigned char reading;
    bool **lastPressedVirtualKeys;
} States;

typedef enum _Key
{
    BACKSPACE,
    TAB,
    TYPEWRITER_ENTER,
    NUMPAD_ENTER,
    LEFT_SHIFT,
    RIGHT_SHIFT,
    LEFT_CTRL,
    RIGHT_CTRL,
    LEFT_ALT,
    RIGHT_ALT,
    PAUSE,
    CAPS_LOCK,
    ESCAPE,
    SPACE,
    PAGE_UP,
    PAGE_DOWN,
    END,
    HOME,
    LEFT_ARROW,
    UP_ARROW,
    RIGHT_ARROW,
    DOWN_ARROW,
    PRINT_SCREEN,
    INSERT,
    DEL,
    TYPEWRITER_0,
    TYPEWRITER_1,
    TYPEWRITER_2,
    TYPEWRITER_3,
    TYPEWRITER_4,
    TYPEWRITER_5,
    TYPEWRITER_6,
    TYPEWRITER_7,
    TYPEWRITER_8,
    TYPEWRITER_9,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    NUMPAD_0,
    NUMPAD_1,
    NUMPAD_2,
    NUMPAD_3,
    NUMPAD_4,
    NUMPAD_5,
    NUMPAD_6,
    NUMPAD_7,
    NUMPAD_8,
    NUMPAD_9,
    NUMPAD_MULTIPLY,
    NUMPAD_ADD,
    NUMPAD_SUBTRACT,
    NUMPAD_DOT,
    NUMPAD_DIVIDE,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    NUM_LOCK,
    TYPEWRITER_ADD,
    COMMA,
    TYPEWRITER_SUBTRACT,
    TYPEWRITER_DOT,
    TILDE
} Key;

#if defined(__unix__) && !defined(__APPLE__)

    //  press and release events
    #define EV_KEY 0x01

    typedef struct _Timeval
    {
        long tv_sec;
        long tv_usec;
    } Timeval;

    typedef struct _InputEvent
    {
        Timeval time;
        uint16_t type;
        uint16_t code;
        int32_t value;
    } InputEvent;

    typedef struct _InputDevices
    {
        unsigned char amount;
        int *fileDescriptors;
    } InputDevices;

    void connect_keyboards();
    void disconnect_keyboards();
    void * system_input_thread(void *args);
    static void get_proc_stats_ppid(int envPid, unsigned long *ppid);

#endif

extern bool getEventsGlobally;
extern bool eventsMainloopKey;
extern unsigned char VIRTUAL_KEYS[94];

void get_events_state();
void free_events_memory();
void exit_events_mainloop();
void execute_events_actions();
void flush_console_input_buffer();
void events_mainloop(Shortcut *quit);

#ifndef _WIN32
    unsigned long get_console_window();
    unsigned long get_foreground_window();
    bool get_async_key_state(unsigned char code);
    static int x11_error_handler(Display *display, XErrorEvent *error);
#endif

EXPORT extern States eventsStates;

//  functions only needed for python
EXPORT void set_events_listener_flag(bool globally);
EXPORT void set_states_reading_flag(unsigned char reading);

EXPORT void create_events(unsigned char amount);
EXPORT void bind_event(Shortcut *condition, EventType type, void (*action)(void*), void *actionArguments, unsigned char index);

#ifdef __cplusplus 
}
#endif

#endif
