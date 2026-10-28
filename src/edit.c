#include "edit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>

#define BYTES_PER_ROW 16
#define HEX_X 10
#define ASCII_X 60
#define MIN_COLS 76
#define MIN_LINES 10
#define HEADER_ROWS 4

void draw_ui(const char *filename, long file_size, const unsigned char *buffer,
             long scroll_offset, long cursor_offset, const char *status, int modified) {
    erase();

    if (COLS < MIN_COLS || LINES < MIN_LINES) {
        mvprintw(0, 0, "Terminal is too small!");
        return;
    }

    mvprintw(0, 0, "chex - The Hex Editor");
    mvprintw(1, 0, "File: %s%s", filename, modified ? " [modified]" : "");
    mvprintw(2, 0, "Size: %ld bytes", file_size);

    const char *version = "chex v0.1";
    int version_x = COLS - (int)strlen(version);

    mvprintw(LINES - 1, 0, "q: Quit");
    mvprintw(LINES - 1, 8, "s: Save");
    mvprintw(LINES - 1, version_x, "%s", version);

    int status_width = version_x - 20 - 1;
    if (status_width > 0 && status[0] != '\0') {
        mvprintw(LINES - 1, 20, "%.*s", status_width, status);
    }

    mvprintw(3, 0, "OFFSET");
    mvprintw(3, HEX_X, "HEX");
    mvprintw(3, ASCII_X, "ASCII");

    for (long offset = scroll_offset * BYTES_PER_ROW;
         offset < file_size && HEADER_ROWS + (offset / BYTES_PER_ROW - scroll_offset) < LINES - 1;
         offset += BYTES_PER_ROW) {
        int y = HEADER_ROWS + (int)(offset / BYTES_PER_ROW - scroll_offset);

        mvprintw(y, 0, "%08lX", offset);

        for (long i = offset; i < file_size && i < offset + BYTES_PER_ROW; i++) {
            int x = HEX_X + (int)(i - offset) * 3;
            int ascii_x = ASCII_X + (int)(i - offset);
            int is_cursor = (i == cursor_offset);

            if (is_cursor) {
                attron(A_REVERSE);
            }

            mvprintw(y, x, "%02X", buffer[i]);
            mvaddch(y, ascii_x, isprint(buffer[i]) ? buffer[i] : '.');

            if (is_cursor) {
                attroff(A_REVERSE);
            }
        }
    }
}

int save_file(FILE *file, const unsigned char *buffer, long file_size) {
    if (fseek(file, 0, SEEK_SET) != 0) {
        return -1;
    }

    size_t written = fwrite(buffer, 1, (size_t)file_size, file);
    if (written != (size_t)file_size) {
        return -1;
    }

    if (fflush(file) != 0) {
        return -1;
    }

    return 0;
}

int hex_value(int ch) {
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }

    if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    }

    if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }

    return -1;
}

void edit(FILE *file, const char *filename) {
    if (fseek(file, 0, SEEK_END) != 0) {
        perror("fseek");
        return;
    }

    long file_size = ftell(file);
    if (file_size == -1L) {
        perror("ftell");
        return;
    }

    if (file_size == 0) {
        fprintf(stderr, "File is empty\n");
        return;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("fseek");
        return;
    }

    unsigned char *buffer = malloc((size_t)file_size);
    if (buffer == NULL) {
        perror("malloc");
        return;
    }

    size_t bytes_count = fread(buffer, 1, (size_t)file_size, file);
    if (bytes_count != (size_t)file_size) {
        if (ferror(file) != 0) {
            perror("fread");
        }
        free(buffer);
        return;
    }

    // TUI
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    cbreak();

    long cursor_offset = 0;
    long scroll_offset = 0;
    int high_nibble = 1;
    int modified = 0;
    int quit_warned = 0;
    const char *status = "";

    for (;;) {
        long visible_rows = LINES - 5;
        if (visible_rows < 1) {
            visible_rows = 1;
        }

        long cursor_row = cursor_offset / BYTES_PER_ROW;
        if (cursor_row < scroll_offset) {
            scroll_offset = cursor_row;
        } else if (cursor_row >= scroll_offset + visible_rows) {
            scroll_offset = cursor_row - visible_rows + 1;
        }

        draw_ui(filename, file_size, buffer, scroll_offset, cursor_offset, status, modified);
        refresh();

        // 3. Ввод
        int ch = getch();
        status = "";

        if (ch == 'q') {
            if (modified && !quit_warned) {
                status = "Unsaved changes! Press q again to quit";
                quit_warned = 1;
                continue;
            }
            break;
        }
        quit_warned = 0;

        if (ch == KEY_LEFT) {
            if (cursor_offset > 0) {
                cursor_offset--;
            }
            high_nibble = 1;
        } else if (ch == KEY_RIGHT) {
            if (cursor_offset < file_size - 1) {
                cursor_offset++;
            }
            high_nibble = 1;
        } else if (ch == KEY_UP) {
            if (cursor_offset >= BYTES_PER_ROW) {
                cursor_offset -= BYTES_PER_ROW;
            }
            high_nibble = 1;
        } else if (ch == KEY_DOWN) {
            if (cursor_offset + BYTES_PER_ROW < file_size) {
                cursor_offset += BYTES_PER_ROW;
            }
            high_nibble = 1;
        } else if (ch == 's') {
            if (save_file(file, buffer, file_size) == 0) {
                status = "Saved!";
                modified = 0;
            } else {
                status = "Save failed!";
            }
        } else {
            int hex_digit = hex_value(ch);
            if (hex_digit != -1) {
                if (high_nibble) {
                    buffer[cursor_offset] = (unsigned char)((buffer[cursor_offset] & 0x0F) | (hex_digit << 4));
                    high_nibble = 0;
                } else {
                    buffer[cursor_offset] = (unsigned char)((buffer[cursor_offset] & 0xF0) | hex_digit);
                    high_nibble = 1;
                    if (cursor_offset < file_size - 1) {
                        cursor_offset++;
                    }
                }
                modified = 1;
            }
        }
    }

    endwin();
    free(buffer);
}
