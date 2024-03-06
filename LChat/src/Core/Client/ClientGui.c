#include "Core/Client/ClientGui.h"
#include <ctype.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "Core/Client/Client.h"
#include "Core/Mensajes.h"
#include "Core/gui.h"
#include "Utils/clog.h"
#include "curses.h"

#define APP_TITLE "Latin Chat Server"
#define CHAT_TTILE " Chat:"
#define INPUT_TITLE " Input: (Para empezar a a escribirt presiona la tecla 'w')"

typedef enum { INPUT_FINISHED, INPUT_CANCEL } STATUS_INPUT;

static STATUS_INPUT proccesInput(Gui* gui, Mensaje* mensaje);

void initGui(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
}

void createMainWindow(Gui* gui) {
    getmaxyx(stdscr, gui->main_window_height, gui->main_window_width);
    gui->main_window =
        newwin(gui->main_window_height, gui->main_window_width, 0, 0);

    if (gui->main_window == NULL) {
        LOG_FATAL("Could not create main window");
        exit(EXIT_FAILURE);
    }

    box(gui->main_window, 0, 0);
    mvwprintw(gui->main_window, 1,
              (gui->main_window_width - strlen(APP_TITLE)) / 2, "%s",
              APP_TITLE);

    wrefresh(gui->main_window);
}

void createChatWindow(Gui* gui) {
    const i32 chat_window_height = (gui->main_window_height * 3) / 4 * 3 / 4;
    const i32 chat_window_width = gui->main_window_width - 2;

    gui->secondary_window =
        derwin(gui->main_window, chat_window_height, chat_window_width, 3, 1);

    if (gui->secondary_window == NULL) {
        LOG_FATAL("Could not create chat window");
        exit(EXIT_FAILURE);
    }

    box(gui->secondary_window, 0, 0);
    mvwprintw(gui->secondary_window, 0,
              (chat_window_width - strlen(CHAT_TTILE)) / 2, "%s", CHAT_TTILE);
    scrollok(gui->secondary_window, TRUE);

    const i32 input_window_height =
        gui->main_window_height - chat_window_height - 3;
    const i32 input_window_y_position = chat_window_height + 3;

    gui->input_window = derwin(gui->main_window, input_window_height - 2,
                               chat_window_width, input_window_y_position, 1);

    if (gui->input_window == NULL) {
        LOG_FATAL("Could not create input window");
        exit(EXIT_FAILURE);
    }
    box(gui->input_window, 0, 0);
    mvwprintw(gui->input_window, 0,
              (chat_window_width - strlen(INPUT_TITLE)) / 2, "%s", INPUT_TITLE);

    keypad(gui->input_window, TRUE);
    wmove(gui->input_window, 1, 1);

    wrefresh(gui->secondary_window);
    wrefresh(gui->input_window);
}

void showNewMessage(Gui* gui, const char* message, const u32 index) {
    if (gui->secondary_window == NULL) {
        LOG_ERROR("Secondary window not created");
    }

    if (gui->main_window == NULL) {
        LOG_ERROR("Main window not created");
    }

    wmove(gui->secondary_window, (index + 1), 2);
    wprintw(gui->secondary_window, "%s", message);
    wrefresh(gui->secondary_window);
}

bool processKey(Client* cliente, Gui* gui, Mensaje* input) {
    int key = '\0';
    STATUS_INPUT status;
    nodelay(gui->input_window, TRUE);
    key = wgetch(gui->input_window);
    if (key != ERR) {
        switch (key) {
            case 'q':
            case 27:
                return false;
                break;
            case 'w':
            case 'W':
                status = proccesInput(gui, input);

                if (status == INPUT_FINISHED) {
                    sendMessage(&cliente->memory, input->contenido);
                    showNewMessage(gui, input->contenido,
                                   cliente->memory.num_mensajes);
                    sendNotifiacion(cliente, input->longitud);
                    werase(gui->input_window);  // Limpia la ventana de entrada
                    box(gui->input_window, 0, 0);
                    mvwprintw(
                        gui->input_window, 0,
                        ((gui->main_window_width - 2) - strlen(INPUT_TITLE)) /
                            2,
                        "%s", INPUT_TITLE);

                    keypad(gui->input_window, TRUE);
                    wmove(gui->input_window, 1, 1);
                    wrefresh(gui->input_window);
                    *input = NewMessage;
                } else if (status == INPUT_CANCEL) {
                    *input = NewMessage;
                    werase(gui->input_window);  // Limpia la ventana de entrada
                    box(gui->input_window, 0, 0);
                    wrefresh(gui->input_window);
                    wmove(gui->input_window, 1, 1);
                }
                return true;
                break;

            default:
                break;
        }
    }
    wrefresh(gui->secondary_window);
    wrefresh(gui->input_window);
    wrefresh(gui->main_window);
    return true;
}

static STATUS_INPUT proccesInput(Gui* gui, Mensaje* mensaje) {
    int key = 0;
    bool finished = false;

    // Configurar la ventana para capturar la entrada sin bloquear.
    nodelay(gui->input_window, FALSE);

    // Limpiar el contenido del mensaje
    memset(mensaje->contenido, 0, MAX_TAMAÑO_MENSAJE);

    // Poner el cursor en la posición inicial para la entrada del mensaje.
    wmove(gui->input_window, 1, 1);

    // Loop hasta que el usuario presione Enter o Escape.
    while (!finished) {
        // Obtener una tecla de la entrada
        key = wgetch(gui->input_window);

        switch (key) {
            case 27:  // Tecla Escape
                return INPUT_CANCEL;

            case '\n':  // Tecla Enter
                return INPUT_FINISHED;
                break;

            case KEY_BACKSPACE:  // Tecla Backspace de NCURSES
            case 127:            // Otra tecla Backspace común
                if (mensaje->longitud > 0) {
                    mensaje->longitud--;
                    mensaje->contenido[mensaje->longitud] = '\0';
                    mvwaddch(gui->input_window, 1, mensaje->longitud + 1, ' ');
                    wmove(gui->input_window, 1,
                          mensaje->longitud +
                              1);  // Mueve el cursor a la posición correcta
                }
                break;

            default:
                if (mensaje->longitud < MAX_TAMAÑO_MENSAJE && isprint(key)) {
                    mensaje->contenido[mensaje->longitud] = (char)key;
                    waddch(gui->input_window, key);
                    mensaje->longitud++;
                }
                break;
        }

        // Asegúrate de que la cadena siempre esté terminada correctamente.
        mensaje->contenido[mensaje->longitud] = '\0';

        // Refrescar la ventana para mostrar los cambios.
        wrefresh(gui->input_window);
    }
    return INPUT_CANCEL;
}
