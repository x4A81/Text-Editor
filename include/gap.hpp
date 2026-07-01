#pragma once

#include <stdlib.h>
#include <string_view>
#include <vector>

class Gap_Buffer {
    public:
        void insert(char c);
        void erase();
        void move_left();
        void move_right();

        std::string_view left() const;
        std::string_view right() const;

    private:
        std::vector<char> buffer;
        long start;
        size_t gap_length;
        long end;
};