#pragma once

#include "globals.hpp"
#include "terminal.hpp"
#include <string>
#include <vector>

class Editor {
    public:
        Editor()
        : t()
        { };

        void run();
        void open_file(const std::string file_name);

        Terminal t; 

        int cursorX() const { return m_cursorX; }
        int cursorY() const { return m_cursorY; }
        int row_offset() const { return m_row_offset; }

        std::string text;
        std::string m_status_msg;
        
    private:
        std::vector<std::string> renderer_lines;
        std::vector<std::string> editor_lines;

        std::string m_file_name;
        bool running = true;

        int m_cursorX = 0;
        int m_cursorY = 0;

        // for scrolling
        int m_row_offset = 0;
        int m_col_offset = 0;

        void process_key(int c);
        void update_text_lines();
        void move_cursor(int c);
        
        void draw_status_bar(std::string &buf);
        void draw_rows(std::string &buf);
        void draw();

        void scroll();
};
