#include "include/editor.hpp"

int main() {
    Editor editor;
    editor.open_file("/home/jkingsbury/Desktop/Text Editor/testfile.txt");
    editor.run();
    return 0;
}