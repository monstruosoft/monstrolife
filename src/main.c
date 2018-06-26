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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "patterns.h"



#define SCR_WIDTH   800
#define SCR_HEIGHT  600



// Allegro global variables
ALLEGRO_EVENT_QUEUE *events = NULL;
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_FONT *font = NULL;
ALLEGRO_EVENT event;
bool game_over = false;
bool redraw = true;

// Game global variables
uint8_t universe[800][600] = {0};
bool creation = false;
bool running = false;
bool shuffle = false;
bool help = true;
int population = 0;
int generation = 0;
int pattern = 0;
int speed = 1;
int size = 1;
int total_frames = 0;
ALLEGRO_COLOR black, white;
ALLEGRO_VERTEX *cells = NULL;
int vtx_count = 0;

// Patrones predefinidos
enum {NONE, GLIDER, LWSS, RPENTOMINO, DIEHARD, ACORN, GOSPERGLIDERGUN, INFINITE1, INFINITE2, INFINITE3, HALFMAX};
char *pattern_names[] = {"MODO NORMAL", "GLIDER", "LIGHTWEIGHT SPACESHIP", "R-PENTOMINO", "DIEHARD", "ACORN", 
                         "GOSPER GLIDER GUN", "INFINITE 1", "INFINITE 2", "INFINITE 3", "HALFMAX"};
MONSTRO_LIFE_PATTERN *patterns[] = {NULL, &glider, &lwss, &rpentomino, &diehard, &acorn, &gosperglidergun, 
                                    &infinite1, &infinite2, &infinite3, &halfmax};



void add_pattern(int mouse_x, int mouse_y, int w, int h, char data[h][w]) {
    int x2 = mouse_x + w;   // Pattern width
    int y2 = mouse_y + h;   // Pattern height

    x2 = x2 < SCR_WIDTH ? x2 : SCR_WIDTH;
    y2 = y2 < SCR_HEIGHT ? y2 : SCR_HEIGHT;
    for (int x = mouse_x, i = 0; x < x2; x++, i++)
        for (int y = mouse_y, j= 0; y < y2; y++, j++)
            universe[x][y] = data[j][i];
}



inline void populate_cells(int x, int y) {
    population++;
    if (population > vtx_count) {
        ALLEGRO_VERTEX *prev_cells = cells;
        int prev_count = vtx_count;

        printf("Population: %d, Vertex count: %d\n", population, vtx_count);
        printf("Increasing vertex count to %d...\n", vtx_count * 2);
        vtx_count *= 2;
        cells = malloc(vtx_count * sizeof(ALLEGRO_VERTEX));
        assert(cells);
        memcpy(cells, prev_cells, prev_count * sizeof(ALLEGRO_VERTEX));
        free(prev_cells);
    }
    cells[population - 1] = (ALLEGRO_VERTEX){.x = x, .y = y, .color = white};
}



void add_cell(int mouse_x, int mouse_y) {
    bool x_valid = (mouse_x > -1 && mouse_x < SCR_WIDTH) ? true : false;
    bool y_valid = (mouse_y > -1 && mouse_y < SCR_HEIGHT) ? true : false;
    if (creation && x_valid && y_valid) {
        if (size == 1 && pattern == NONE)
            universe[mouse_x][mouse_y] = 1;
        else if (size > 1 && pattern == NONE) {
            int offset = size / 2;
            int x1 = mouse_x - offset;
            int x2 = mouse_x + offset;
            int y1 = mouse_y - offset;
            int y2 = mouse_y + offset;

            x1 = x1 > -1 ? x1 : 0;
            x2 = x2 < SCR_WIDTH ? x2 : SCR_WIDTH;
            y1 = y1 > -1 ? y1 : 0;
            y2 = y2 < SCR_HEIGHT ? y2 : SCR_HEIGHT;
            for (int x = x1; x < x2; x++)
                for (int y = y1; y < y2; y++)
                    if (!shuffle) universe[x][y] = 1;
                    else universe[x][y] = rand() % 2;
        }
        else {
            MONSTRO_LIFE_PATTERN p = *patterns[pattern];
            add_pattern(mouse_x, mouse_y, p.w, p.h, (char(*)[])p.data);
        }
    }

    population = 0;
    for (int x = 0; x < SCR_WIDTH; x++)
        for (int y = 0; y < SCR_HEIGHT; y++)
            if (universe[x][y] & 1)
                populate_cells(x, y);
}



/*
 * Game logic.
 */
