#pragma once

#include <string>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <termios.h>
#endif

class Terminal {
    public:
        Terminal() {
            enable_raw();
            size();
        };

        ~Terminal() {
            disable_raw();
        };

        
        int read_key();

        void terminal_write(const std::string &s);
        
        int rows() const { return m_rows; }
        int cols() const { return m_cols; }
        
    private:
        void enable_raw();
        void disable_raw();
        int m_rows;
        int m_cols;

        void size();
        #ifdef _WIN32
            HANDLE hIn;
            HANDLE hOut;
            DWORD originalInMode;
            DWORD originalOutMode;
        #else
            struct termios original;
        #endif
};