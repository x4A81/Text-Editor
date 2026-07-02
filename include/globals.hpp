#pragma once

inline constexpr auto VERSION = "0.0.1";

inline constexpr int TAB_STOP = 8;

enum class Keys : int {
    BACK_SPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    DEL_KEY,
    HOME_KEY,
    END_KEY,
    PAGE_UP,
    PAGE_DOWN,
};

inline int keys_to_int(Keys key) {
    return static_cast<int>(key);
}