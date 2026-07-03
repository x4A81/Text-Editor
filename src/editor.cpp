#include "../include/editor.hpp"
#include "../include/gap_buffer.hpp"

#include <stdexcept>
#include <fstream>
#include <sstream>
#include <array>
#include <filesystem>

using enum Keys;
using enum Highlight;

const std::string colour_highlight(Highlight hl) {
    switch (hl) {
    case TILDE:
        return "\x1b[91m";

    case NORMAl:
        return "\x1b[m";

    case NUMBER:
        return "\x1b[95m";

    case MATCH:
        return "\x1b[92m";
    
    default:
        break;
    }

    return "";
}

void Editor::run() {
    running = true;
    while (running) {
        update_text_lines(g.to_string());
        draw();
        process_key(t.read_key());
    }
}

void Editor::open_file(const std::string s) {
    file_name = s;
    std::fstream file(file_name);
    if (!file.is_open()) throw std::runtime_error("file didn't open");
    std::stringstream buf;
    buf << file.rdbuf();
    text = buf.str();
    file.close();
    g.load_text(text);
}

void Editor::save_file() {
    if (file_name.empty()) {
        file_name = prompt("Save as: ");
        if (file_name.empty()) file_name = "New file.txt";
    }
    
    std::ofstream file(file_name);
    if (!file)
        throw std::runtime_error("file didn't open");
    std::string t = g.to_string();
    file << t;
    if (!file)
        throw std::runtime_error("error writing file");
        
    status_msg = std::to_string(t.size()) + " bytes written to disk.";
    dirty = false;
}

bool Editor::find_next(const std::string &s) {
    for (int i = cursorY; i < editor_lines.size(); ++i) {
        std::string r = editor_lines[i];
        size_t match = r.find(s);
        if (match != std::string::npos) {
            match_posX = match;
            match_posY = i;
            match_len = s.length();
            status_msg = "Found!";
            break;
        }
    }
    
    if (match_posX == -1) {
        return false;
    }

    return true;
}

bool Editor::find_prev(const std::string &s) {
    for (int i = cursorY; i >= 0; --i) {
        std::string r = editor_lines[i];
        size_t match = r.find(s);
        if (match != std::string::npos) {
            match_posX = match;
            match_posY = i;
            match_len = s.length();
            status_msg = "Found!";
            break;
        }
    }
    
    if (match_posX == -1) {
        return false;
    }

    return true;
}



void Editor::find() {
    std::string s = prompt("Find: ");

    
    if (s.empty()) return;

    if (!find_next(s)) {
        if (!find_prev(s)) {
            status_msg = "Not Found.";
            return;
        }
    }

    while (match_posY != cursorY) {
        if (cursorY < match_posY)
            move_cursor((int) ARROW_DOWN);

        if (cursorY > match_posY)
            move_cursor((int) ARROW_UP);
    }

    while (match_posX != cursorX) {
        if (cursorX < match_posX)
            move_cursor((int) ARROW_RIGHT);

        if (cursorX > match_posX)
            move_cursor((int) ARROW_LEFT);
    }
}

size_t Editor::pos() {
    size_t index = 0;

    for (int i = 0; i < cursorY; ++i)
        index += editor_lines[i].size() + 1; // + '\n'

    index += cursorX;

    return index;
}

void Editor::move_cursor(int c) {
    // find row
    std::string line;
    if (cursorY < renderer_lines.size())
        line = renderer_lines[cursorY];
    switch (c) {
        case (int) ARROW_UP: if (cursorY != 0) cursorY--; break;
        case (int) ARROW_DOWN:  if (cursorY < renderer_lines.size() - 1) cursorY++; break;
        case (int) ARROW_LEFT: 
            if (cursorX != 0) {
                cursorX--;
            }
            else if (cursorY > 0) {
                cursorY--;
                cursorX = line.size();
            }

            break;
        case (int) ARROW_RIGHT: 
            if (cursorX < line.size()) {
                cursorX++;

            } 
            else if (cursorX == line.size() && cursorY < renderer_lines.size()-1) {
                cursorY++;
                cursorX = 0;
            }

            break;
    
        default:
            break;
    }

    if (cursorY < renderer_lines.size()) {
        line = renderer_lines[cursorY];
        if (cursorX > line.size())
        cursorX = line.size();
    }

    g.go_to(pos());
}

constexpr int CTRL_KEY(int c) { return ((c) & 0x1f); }

