#pragma once

#include <termios.h>

class Terminal {
    public:
        Terminal() {
            size();
            enable_raw();
        };

        ~Terminal() {
            disable_raw();
        };

        
        int read_key();
        
        int rows() const { return m_rows; }
        int cols() const { return m_cols; }
        
    private:
        struct termios original;
        void enable_raw();
        void disable_raw();
        int m_rows;
        int m_cols;

        void size();
};