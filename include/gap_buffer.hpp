#pragma once

#include <string>

class Gap_Buffer {
    public:
        Gap_Buffer(size_t size) {
            buffer.resize(size);
            gap_end = size;
        }

        // Inserts char c at current buffer position
        void insert_char(char c);
        void erase();

        
        void move_left();
        void move_right();
        
        void go_to(int pos);
        
        void load_text(const std::string &text);
        
        std::string to_string();
        
    private:
        void grow();
        std::string buffer;
        size_t gap_begin = 0;
        size_t gap_end = 0;


};