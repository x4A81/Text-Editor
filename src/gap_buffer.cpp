#include "../include/gap_buffer.hpp"


void Gap_Buffer::insert_char(char c) {
    if (gap_begin == gap_end) {
        // grow
        grow();
    }

    buffer[gap_begin++] = c;
}

void Gap_Buffer::grow() {
    size_t new_size = buffer.empty() ? 64 : buffer.size() * 2;
    std::string new_buffer(new_size, '\0');

    // copy left
    std::copy(
        buffer.begin(),
        buffer.begin() + gap_begin,
        new_buffer.begin()
    );

    // copy right
    size_t suffix = buffer.size() - gap_end;
    size_t new_gap_end = new_size - suffix;

    std::copy(
        buffer.begin() + gap_end,
        buffer.end(),
        new_buffer.begin() + new_gap_end);

    buffer = std::move(new_buffer);
    gap_end = new_gap_end;
}

void Gap_Buffer::erase() {
    if (gap_begin == 0)
        return;

    --gap_begin;
}

void Gap_Buffer::move_left() {
    if (gap_begin == 0) return;
    buffer[gap_end - 1] = buffer[gap_begin - 1];

    --gap_begin;
    --gap_end;
}

void Gap_Buffer::move_right() {
    if (gap_end == buffer.size()) return;
    buffer[gap_begin] = buffer[gap_end];

    ++gap_begin;
    ++gap_end;
}

void Gap_Buffer::go_to(int pos) {
    while (gap_begin < pos)
        move_right();

    while (gap_begin > pos)
        move_left();
}

void Gap_Buffer::load_text(const std::string &text) {
    size_t gap = 64;
    buffer = text;
    buffer.resize(text.size() + gap);

    gap_begin = text.size();
    gap_end = text.size() + gap;
}

std::string Gap_Buffer::to_string() {
    std::string out;
    out.append(buffer, 0, gap_begin);
    out.append(buffer, gap_end, buffer.size() - gap_end);

    return out;
}