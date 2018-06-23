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
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>



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
int pattern = 0;
int speed = 1;
int size = 1;
int total_frames = 0;

enum {NONE, GLIDER, LWSS, RPENTOMINO, DIEHARD, ACORN, GOSPERGLIDERGUN, INFINITE1, INFINITE2, INFINITE3};
char glider[3][3] = {{0,0,1},
                     {1,0,1},
                     {0,1,1}};
char lwss[4][5] = {{0,1,1,0,0},
                   {1,1,0,1,1},
                   {0,1,1,1,1},
                   {0,0,1,1,0}};
char rpentomino[3][3] = {{0,1,1},
                         {1,1,0},
                         {0,1,0}};
char diehard[3][8] = {{0,0,0,0,0,0,1,0},
                      {1,1,0,0,0,0,0,0},
                      {0,1,0,0,0,1,1,1}};
char acorn[3][7] = {{0,1,0,0,0,0,0},
                    {0,0,0,1,0,0,0},
                    {1,1,0,0,1,1,1}};
char gosperglidergun[9][36] = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
                               {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0},
                               {0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
                               {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
                               {1,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                               {1,1,0,0,0,0,0,0,0,0,1,0,0,0,1,0,1,1,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0},
                               {0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0},
                               {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                               {0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};
char infinite1[6][8] = {{0,0,0,0,0,0,1,0},
                        {0,0,0,0,1,0,1,1},
                        {0,0,0,0,1,0,1,0},
                        {0,0,0,0,1,0,0,0},
                        {0,0,1,0,0,0,0,0},
                        {1,0,1,0,0,0,0,0}};
char infinite2[5][5] = {{1,1,1,0,1},
                        {1,0,0,0,0},
                        {0,0,0,1,1},
                        {0,1,1,0,1},
                        {1,0,1,0,1}};
char infinite3[1][39] = {1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,1,0,1,1,1,1,1};



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
        else if (pattern == GLIDER) {
            int x2 = mouse_x + 3;   // Glider X size
            int y2 = mouse_y + 3;   // Glider Y size

            x2 = x2 < SCR_WIDTH ? x2 : SCR_WIDTH;
            y2 = y2 < SCR_HEIGHT ? y2 : SCR_HEIGHT;
            for (int x = mouse_x, i = 0; x < x2; x++, i++)
                for (int y = mouse_y, j= 0; y < y2; y++, j++)
                    universe[x][y] = glider[j][i];
        }
        else if (pattern == LWSS) {
            int x2 = mouse_x + 5;   // LWSS X size
            int y2 = mouse_y + 4;   // LWSS Y size

            x2 = x2 < SCR_WIDTH ? x2 : SCR_WIDTH;
            y2 = y2 < SCR_HEIGHT ? y2 : SCR_HEIGHT;
            for (int x = mouse_x, i = 0; x < x2; x++, i++)
                for (int y = mouse_y, j= 0; y < y2; y++, j++)
                    universe[x][y] = lwss[j][i];
        }
        else if (pattern == RPENTOMINO) {
            int x2 = mouse_x + 3;   // R-Pentomino X size
            int y2 = mouse_y + 3;   // R-Pentomino Y size

            x2 = x2 < SCR_WIDTH ? x2 : SCR_WIDTH;
            y2 = y2 < SCR_HEIGHT ? y2 : SCR_HEIGHT;
            for (int x = mouse_x, i = 0; x < x2; x++, i++)
                for (int y = mouse_y, j= 0; y < y2; y++, j++)
                    universe[x][y] = rpentomino[j][i];
        }
        else if (pattern == DIEHARD) {
            int x2 = mouse_x + 8;   // Diehard X size
            int y2 = mouse_y + 3;   // Diehard Y size

            x2 = x2 < SCR_WIDTH ? x2 : SCR_WIDTH;
            y2 = y2 < SCR_HEIGHT ? y2 : SCR_HEIGHT;
            for (int x = mouse_x, i = 0; x < x2; x++, i++)
                for (int y = mouse_y, j= 0; y < y2; y++, j++)
                    universe[x][y] = diehard[j][i];
        }
        else if (pattern == ACORN) {
            int x2 = mouse_x + 7;   // Acorn X size
            int y2 = mouse_y + 3;   // Acorn Y size

            x2 = x2 < SCR_WIDTH ? x2 : SCR_WIDTH;
            y2 = y2 < SCR_HEIGHT ? y2 : SCR_HEIGHT;
            for (int x = mouse_x, i = 0; x < x2; x++, i++)
                for (int y = mouse_y, j= 0; y < y2; y++, j++)
                    universe[x][y] = acorn[j][i];
        }
        else if (pattern == GOSPERGLIDERGUN) {
            int x2 = mouse_x + 36;   // Gosper Glider Gun X size
            int y2 = mouse_y + 9;    // Gosper Glider Gun Y size

            x2 = x2 < SCR_WIDTH ? x2 : SCR_WIDTH;
            y2 = y2 < SCR_HEIGHT ? y2 : SCR_HEIGHT;
            for (int x = mouse_x, i = 0; x < x2; x++, i++)
                for (int y = mouse_y, j= 0; y < y2; y++, j++)
                    universe[x][y] = gosperglidergun[j][i];
        }
        else if (pattern == INFINITE1) {
            int x2 = mouse_x + 8;   // Infinite 1 X size
            int y2 = mouse_y + 6;   // Infinite 1 Y size

            x2 = x2 < SCR_WIDTH ? x2 : SCR_WIDTH;
            y2 = y2 < SCR_HEIGHT ? y2 : SCR_HEIGHT;
            for (int x = mouse_x, i = 0; x < x2; x++, i++)
                for (int y = mouse_y, j= 0; y < y2; y++, j++)
                    universe[x][y] = infinite1[j][i];
        }
        else if (pattern == INFINITE2) {
            int x2 = mouse_x + 5;   // Infinite 2 X size
            int y2 = mouse_y + 5;   // Infinite 2 Y size

            x2 = x2 < SCR_WIDTH ? x2 : SCR_WIDTH;
            y2 = y2 < SCR_HEIGHT ? y2 : SCR_HEIGHT;
            for (int x = mouse_x, i = 0; x < x2; x++, i++)
                for (int y = mouse_y, j= 0; y < y2; y++, j++)
                    universe[x][y] = infinite2[j][i];
        }
        else if (pattern == INFINITE3) {
            int x2 = mouse_x + 39;   // Infinite 3 X size
            int y2 = mouse_y + 1;    // Infinite 3 Y size

            x2 = x2 < SCR_WIDTH ? x2 : SCR_WIDTH;
            y2 = y2 < SCR_HEIGHT ? y2 : SCR_HEIGHT;
            for (int x = mouse_x, i = 0; x < x2; x++, i++)
                for (int y = mouse_y, j= 0; y < y2; y++, j++)
                    universe[x][y] = infinite3[j][i];
        }
    }
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
        for (int x = 0; x < SCR_WIDTH; x++) {
            for (int y = 0; y < SCR_HEIGHT; y++) {
                int count = 0;

            // Conteo de células
                if (y - 1 > -1) {
                    if ((x - 1 > -1) && (universe[x - 1][y - 1] & 1)) count++;
                    if (universe[x][y - 1] & 1) count++;
                    if ((x + 1 < SCR_WIDTH) && (universe[x + 1][y - 1] & 1)) count++;
                }
                if ((x - 1 > -1) && (universe[x - 1][y] & 1)) count++;
                if ((x + 1 < SCR_WIDTH) && (universe[x + 1][y] & 1)) count++;
                if (y + 1 < SCR_HEIGHT) {
                    if ((x - 1 > -1) && (universe[x - 1][y + 1] & 1)) count++;
                    if (universe[x][y + 1] & 1) count++;
                    if ((x + 1 < SCR_WIDTH) && (universe[x + 1][y + 1] & 1)) count++;
                }

            // Actualización de células
                if ((universe[x][y] & 1) && (count > 3 || count < 2))
                    universe[x][y] += 0;    // La célula se muere
                else if ((universe[x][y] & 1) && (count == 2 || count == 3))
                    universe[x][y] += 2;    // La célula continúa con vida
                else if (!(universe[x][y] & 1) && (count == 3))
                    universe[x][y] += 2;    // La célula nace
            }
        }
        for (int x = 0; x < SCR_WIDTH; x++)
            for (int y = 0; y < SCR_HEIGHT; y++)
                universe[x][y] >>= 1;
    }
    else if (event->any.source == al_get_keyboard_event_source()) {
        if (event->type == ALLEGRO_EVENT_KEY_UP) {
            if (event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                game_over = true;
            if (event->keyboard.keycode == ALLEGRO_KEY_SPACE)
                running = !running;
        }
        else if (event->type == ALLEGRO_EVENT_KEY_CHAR) {
            switch (event->keyboard.unichar) {
            case '+':
                size += size < 16 ? 1 : 0;
                break;
            case '-':
                size -= size > 1 ? 1 : 0;
                break;
            }
            switch (event->keyboard.keycode) {
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
            case ALLEGRO_KEY_1:
                pattern = GLIDER;
                break;
            case ALLEGRO_KEY_2:
                pattern = LWSS;
                break;
            case ALLEGRO_KEY_3:
                pattern = RPENTOMINO;
                break;
            case ALLEGRO_KEY_4:
                pattern = DIEHARD;
                break;
            case ALLEGRO_KEY_5:
                pattern = ACORN;
                break;
            case ALLEGRO_KEY_6:
                pattern = GOSPERGLIDERGUN;
                break;
            case ALLEGRO_KEY_7:
                pattern = INFINITE1;
                break;
            case ALLEGRO_KEY_8:
                pattern = INFINITE2;
                break;
            case ALLEGRO_KEY_9:
                pattern = INFINITE3;
                break;
            case ALLEGRO_KEY_0:
                pattern = NONE;
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
    for (int x = 0; x < SCR_WIDTH; x++)
        for (int y = 0; y < SCR_HEIGHT; y++)
            if (universe[x][y] == 1)
                al_draw_pixel(x, y, al_map_rgb(255, 255, 255));

    if (help) {
        al_draw_textf(font, al_map_rgb(255, 255, 255), 0,   0, 0, "• Usa el mouse para colocar células.");
        al_draw_textf(font, al_map_rgb(255, 255, 255), 0,  10, 0, "• Presiona Tab para mostrar/ocultar esta ayuda.");
        al_draw_textf(font, al_map_rgb(255, 255, 255), 0,  20, 0, "• Presiona Espacio para iniciar/detener la simulación.");
        al_draw_textf(font, al_map_rgb(255, 255, 255), 0,  30, 0, "• Presiona +/- para aumentar/reducir el tamaño del puntero del mouse: %d.", size);
        al_draw_textf(font, al_map_rgb(255, 255, 255), 0,  40, 0, "• Presiona R para activar/desactivar el modo de células aleatorias: %s.", shuffle ? "ON" : "OFF");
        al_draw_textf(font, al_map_rgb(255, 255, 255), 0,  50, 0, "• Presiona F para cambiar la velocidad de la simulación: x%d.", speed);
        al_draw_textf(font, al_map_rgb(255, 255, 255), 0,  60, 0, "• Presiona 1 para insertar GLIDERS: %s.", pattern == 1 ? "ON" : "OFF");
        al_draw_textf(font, al_map_rgb(255, 255, 255), 0,  70, 0, "• Presiona 2 para insertar LWSS-SPACESHIPS: %s.", pattern == 2 ? "ON" : "OFF");
        al_draw_textf(font, al_map_rgb(255, 255, 255), 0,  80, 0, "• Presiona 3 para insertar R-PENTOMINOS: %s.", pattern == 3 ? "ON" : "OFF");
        al_draw_textf(font, al_map_rgb(255, 255, 255), 0,  90, 0, "• Presiona 4 para insertar DIEHARDS: %s.", pattern == 4 ? "ON" : "OFF");
        al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 100, 0, "• Presiona 5 para insertar ACORNS: %s.", pattern == 5 ? "ON" : "OFF");
        al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 110, 0, "• Presiona 6 para insertar GOSPER GLIDER GUNS: %s.", pattern == 6 ? "ON" : "OFF");
        al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 120, 0, "• Presiona 7 para insertar INFINITE-1: %s.", pattern == 7 ? "ON" : "OFF");
        al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 130, 0, "• Presiona 8 para insertar INFINITE-2: %s.", pattern == 8 ? "ON" : "OFF");
        al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 140, 0, "• Presiona 9 para insertar INFINITE-3: %s.", pattern == 9 ? "ON" : "OFF");
        al_draw_textf(font, al_map_rgb(255, 255, 255), 0, 150, 0, "• Presiona 0 para volver al modo normal.");
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

