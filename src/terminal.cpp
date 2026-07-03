#include "../include/terminal.hpp"
#include "../include/globals.hpp"

#include <stdexcept>

#include <sys/ioctl.h>
#include <unistd.h>
#include <array>

using enum Keys;

void Terminal::size() {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        throw std::runtime_error("failed to find window size");
    } else {
        m_cols = ws.ws_col;
        m_rows = ws.ws_row - 1; // Leave last row for status bar
    }
}

void Terminal::disable_raw() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original) == -1) 
        throw std::runtime_error("failed to set attribute");
}

void Terminal::enable_raw() {
    if (tcgetattr(STDIN_FILENO, &original) == -1)
        throw std::runtime_error("failed to set attribute");

    struct termios raw = original;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        throw std::runtime_error("failed to set attribute");
}

int Terminal::read_key() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN)
            throw std::runtime_error("failed to read. i skipped reading class");
    }

    if (c != '\x1b') {
        return c;
    }

    std::array<char, 3> seq{};

    if (read(STDIN_FILENO, seq.data(), 2) != 2)
        return '\x1b';

    if (seq[0] != '[')
        return '\x1b';
    if (seq[1] >= '0' && seq[1] <= '9') {
        if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';

        if (seq[2] == '~') {
            switch (seq[1]) {
                case '1': return (int) HOME_KEY;
                case '3': return (int) DEL_KEY;
                case '4': return (int) END_KEY;
                case '5': return (int) PAGE_UP;
                case '6': return (int) PAGE_DOWN;
                case '7': return (int) HOME_KEY;
                case '8': return (int) END_KEY;
            }
        }
    } else {
        switch (seq[1]) {
            case 'A': return (int) ARROW_UP;
            case 'B': return (int) ARROW_DOWN;
            case 'C': return (int) ARROW_RIGHT;
            case 'D': return (int) ARROW_LEFT;
            case 'H': return (int) HOME_KEY;
            case 'F': return (int) END_KEY;
        }
    }

    if (seq[0] == 'O') {
        switch (seq[1]) {
            case 'H': return (int) HOME_KEY;
            case 'F': return (int) END_KEY;
        }
    }

    return '\x1b';
}

void Terminal::terminal_write(const std::string &s) {
    write(STDOUT_FILENO, s.c_str(), s.length());
}