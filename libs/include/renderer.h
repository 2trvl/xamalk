#ifndef RENDERER_H
#define RENDERER_H

#if defined(_MSC_VER)
    #define EXPORT __declspec(dllexport)
#elif defined(__GNUC__)
    #define EXPORT __attribute__((visibility("default")))
#else
    #define EXPORT
#endif

#if defined(_WIN32) && defined(ANSI)
    #define ENABLE_ANSI enable_virtual_terminal_processing();
    #define CREATE_ALTERNATE_SCREEN_BUFFER 
#elif defined(_WIN32) && defined(VT)
    #define ENABLE_ANSI 
    #define CREATE_ALTERNATE_SCREEN_BUFFER create_alternate_screen_buffer();
#else
    #define ENABLE_ANSI 
    #define CREATE_ALTERNATE_SCREEN_BUFFER 
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef _WIN32
    #include <Windows.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <pthread.h>
    #include <sys/ioctl.h>

    #if _POSIX_C_SOURCE >= 199309L
        #include <time.h>
    #endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct _TerminalSize
{
    unsigned short cols;
    unsigned short rows;
} TerminalSize;

typedef struct _Coordinates
{
    unsigned short X;
    unsigned short Y;
} Coords;

typedef enum _Color
{
    RED,
    AQUA,
    BLUE,
    GRAY,
    BLACK,
    GREEN,
    WHITE,
    PURPLE,
    YELLOW,
    NO_COLOR = 14,
    LIGHT_RED = 9,
    LIGHT_BLUE = 10,
    LIGHT_GRAY = 11,
    LIGHT_GREEN = 12,
    LIGHT_PURPLE = 13
} Color;

typedef struct _Particle
{
    char *end;
    char *text;
    Color bgcode;
    Color fgcode;
    Coords *position;
    unsigned int milliseconds;
} Particle;

void get_terminal_size();
void free_renderer_memory();
void move_cursor(Coords *position);
void switch_screen(bool alternate);
void time_sleep(unsigned int milliseconds);
void change_stdin_visibility(bool visibility);
void change_cursor_visibility(bool visibility);

#ifdef _WIN32
    void enable_virtual_terminal_processing();
#endif

#ifdef ANSI
    extern char COLOR_CODES[2][15][6];
#endif

#ifdef VT
    extern unsigned char COLOR_CODES[2][15];

    void create_alternate_screen_buffer();
    void set_color(unsigned char bgcolor, unsigned char fgcolor);
#endif

EXPORT extern TerminalSize resolution;

EXPORT void clear_screen();
EXPORT void render_frame(unsigned int amount);
EXPORT void create_particles(unsigned int amount);
EXPORT void set_particle(Particle *particle, unsigned int index);
EXPORT void color_print(char *text, Color bgcode, Color fgcode, char *end);

#ifdef __cplusplus 
}
#endif

#endif
