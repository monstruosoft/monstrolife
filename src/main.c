/**
 * @file main.c
 * 
 * @section LICENSE License
 * 
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * For more information, please refer to <https://unlicense.org>
 * 
 * @section DESCRIPTION Description
 * 
 * Game loop.
 */

#include <stdint.h>
#include <string.h>
#include <allegro5/allegro.h>



#define SCR_WIDTH   800
#define SCR_HEIGHT  600



// Allegro global variables
ALLEGRO_EVENT_QUEUE *events = NULL;
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_EVENT event;
bool game_over = false;
bool redraw = true;

// Game global variables
uint8_t universe[800][600] = {0};
bool creation = false;
bool running = false;
int total_frames = 0;



/*
 * Game logic.
 */
void logic(ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_TIMER) {
        if (al_is_event_queue_empty(events))
            redraw = true;
        total_frames++;

        if (!running) return;
        for (int x = 0; x < SCR_WIDTH; x++)
            for (int y = 0; y < SCR_HEIGHT; y++) {
                int count = 0;

            // Conteo de células
                if (y - 1 > -1) {
                    if ((x - 1 > -1) && (universe[x - 1][y - 1] == 1)) count++;
                    if (universe[x][y - 1] == 1) count++;
                    if ((x + 1 < SCR_WIDTH) && (universe[x + 1][y - 1] == 1)) count++;
                }
                if ((x - 1 > -1) && (universe[x - 1][y] == 1)) count++;
                if ((x + 1 < SCR_WIDTH) && (universe[x + 1][y] == 1)) count++;
                if (y + 1 < SCR_HEIGHT) {
                    if ((x - 1 > -1) && (universe[x - 1][y + 1] == 1)) count++;
                    if (universe[x][y + 1] == 1) count++;
                    if ((x + 1 < SCR_WIDTH) && (universe[x + 1][y + 1] == 1)) count++;
                }

            // Actualización de células
                if (universe[x][y] == 1 && (count > 3 || count < 2))
                    universe[x][y] = 0;
                else if (universe[x][y] == 0 && count == 3)
                    universe[x][y] = 1;
            }
    }
    else if (event->any.source == al_get_keyboard_event_source()) {
        if (event->type == ALLEGRO_EVENT_KEY_UP) {
            if (event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                game_over = true;
            if (event->keyboard.keycode == ALLEGRO_KEY_SPACE)
                running = !running;
        }
    }
    else if (event->any.source == al_get_mouse_event_source()) {
        if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && event->mouse.button == 1)
            creation = true;
        else if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && event->mouse.button == 1)
            creation = false;
        else if (event->type == ALLEGRO_EVENT_MOUSE_AXES) {
            bool x_valid = (event->mouse.x > -1 && event->mouse.x < SCR_WIDTH) ? true : false;
            bool y_valid = (event->mouse.y > -1 && event->mouse.y < SCR_HEIGHT) ? true : false;
            if (creation && x_valid && y_valid)
                universe[event->mouse.x][event->mouse.y] = 1;
        }
    }
}



/*
 * Screen update.
 */
void update() {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    for (int x = 0; x < SCR_WIDTH; x++)
        for (int y = 0; y < SCR_HEIGHT; y++)
            if (universe[x][y] == 1)
                al_draw_pixel(x, y, al_map_rgb(255, 255, 255));
}



/*
 * Game initialization.
 */
void initialization() {
// Allegro initialization
    assert(al_init());
    assert(al_install_keyboard());
    assert(al_install_mouse());
    al_set_new_display_flags(ALLEGRO_WINDOWED);
    al_set_new_window_title("El juego de la muerte de NK");
    display = al_create_display(SCR_WIDTH, SCR_HEIGHT);
    assert(display);
    // assert(al_init_primitives_addon());
    // assert(al_init_font_addon());
    // assert(al_init_ttf_addon());

    events = al_create_event_queue();
    assert(events);
    timer = al_create_timer(ALLEGRO_BPS_TO_SECS(10));
    assert(timer);
    al_start_timer(timer);
    al_register_event_source(events, al_get_keyboard_event_source());
    al_register_event_source(events, al_get_mouse_event_source());
    al_register_event_source(events, al_get_timer_event_source(timer));
    
    srand(time(NULL));

// Game initialization
}



/*
 * Game loop.
 */
int main() {
    initialization();
    
    while (!game_over) {
        al_wait_for_event(events, &event);
        logic(&event);
        if (redraw && al_is_event_queue_empty(events)) {
            update();
            al_flip_display();
            redraw = false;
        }  
    }
}

