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
        void open_file(const std::string s);

        void save_file();

        Terminal t;
        Gap_Buffer g;

        std::string text;
        std::string status_msg;

        size_t pos();
        
    private:
        bool dirty = false; // true if unsaved modifications
        
        std::vector<std::string> renderer_lines;
        std::vector<std::vector<Highlight>> highlights;
        std::vector<std::string> editor_lines;

        std::string file_name;
        bool running = false;

        int cursorX = 0;
        int renderX = 0;
        int cursorY = 0;

        // for scrolling
        int row_offset = 0;
        int col_offset = 0;

        // for find()
        int match_pos = -1;
        int match_len = -1;

        // checks for specific key presses
        void process_key(int c);

        // updates the rendering and editor text
        void update_text_lines(const std::string &t);
        void move_cursor(int c);
        
        void draw_status_bar(std::string &buf);
        void draw_rows(std::string &buf);
        void draw();

        std::string prompt(const std::string &msg);

        void find();

        int cursor_to_renderX();

        void scroll();
};
