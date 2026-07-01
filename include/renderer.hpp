#pragma once

#include "terminal.hpp"
#include <string>

class Editor;

class Renderer {
    public:
        Renderer() {};
        void draw(Editor &e);
        std::string text;
    private:
        void draw_rows(std::string &buf, const Terminal &t);
};