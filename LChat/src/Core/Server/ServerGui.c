#include "Core/Server/ServerGui.h"
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "Utils/clog.h"
#include "curses.h"

#define APP_TITLE "Latin Chat Server"
#define TITLE_INPUT " Puerto:"
#define CHAT_TTILE " Chat:"
#define DEFAULT_PORT 8080

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
    // LOG_INFO("Main window created");
}

void createInputWindow(Gui* gui) {
    gui->secondary_window =
        derwin(gui->main_window, 3, 20, gui->main_window_height / 2,
               (gui->main_window_width - 20) / 2);

    if (gui->secondary_window == NULL) {
        LOG_FATAL("Could not create input window");
        exit(EXIT_FAILURE);
    }
    box(gui->secondary_window, 0, 0);
    mvwprintw(gui->secondary_window, 0, (20 - strlen(TITLE_INPUT)) / 2, "%s",
              TITLE_INPUT);

    wmove(gui->secondary_window, 1, 1);
    curs_set(1);

    wrefresh(gui->main_window);
    wrefresh(gui->secondary_window);
}

i32 getInputWindow(Gui* gui) {
    i32 port = DEFAULT_PORT;
    char input_buffer[5] = {0};
    char ch;
    i32 index = 0;

    while (index < 5) {
        ch = wgetch(gui->secondary_window);

        if (ch >= '0' && ch <= '9') {
            if (index < 5 - 1) {
                input_buffer[index] = (char)ch;
                waddch(gui->secondary_window, ch);
                index++;
            }
        } else if ((i32)ch == KEY_BACKSPACE || ch == 127) {
            if (index > 0) {
                input_buffer[--index] = '\0';
                mvwaddch(gui->secondary_window, 1, index + 1, ' ');
                wmove(gui->secondary_window, 1, index + 1);
            }
        } else if (ch == '\n') {
            break;
        }
    }

    if (index > 4) {
        port = atoi(input_buffer);
    }

    flushinp();
    werase(gui->secondary_window);
    wrefresh(gui->main_window);
    delwin(gui->secondary_window);

    return port;
}

void createChatWindow(Gui* gui) {
    gui->secondary_window =
        derwin(gui->main_window, gui->main_window_height - 6,
               gui->main_window_width - 3, 3, 1);

    if (gui->secondary_window == NULL) {
        LOG_FATAL("Could not create chat window");
        exit(EXIT_FAILURE);
    }
    box(gui->secondary_window, 0, 0);

    mvwprintw(gui->secondary_window, 0,
              ((gui->main_window_width - 3) - strlen(CHAT_TTILE)) / 2, "%s",
              CHAT_TTILE);

    keypad(gui->secondary_window, TRUE);
    scrollok(gui->secondary_window, TRUE);
    nodelay(gui->secondary_window, TRUE);

    wmove(gui->secondary_window, 1, 2);
    wrefresh(gui->main_window);
    wrefresh(gui->secondary_window);
}

void showNewMessage(Gui* gui, const char* message, const u32 index) {
    if (gui->secondary_window == NULL) {
        LOG_ERROR("Secondary window not created");
    }

    if (gui->main_window == NULL) {
        LOG_ERROR("Main window not created");
    }

    wprintw(gui->secondary_window, "%s", message);
    wmove(gui->secondary_window, (index + 1), 2);
    wrefresh(gui->secondary_window);
}

void terminateGui(Gui* gui) {
    delwin(gui->secondary_window);
    delwin(gui->main_window);
    endwin();
}

bool processInputKey(Gui* gui) {
    int key = '\0';
    // flushinp();
    nodelay(gui->secondary_window, TRUE);
    key = wgetch(gui->secondary_window);

    if (key != ERR) {
        switch (key) {
            case 'q':
                return false;
                break;
            default:
                break;
        }
    }
    wrefresh(gui->secondary_window);
    wrefresh(gui->main_window);
    return true;
}
