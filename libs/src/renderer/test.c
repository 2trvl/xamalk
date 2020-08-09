#include "../../include/renderer.h"

int main(void)
{
    ENABLE_ANSI
    CREATE_ALTERNATE_SCREEN_BUFFER
    switch_screen(true);
    change_cursor_visibility(false);
    get_terminal_size();

    char *text = "Hello there!";
    char *end = "\n";

    color_print(text, WHITE, RED, end);
    color_print(text, AQUA, NO_COLOR, end);
    color_print(text, NO_COLOR, RED, end);
    color_print(text, NO_COLOR, NO_COLOR, end);
    color_print("Without any color", NO_COLOR, NO_COLOR, end);
    printf("Current resolution: (%d, %d)\n", resolution.cols, resolution.rows);

    Coords positions[2];
    positions[0].X = 12;
    positions[0].Y = 10;
    
    color_print(text, WHITE, RED, end);
    move_cursor(&positions[0]);
    color_print(text, WHITE, RED, end);
    
    time_sleep(2000);
    clear_screen();

    create_particles(2);
    Particle particles[2];

    particles[0].text = "   ";
    particles[0].bgcode = AQUA;
    particles[0].fgcode = NO_COLOR;
    particles[0].end = "";

    positions[0].X = 0;
    positions[0].Y = 0;
    particles[0].position = &positions[0];

    particles[0].milliseconds = 0;

    set_particle(&particles[0], 0);

    particles[1].text = "   ";
    particles[1].bgcode = AQUA;
    particles[1].fgcode = NO_COLOR;
    particles[1].end = "\n";

    positions[1].X = 12;
    positions[1].Y = 5;
    particles[1].position = &positions[1];
    
    particles[1].milliseconds = 1000;

    set_particle(&particles[1], 1);

    render_frame(2);
    switch_screen(false);
    change_cursor_visibility(true);
    free_renderer_memory();
    return 0;
}
