#include "../../include/renderer.h"

TerminalSize resolution;
Particle *currentFrame = NULL;

#if defined(VT)

    int primaryStdIn;
    FILE *alternateStdIn;

    int primaryStdOut;
    FILE *alternateStdOut;

    HANDLE hPrimaryConsoleOutput;
    HANDLE hAlternateConsoleOutput;

    unsigned char COLOR_CODES[2][15] = {
        { 0x40, 0x30, 0x10, 0x80, 0x0, 0x20, 0xF0, 0x50, 0x60, 0xC0, 0x90, 0x70, 0xA0, 0xD0, 0x0 },    //  Background
        { 0x4, 0x3, 0x1, 0x8, 0x0, 0x2, 0xF, 0x5, 0x6, 0xC, 0x9, 0x7, 0xA, 0xD, 0x7 }};                //  Foreground

    void clear_screen()
    {
        DWORD cCharsWritten;
        COORD coordScreen = {0, 0};
        FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), 
                                   (TCHAR) ' ',
                                   resolution.cols * resolution.rows, 
                                   coordScreen, 
                                   &cCharsWritten);
        FillConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
                                   0x0 | 0x7,
                                   resolution.cols * resolution.rows,
                                   coordScreen,
                                   &cCharsWritten);
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordScreen);
    }

    void move_cursor(Coords *position)
    {
        COORD dwCursorPosition;
        dwCursorPosition.X = position->X;
        dwCursorPosition.Y = position->Y;
        HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleCursorPosition(hConsoleOutput, dwCursorPosition);
    }

    void switch_screen(bool alternate)
    {
        if (alternate)
        {
            //  activate alternate screen buffer and enable vertical scroll bar
            SetConsoleActiveScreenBuffer(hAlternateConsoleOutput);
            SetStdHandle(STD_OUTPUT_HANDLE, hAlternateConsoleOutput);
            ShowScrollBar(GetConsoleWindow(), SB_VERT, TRUE);
            EnableScrollBar(GetConsoleWindow(), SB_VERT, ESB_DISABLE_BOTH);
            //  redirect standard streams
            primaryStdIn = _dup(_fileno(stdin));
            primaryStdOut = _dup(_fileno(stdout));
            alternateStdIn = freopen("CONIN$", "w", stdin);
            alternateStdOut = freopen("CONOUT$", "w", stdout);
        }
        else
        {
            //  deactivate alternate screen buffer
            time_sleep(1000);
            SetConsoleActiveScreenBuffer(hPrimaryConsoleOutput);
            SetStdHandle(STD_OUTPUT_HANDLE, hPrimaryConsoleOutput);
            //  stop redirecting standard streams
            _dup2(primaryStdIn, _fileno(stdin));
            _dup2(primaryStdOut, _fileno(stdout));
            _close(alternateStdIn);
            _close(alternateStdOut);
        }
    }

    void change_cursor_visibility(bool visibility)
    {
        CONSOLE_CURSOR_INFO lpConsoleCursorInfo;
        lpConsoleCursorInfo.dwSize = 100;
        if (visibility)
            lpConsoleCursorInfo.bVisible = TRUE;
        else
            lpConsoleCursorInfo.bVisible = FALSE;
        SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &lpConsoleCursorInfo);
    }

    void create_alternate_screen_buffer()
    {
        //  assign values to screen buffers handlers
        hPrimaryConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        hAlternateConsoleOutput = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 
                                                            FILE_SHARE_WRITE | FILE_SHARE_READ, 
                                                            NULL, 
                                                            CONSOLE_TEXTMODE_BUFFER, 
                                                            NULL);
        //  copy primary screen buffer size to the alternate screen buffer
        CONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo;
        GetConsoleScreenBufferInfo(hPrimaryConsoleOutput, &lpConsoleScreenBufferInfo);
        lpConsoleScreenBufferInfo.dwMaximumWindowSize.Y += 1;
        SetConsoleScreenBufferSize(hAlternateConsoleOutput, lpConsoleScreenBufferInfo.dwMaximumWindowSize);
        SMALL_RECT lpConsoleWindow = {0, 0,
                                      lpConsoleScreenBufferInfo.dwMaximumWindowSize.X,
                                      lpConsoleScreenBufferInfo.dwMaximumWindowSize.Y};
        SetConsoleWindowInfo(hAlternateConsoleOutput, TRUE, &lpConsoleWindow);
    }

    void set_color(unsigned char bgcolor, unsigned char fgcolor)
    {
        WORD wAttributes = bgcolor | fgcolor;
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), wAttributes);
    }

    void color_print(char *text, Color bgcode, Color fgcode, char *end)
    {
        set_color(COLOR_CODES[0][bgcode], COLOR_CODES[1][fgcode]);
        printf(text);
        set_color(0x0, 0x7);
        printf(end);
    }

