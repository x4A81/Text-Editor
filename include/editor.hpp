#pragma once

#include "globals.hpp"
#include "terminal.hpp"
#include "gap_buffer.hpp"
#include <string>
#include <vector>

class Editor {
    public:
        Editor()
        : t(), g(64)
        { };

        // runs the editor
        void run();

        // opens a file to edit
        void open_file(const std::string file_name);

        void save_file();

        Terminal t;
        Gap_Buffer g;

        int cursorX() const { return m_cursorX; }
        int cursorY() const { return m_cursorY; }
        int row_offset() const { return m_row_offset; }

        std::string text;
        std::string m_status_msg;

        size_t pos();
        
    private:
        bool dirty = false; // true if unsaved modifications
        
        std::vector<std::string> renderer_lines;
        std::vector<std::string> editor_lines;

        std::string m_file_name;
        bool running = false;

        int m_cursorX = 0;
        int m_renderX = 0;
        int m_cursorY = 0;

        // for scrolling
        int m_row_offset = 0;
        int m_col_offset = 0;

        // checks for specific key presses
        void process_key(int c);

        // updates the rendering and editor text
        void update_text_lines(const std::string &t);
        void move_cursor(int c);
        
        void draw_status_bar(std::string &buf);
        void draw_rows(std::string &buf);
        void draw();

        int cursor_to_renderX();

        void scroll();
};
