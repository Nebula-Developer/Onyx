#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define MAX_INPUT_LENGTH 1024

void move_cursor(int x) {
    char buf[32];
    sprintf(buf, "\033[%dG", x);
    write(STDOUT_FILENO, buf, strlen(buf));
}

int main() {
    char prefix[] = "Prefix: ";
    char input[MAX_INPUT_LENGTH] = "";
    char autocomplete[] = "Autocomplete";
    int cursor_x = 0;
    int input_length = strlen(input);

    struct termios old_term, new_term;
    tcgetattr(STDIN_FILENO, &old_term);
    new_term = old_term;
    new_term.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

    write(STDOUT_FILENO, "\033[G", 3);
    write(STDOUT_FILENO, prefix, strlen(prefix));

    while (1) {
        move_cursor(strlen(prefix));

        write(STDOUT_FILENO, input, input_length);
        write(STDOUT_FILENO, autocomplete, strlen(autocomplete));
        write(STDOUT_FILENO, " ", 1);

        move_cursor(strlen(prefix) + cursor_x);
        fflush(stdout);

        char c;
        read(STDIN_FILENO, &c, 1);

        if (c == '\n') {
            break;
        } else if (c == '\b' && cursor_x > 0) {
            cursor_x--;
            input_length--;
            memmove(&input[cursor_x], &input[cursor_x + 1], input_length - cursor_x);
        } else if (c == '\033') {
            if (read(STDIN_FILENO, &c, 1) == 1 && c == '[') {
                if (read(STDIN_FILENO, &c, 1) == 1) {
                    if (c == 'C' && cursor_x < input_length) {
                        cursor_x++;
                    } else if (c == 'D' && cursor_x > 0) {
                        cursor_x--;
                    }
                }
            }
        } else if (c >= ' ' && c <= '~' && input_length < MAX_INPUT_LENGTH) {
            memmove(&input[cursor_x + 1], &input[cursor_x], input_length - cursor_x);
            input[cursor_x] = c;
            cursor_x++;
            input_length++;
        } else if (c == 127) {
            if (cursor_x > 0) {
                cursor_x--;
                input_length--;
                memmove(&input[cursor_x], &input[cursor_x + 1], input_length - cursor_x);
            }
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);

    return 0;
}
