#include "../include/editor.hpp"
#include <unistd.h>
#include <stdexcept>
#include <fstream>
#include <string>
#include <sstream>

using enum Keys;

void Editor::run() {
    while (running) {
        r.draw(*this);
        process_key(t.read_key());
    }
}

void Editor::open_file(const std::string file_name) {
    std::fstream file(file_name);
    if (!file.is_open()) throw std::runtime_error("file didn't open");
    std::stringstream buf;
    buf << file.rdbuf();
    std::string text = buf.str();
    r.text = text;
}

void Editor::move_cursor(int c) {
    switch (c) {
        case (int) ARROW_UP: if (m_cursorY != 0) m_cursorY--; break;
        case (int) ARROW_DOWN:  if (m_cursorY != t.rows() - 1) m_cursorY++; break;
        case (int) ARROW_LEFT:  if (m_cursorX != 0) m_cursorX--; break;
        case (int) ARROW_RIGHT: if (m_cursorX != t.cols() - 1) m_cursorX++; break;
    
        default:
            break;
    }
}

constexpr int CTRL_KEY(int c) { return ((c) & 0x1f); }

void Editor::process_key(int c) {
    switch (c) { 
        case CTRL_KEY('q'): 
            // Clear then reposition cursor
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            running = false;
            break;

        case (int) DEL_KEY: break; /* TODO */
        
        case (int) HOME_KEY:
            m_cursorX = 0;
            break;
        case (int) END_KEY:
            m_cursorX = t.cols() - 1;
            break;
        
        case (int) PAGE_UP:
        case (int) PAGE_DOWN:
            {
                int times = t.rows();
                while (times--)
                move_cursor(c == (int) PAGE_UP ? (int) ARROW_UP : (int) ARROW_DOWN);
            }
            break;

        // Move cursor
        case (int) ARROW_UP:
        case (int) ARROW_DOWN:
        case (int) ARROW_LEFT:
        case (int) ARROW_RIGHT:
            move_cursor(c); break;
    }
}