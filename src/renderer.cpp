#include "../include/renderer.hpp"
#include "../include/globals.hpp"
#include "../include/editor.hpp"

#include <unistd.h>

void Renderer::draw_rows(std::string &buf, const Terminal &t) {
    std::string cpy = text;
    for (int y = 0; y < t.rows(); y++) {
        
        if (y == t.rows() / 3) {
            std::string s = "Text editor -- version " + (std::string)VERSION;
            if (s.size() > t.cols()) s.resize(t.cols());
            size_t padding = (t.cols() - s.size()) / 2;
            if (padding) {
                buf += '~';
                --padding;
            }

            buf.append(padding, ' ');
            buf.append(s);
        } else {
            buf.append("~");
        }

        int col = 0;
        while (!cpy.empty() && cpy[0] != '\n') {
            if (col < t.cols()) {
                buf += cpy[0]; 
                col++;
            }

            cpy.erase(0, 1);
        }

        if (!cpy.empty() && cpy[0] == '\n') cpy.erase(0,1);

        buf.append("\x1b[K");
        if (y < t.rows() - 1) {
            buf.append("\r\n");
        }
    }
}

void Renderer::draw(Editor &e) {

    std::string append_buf = "\0";

    // reposition cursor
    append_buf.append("\x1b[?25l");
    append_buf.append("\x1b[H");

    draw_rows(append_buf, e.t);
    std::string cmd = "\x1b[" + std::to_string(e.cursorY()) + ";" + std::to_string(e.cursorX()) + "H";
    append_buf.append(cmd);
    append_buf.append("\x1b[?25h");
    write(STDOUT_FILENO, append_buf.c_str(), append_buf.size());
}