#include "../include/editor.hpp"

int main() {
    Editor editor;
    editor.open_file("../testfile.txt");
    // editor.m_status_msg = "HELP!!!!! HELP!!!!";
    editor.run();
    return 0;
}