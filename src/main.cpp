#include "../include/editor.hpp"

int main(int argc, char* argv[]) {
    Editor editor;
    if (argc > 1)
        editor.open_file(argv[1]);
    editor.run();
    return 0;
}