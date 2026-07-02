#include "../include/editor.hpp"
#include <unistd.h>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <array>
#include <filesystem>

using enum Keys;

void Editor::run() {
    while (running) {
        update_text_lines();
        draw();
        process_key(t.read_key());
    }
}

void Editor::open_file(const std::string file_name) {
    m_file_name = file_name;
    std::fstream file(file_name);
    if (!file.is_open()) throw std::runtime_error("file didn't open");
    std::stringstream buf;
    buf << file.rdbuf();
    text = buf.str();
    file.close();
}

void Editor::move_cursor(int c) {
    // find row
    std::string line = renderer_lines[m_cursorY];
    switch (c) {
        case (int) ARROW_UP: if (m_cursorY != 0) m_cursorY--; break;
        case (int) ARROW_DOWN:  if (m_cursorY < renderer_lines.size()) m_cursorY++; break;
        case (int) ARROW_LEFT: 
            if (m_cursorX != 0) m_cursorX--;
            else if (m_cursorY > 0) {
                m_cursorY--;
                m_cursorX = line.size();
            }

            break;
        case (int) ARROW_RIGHT: 
            if (m_cursorX < line.size()) m_cursorX++;
            else if (m_cursorX == line.size()) {
                m_cursorY++;
                m_cursorX = 0;
            }

            break;
    
        default:
            break;
    }

    line = renderer_lines[m_cursorY];
    if (m_cursorX > line.size())
        m_cursorX = line.size();
}

constexpr int CTRL_KEY(int c) { return ((c) & 0x1f); }

void Editor::process_key(int c) {
    switch (c) { 
        case CTRL_KEY('q'): 
            // Clear then reposition cursor
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            running = false;
            break;

        case (int) DEL_KEY: 
            break; /* TODO */
        
        case (int) HOME_KEY:
            m_cursorX = 0;
            break;
        case (int) END_KEY:
            if (m_cursorY < renderer_lines.size()) m_cursorX = renderer_lines[m_cursorY].size();
            break;
        
        case (int) PAGE_UP:
        case (int) PAGE_DOWN:
            {
                if (c == (int) PAGE_UP) {
                    m_cursorY = m_row_offset;
                } else if (c == (int) PAGE_DOWN) {
                    m_cursorY = m_row_offset + t.rows() - 1;
                    if (m_cursorY > m_row_offset) 
                        m_cursorY = t.rows();
                }

                int times = t.rows();
                while (times--)
                move_cursor(c == (int) PAGE_UP ? (int) ARROW_UP : (int) ARROW_DOWN);
            }
            break;

        // Move cursor
        case (int) ARROW_UP:
        case (int) ARROW_DOWN:
        case (int) ARROW_LEFT:
        case (int) ARROW_RIGHT:
            move_cursor(c); break;
    }
}

void Editor::update_text_lines() {
    std::string r_curr, e_curr;
    renderer_lines.clear();
    editor_lines.clear();

    for (char c : text) {
        if (c == '\n') {
            renderer_lines.push_back(r_curr);
            editor_lines.push_back(e_curr);
            r_curr.clear();
            e_curr.clear();
        } else if (c == '\t') {
            int spaces = TAB_STOP - (r_curr.size() % TAB_STOP);
            r_curr.append(spaces, ' ');
            e_curr += c;
        }
        
        else {
            r_curr += c;
            e_curr += c;
        }
    }

    renderer_lines.push_back(r_curr);
    editor_lines.push_back(e_curr);
}

void Editor::draw_rows(std::string &buf) {
    int curr_line = 0;
    for (int y = 0; y < t.rows(); y++) {
        curr_line = y + m_row_offset;
        if (curr_line >= renderer_lines.size()) {
            if (renderer_lines.empty() && y == t.rows() / 3) {
                std::string s = "Text editor -- version " + (std::string)VERSION;
                if (s.size() > t.cols()) s.resize(t.cols());
                size_t padding = (t.cols() - s.size()) / 2;
                if (padding) {
                    buf += '~';
                    --padding;
                }
    
                buf.append(padding, ' ');
                buf.append(s);
            } else {
                buf.append("~");
            }

        } else {
            std::string line = renderer_lines[curr_line];
            if (m_col_offset < line.size()) {
                line = line.substr(m_col_offset);
                line.resize(t.cols());
                buf.append(line);
            }
        }


        buf.append("\x1b[K");
        buf.append("\r\n");
    }
}

void Editor::draw_status_bar(std::string & buf) {
    int i = 0;
    std::string s; 
    buf.append("\x1b[7m");
    if (m_status_msg.size() > 0)
        s = m_status_msg;
    else {
        if (m_file_name == "")
            s = "[New]";
        else
            s = "[" + std::filesystem::path(m_file_name).filename().string() + "]";
    }

    buf.append(s);
    i += s.size();
    for (i; i < t.cols(); ++i) {
        buf.append(" ");
    }

    buf.append("\x1b[m");
}

void Editor::draw() {
    scroll();

    std::string append_buf = "\0";

    // reposition cursor
    append_buf.append("\x1b[?25l");
    append_buf.append("\x1b[H");

    draw_rows(append_buf);
    draw_status_bar(append_buf);
    std::string cmd = "\x1b[" + std::to_string(m_cursorY - m_row_offset + 1)
     + ";" + std::to_string(m_cursorX - m_col_offset + 1) + "H";
    append_buf.append(cmd);
    append_buf.append("\x1b[?25h");
    write(STDOUT_FILENO, append_buf.c_str(), append_buf.size());
}

void Editor::scroll() {

    // UP
    if (m_cursorY < m_row_offset)
        m_row_offset = m_cursorY;

    // Down
    if (m_cursorY >= m_row_offset + t.rows())
        m_row_offset = m_cursorY - t.rows() + 1;

    // Left
    if (m_cursorX < m_col_offset)
        m_col_offset = m_cursorX;

    // Right
    if (m_cursorX >= m_col_offset + t.cols())
        m_col_offset = m_cursorX - t.cols() + 1;
}