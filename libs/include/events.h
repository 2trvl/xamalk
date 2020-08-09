#ifndef EVENTS_H
#define EVENTS_H

#if defined(_MSC_VER)
    #define EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
    #define EXPORT __attribute__((visibility("default")))
#else
    #define EXPORT
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#if defined(_WIN32)
    #include <Windows.h>
#elif defined(__unix__)
    //#include
#elif defined(__APPLE__)
    //#include
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
    bool reading;
    bool *status;
    unsigned char amount;
    bool **lastPressedVirtualKeys;
} States;

typedef enum _Key
{
    LEFT_MOUSE_BUTTON,
    RIGHT_MOUSE_BUTTON,
    MIDDLE_MOUSE_BUTTON,
    BACKSPACE,
    TAB,
    ENTER,
    SHIFT,
    CTRL,
    ALT,
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
    NUMPAD_SEPARATOR,
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

extern bool eventsMainloopKey;
extern unsigned char VIRTUAL_KEYS[94];

void get_events_state();
void free_events_memory();
void exit_events_mainloop();
void execute_events_actions();
void events_mainloop(Shortcut *quit);

#ifdef _WIN32
    DWORD WINAPI get_input_characters(void *data);
#endif

EXPORT extern States eventsStates;

EXPORT void set_states_reading_flag(bool flag);  //  function only needed for python
EXPORT void create_events(unsigned char amount);
EXPORT void bind_event(Shortcut *condition, EventType type, void (*action)(void*), void *actionArguments, unsigned char index);

#ifdef __cplusplus 
}
#endif

#endif