#elif defined(ANSI)

    bool renderMode = false;

    char COLOR_CODES[2][15][6] = {
        { "0;41m", "0;46m", "0;44m", "1;40m", "0;40m", "0;42m", "1;47m", "0;45m", "1;43m", "1;41m", "1;44m", "0;47m", "1;42m", "1;45m", "\0\0\0\0\0" },    //  Background
        { "0;31m", "0;36m", "0;34m", "1;30m", "0;30m", "0;32m", "1;37m", "0;35m", "1;33m", "1;31m", "1;34m", "0;37m", "1;32m", "1;35m", "\0\0\0\0\0" }};   //  Foreground

    void clear_screen()
    {
        printf("\033[2J\033[H");
    }

    void move_cursor(Coords *position)
    {
        printf("\033[%d;%dH", position->Y+1, position->X+1);
    }

    void switch_screen(bool alternate)
    {
        if (alternate)
            printf("\033[?1049h");
        else
        {
            time_sleep(1000);
            printf("\033[2J\033[H\033[?1049l");
        }
    }

    void change_cursor_visibility(bool visibility)
    {
        if (visibility)
            printf("\033[?25h");
        else
            printf("\033[?25l");
    }

    char * color_print(char *text, Color bgcode, Color fgcode, char *end)
    {
        unsigned int length;
        length = (unsigned int)(strlen(text) + strlen(end));

        char *string;
        string = (char*)malloc(length+17);

        char bgcolor[6];
        strncpy(bgcolor, COLOR_CODES[0][bgcode], sizeof(char) * 6);
        
        char fgcolor[6];
        strncpy(fgcolor, COLOR_CODES[1][fgcode], sizeof(char) * 6);

        if (fgcolor[0] != '\0')
        {
            snprintf(string, 7, "\033[%s", fgcolor);
            if (bgcolor[0] != '\0')
            {
                snprintf(&string[6], length+11, ";%s%s\033[0m%s", bgcolor, text, end);
            }
            else
                snprintf(&string[6], length+6, "m%s\033[0m%s", text, end);
        }

        else if (bgcolor[0] != '\0')
            snprintf(string, length+12, "\033[%s%s\033[0m%s", bgcolor, text, end);

        else
            snprintf(string, length+1, "%s%s", text, end);

        if (renderMode)
            return string;

        printf(string);
        free(string);
        return NULL;
    }

#endif

#ifdef _WIN32

    void time_sleep(unsigned int milliseconds)
    {
        Sleep(milliseconds);
    }

    void get_terminal_size()
    {
        CONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo;
        #ifdef VT
        if (GetConsoleScreenBufferInfo(hAlternateConsoleOutput, &lpConsoleScreenBufferInfo))
        #else
        if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &lpConsoleScreenBufferInfo))
        #endif
        {
            resolution.cols = lpConsoleScreenBufferInfo.srWindow.Right - lpConsoleScreenBufferInfo.srWindow.Left + 1;
            resolution.rows = lpConsoleScreenBufferInfo.srWindow.Bottom - lpConsoleScreenBufferInfo.srWindow.Top + 1;
        }
    }

    void enable_virtual_terminal_processing()
    {
        DWORD dwMode = 0x0;
        GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &dwMode);
        dwMode |= 0x4;
        SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), dwMode);
    }

#else

    void time_sleep(unsigned int milliseconds)
    {
        #if _POSIX_C_SOURCE >= 199309L
            struct timespec ts;
            ts.tv_sec = milliseconds / 1000;
            ts.tv_nsec = (milliseconds % 1000) * 1000000;
            nanosleep(&ts, NULL);
        #else
            usleep(milliseconds * 1000);
        #endif
    }

    void get_terminal_size()
    {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        resolution.cols = w.ws_col;
        resolution.rows = w.ws_row;
    }

#endif

void free_renderer_memory()
{
    if (currentFrame != NULL)
    {
        free(currentFrame);
        currentFrame = NULL;
    }
    #ifdef VT
        if (GetStdHandle(STD_OUTPUT_HANDLE) != hAlternateConsoleOutput)
            CloseHandle(hAlternateConsoleOutput);
    #endif
}

void create_particles(unsigned int amount)
{
    free_renderer_memory();
    currentFrame = (Particle*)malloc(sizeof(Particle) * amount);
}

void set_particle(Particle *particle, unsigned int index)
{
    Particle *currentParticle;
    currentParticle = currentFrame + index;

    currentParticle->end = particle->end;
    currentParticle->text = particle->text;

    currentParticle->bgcode = particle->bgcode;
    currentParticle->fgcode = particle->fgcode;

    currentParticle->position = particle->position;
    currentParticle->milliseconds = particle->milliseconds;
}

void render_frame(unsigned int amount)
{
    #ifdef ANSI
        char *string;
        renderMode = true;
    #endif
    Particle *currentParticle;
    for (register unsigned int index = 0; index < amount; ++index)
    {
        currentParticle = currentFrame + index;
        time_sleep(currentParticle->milliseconds);
        move_cursor(currentParticle->position);
        #if defined(ANSI)
            string = color_print(currentParticle->text,
                                 currentParticle->bgcode, 
                                 currentParticle->fgcode,
                                 currentParticle->end);
            printf(string);
            free(string);
        #elif defined(VT)
            color_print(currentParticle->text, 
                        currentParticle->bgcode, 
                        currentParticle->fgcode,
                        currentParticle->end);
        #endif
    }
    #ifdef ANSI
        renderMode = false;
    #endif
}