void Editor::process_key(int c) {
    match_posX = -1;
    static int quit_times = 2;
    status_msg = ""; // reset status msg on key press
    switch (c) { 
        case '\r':
            g.insert_char('\n');
            cursorX = 0;
            cursorY++;
            g.go_to(pos());
            break;
        case CTRL_KEY('q'):
            if (dirty && quit_times > 0) {
                status_msg = "WARNING!!! File has unsaved modifications. Ctrl+Q to quit without saving.";
                quit_times --;
                return;
            }

            // Clear then reposition cursor
            t.terminal_write("\x1b[2J");
            t.terminal_write("\x1b[H");
            running = false;
            break;

        case CTRL_KEY('s'):
            save_file();
            break;

        case CTRL_KEY('f'):
            find();
            
            break;

        case (int) BACK_SPACE:
        case CTRL_KEY('h'):
        case (int) DEL_KEY: 
            g.erase();
            move_cursor((int) ARROW_LEFT);
            dirty = true;
            break; 
        
        case (int) HOME_KEY:
            cursorX = 0;
            break;
        case (int) END_KEY:
            if (cursorY < renderer_lines.size()) cursorX = renderer_lines[cursorY].size();
            break;
        
        case (int) PAGE_UP:
        case (int) PAGE_DOWN:
            {
                if (c == (int) PAGE_UP) {
                    cursorY = row_offset;
                } else if (c == (int) PAGE_DOWN) {
                    cursorY = row_offset + t.rows() - 1;
                    if (cursorY > renderer_lines.size()) 
                        cursorY = renderer_lines.size()-1;
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

        case CTRL_KEY('l'):
        case '\x1b':
            break;
        
        default:
            g.insert_char(c); 
            cursorX++;
            dirty = true;
            break;
    }

    quit_times = 2;
}

void Editor::update_text_lines(const std::string &t) {
    std::string r_curr, e_curr;
    std::vector<Highlight> h_curr;
    renderer_lines.clear();
    editor_lines.clear();
    highlights.clear();
    int line = 0;
    int x = 0;
    for (char c : t) {
        if (c == '\n') {
            h_curr.push_back(NORMAl);
            renderer_lines.push_back(r_curr);
            editor_lines.push_back(e_curr);
            highlights.push_back(h_curr);
            r_curr.clear();
            e_curr.clear();
            h_curr.clear();
            line++;
            x = 0;
        }
        
        else if (c == '\t') {
            int spaces = TAB_STOP - (r_curr.size() % TAB_STOP);
            r_curr.append(spaces, ' ');
            h_curr.insert(h_curr.end(), spaces, NORMAl);
            e_curr += c;
        } else if ('0' <= c && c <= '9') {
            if (match_posX != -1 &&
             (line == match_posY && x > match_posX && x <= match_posX + match_len))
                h_curr.push_back(MATCH);
            else
                h_curr.push_back(NUMBER);

            r_curr += c;
            e_curr += c;
        }
        
        else {
            r_curr += c;
            if (match_posX != -1 &&
                (line == match_posY && x > match_posX && x <= match_posX + match_len))
                h_curr.push_back(MATCH);
            else
                h_curr.push_back(NORMAl);
            e_curr += c;
        }
        
        x++;
    }

    renderer_lines.push_back(r_curr);
    editor_lines.push_back(e_curr);
    highlights.push_back(h_curr);
}

void Editor::draw_rows(std::string &buf) {
    int curr_line = 0;
    for (int y = 0; y < t.rows(); y++) {
        curr_line = y + row_offset;
        if (curr_line >= renderer_lines.size()) {
            if (renderer_lines.size() == 1 && y == t.rows() / 3) {
                std::string s = "Text editor -- version " + (std::string)VERSION;
                if (s.size() > t.cols()) s.resize(t.cols());
                size_t padding = (t.cols() - s.size()) / 2;
                if (padding) {
                    buf.append(colour_highlight(TILDE));
                    buf.append("~");
                    buf.append(colour_highlight(NORMAl));
                    --padding;
                }
    
                buf.append(padding, ' ');
                buf.append(s);
            } else {
                buf.append(colour_highlight(TILDE));
                buf.append("~");
                buf.append(colour_highlight(NORMAl));
            }

        } else if (g.to_string().empty() && curr_line == 0) {
            buf.append(colour_highlight(TILDE));
            buf.append("~");
            buf.append(colour_highlight(NORMAl));
        } else {
            std::string line = renderer_lines[curr_line];
            std::vector<Highlight> hl = highlights[curr_line];
            if (col_offset < line.size()) {
                line = line.substr(col_offset);
                line.resize(t.cols());
                for (size_t i = 0; i < line.size(); ++i) {
                    size_t orig = i + col_offset;

                    if (orig >= hl.size())
                        break;

                    buf += colour_highlight(hl[orig]);
                    buf += line[i];
                }
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
    if (status_msg.size() > 0)
        s = status_msg;
    else {
        if (file_name == "")
            s = "[New]";
        else {
            s = "[" + std::filesystem::path(file_name).filename().string() + "]";
            if (dirty) s += "(modified)";
        }
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

    // change cursor type to blinking bar
    append_buf.append("\x1b[5 q");
    // reposition cursor
    append_buf.append("\x1b[?25l");
    append_buf.append("\x1b[H");

    draw_rows(append_buf);
    draw_status_bar(append_buf);

    std::string cmd = "\x1b[" + std::to_string(cursorY - row_offset + 1)
     + ";" + std::to_string(renderX - col_offset + 1) + "H";
    append_buf.append(cmd);
    append_buf.append("\x1b[?25h");
    t.terminal_write(append_buf);
}

std::string Editor::prompt(const std::string &msg) {
    std::string in;
    while (true) {
        status_msg = msg + in;
        draw();
        int c = t.read_key();
        switch (c) {
            case '\r':
                return in;

            case '\x1b':
                return "";

            case (int) BACK_SPACE:
                if (!in.empty())
                    in.pop_back();
                break;

            default:
                if (isprint(c))
                    in += (char)c;
        }
    }
    
    return std::string();
}

int Editor::cursor_to_renderX() {
    int rx = 0;
    for (int j = 0; j < cursorX; j++) {
        if (editor_lines[cursorY][j] == '\t')
            rx += (TAB_STOP - 1) - (rx % TAB_STOP);
        ++rx; 
    }

    return rx;
}

void Editor::scroll() {
    renderX = 0;

    if (cursorY < editor_lines.size()) {
        renderX = cursor_to_renderX();
    }

    // UP
    if (cursorY < row_offset)
        row_offset = cursorY;

    // Down
    if (cursorY >= row_offset + t.rows())
        row_offset = cursorY - t.rows() + 1;

    // Left
    if (renderX < col_offset)
        col_offset = renderX;

    // Right
    if (renderX >= col_offset + t.cols())
        col_offset = renderX - t.cols() + 1;

    g.go_to(pos());
}