void logic(ALLEGRO_EVENT *event) {
    if (event->type == ALLEGRO_EVENT_TIMER) {
        if (al_is_event_queue_empty(events))
            redraw = true;
        total_frames++;

        if (!running) return;

        population = 0;
        generation++;
        for (int y = 0; y < SCR_HEIGHT; y++) {
            bool py, ny;
            int count = 0, counta = 0, countb = 0, countc = 0;

            py = y - 1 > -1;             // Check if previous Y position (Y - 1) is valid
            ny = y + 1 < SCR_HEIGHT;     // Check if next Y position (Y + 1) is valid
            for (int x = 0; x < SCR_WIDTH; x++) {
                bool nx, current;
            // Conteo de células
                nx = x + 1 < SCR_WIDTH;         // Check if next X position (X + 1) is valid
                current = universe[x][y] & 1;   // Check the state of the current (X, Y) cell
                if (x == 0) {
                    if (py && (universe[x][y - 1] & 1)) countb++;
                    if (current) countb++;
                    if (ny && (universe[x][y + 1] & 1)) countb++;
                }
                if (py && nx && (universe[x + 1][y - 1] & 1)) countc++;
                if (nx && (universe[x + 1][y] & 1)) countc++;
                if (ny && nx && (universe[x + 1][y + 1] & 1)) countc++;

                count = counta + countb + countc - current;

            // Actualización de células
                if (current && (count > 3 || count < 2))
                    universe[x][y] += 0;    // La célula se muere
                else if (current && (count == 2 || count == 3)) {
                    universe[x][y] += 2;    // La célula continúa con vida
                    populate_cells(x, y);
                }
                else if (!current && (count == 3)) {
                    universe[x][y] += 2;    // La célula nace
                    populate_cells(x, y);
                }

                counta = countb;
                countb = countc;
                countc = 0;
            }
        }
        for (int x = 0; x < SCR_WIDTH; x++)
            for (int y = 0; y < SCR_HEIGHT; y++)
                universe[x][y] >>= 1;
    }
    else if (event->any.source == al_get_keyboard_event_source()) {
        if (event->type == ALLEGRO_EVENT_KEY_CHAR) {
            switch (event->keyboard.unichar) {
            case '+':
                size += size < 64 ? 1 : 0;
                break;
            case '-':
                size -= size > 1 ? 1 : 0;
                break;
            }

            switch (event->keyboard.keycode) {
            case ALLEGRO_KEY_ESCAPE:
                game_over = true;
                break;
            case ALLEGRO_KEY_SPACE:
                running = !running;
                break;
            case ALLEGRO_KEY_TAB:
                help = !help;
                break;
            case ALLEGRO_KEY_R:
                shuffle = !shuffle;
                break;
            case ALLEGRO_KEY_F:
                speed = speed < 4 ? speed + 1 : 1;
                al_set_timer_speed(timer, ALLEGRO_BPS_TO_SECS(speed * 1.5 * 10));
                break;
            case ALLEGRO_KEY_P:
                pattern = pattern < HALFMAX ? pattern + 1 : 0;
                break;
            }
        }
    }
    else if (event->any.source == al_get_mouse_event_source()) {
        if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && event->mouse.button == 1) {
            creation = true;
            add_cell(event->mouse.x, event->mouse.y);
        }
        else if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_UP && event->mouse.button == 1)
            creation = false;
        else if (event->type == ALLEGRO_EVENT_MOUSE_AXES)
            add_cell(event->mouse.x, event->mouse.y);
    }
}



/*
 * Screen update.
 */
void update() {
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_draw_prim(cells, NULL, NULL, 0, population, ALLEGRO_PRIM_POINT_LIST);

    if (help) {
        al_draw_textf(font, white, 0,   0, 0, "• Usa el mouse para colocar células.");
        al_draw_textf(font, white, 0,  10, 0, "• Presiona Tab para mostrar/ocultar esta ayuda.");
        al_draw_textf(font, white, 0,  20, 0, "• Presiona Espacio para iniciar/detener la simulación.");
        al_draw_textf(font, white, 0,  30, 0, "• Presiona +/- para aumentar/reducir el tamaño del puntero del mouse: %d.", size);
        al_draw_textf(font, white, 0,  40, 0, "• Presiona R para activar/desactivar el modo de células aleatorias: %s.", shuffle ? "ON" : "OFF");
        al_draw_textf(font, white, 0,  50, 0, "• Presiona F para cambiar la velocidad de la simulación: x%d.", speed);
        al_draw_textf(font, white, 0,  60, 0, "• Presiona P para cambiar el patrón: %s.", pattern_names[pattern]);
        al_draw_textf(font, white, 0,  70, 0, "Generación: %d.", generation);
        al_draw_textf(font, white, 0,  80, 0, "Población: %d.", population);
    }
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
    assert(al_init_primitives_addon());
    assert(al_init_font_addon());
    assert(al_init_ttf_addon());

    events = al_create_event_queue();
    assert(events);
    timer = al_create_timer(ALLEGRO_BPS_TO_SECS(10));
    assert(timer);
    al_start_timer(timer);
    al_register_event_source(events, al_get_keyboard_event_source());
    al_register_event_source(events, al_get_mouse_event_source());
    al_register_event_source(events, al_get_timer_event_source(timer));
    font = al_load_ttf_font("font/ArchivoNarrow-Bold.otf", 10, 0);
    assert(font);
    
    srand(time(NULL));

// Game initialization
    black = al_map_rgb(0, 0, 0);
    white = al_map_rgb(255, 255, 255);
    vtx_count = 1024;
    cells = malloc(vtx_count * sizeof(ALLEGRO_VERTEX));
    assert(cells);
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

