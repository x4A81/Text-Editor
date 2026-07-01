#pragma once

#include "globals.hpp"
#include "terminal.hpp"
#include "renderer.hpp"
#include <string>

class Editor {
    public:
        Editor()
        : t(),
          r()
        { };

        void run();
        void open_file(const std::string file_name);

        Terminal t; 
        Renderer r;

        int cursorX() const { return m_cursorX; }
        int cursorY() const { return m_cursorY; }
        int text_rows() const { return m_text_rows; }

        std::string text;

    private:
        bool running = true;

        int m_cursorX = 0;
        int m_cursorY = 0;

        int m_text_rows = 0;

        void process_key(int c);
        void move_cursor(int c);
};
