#include "../include/display.hpp"

const int Display::LEFT_MARGIN = 10;
const int Display::TOP_MARGIN = 3;
const int Display::ORGAN_WIDTH = 52;
const int Display::ORGAN_HEIGHT = 7;
const int Display::VVESSEL_WIDTH = 5;
const int Display::HVESSEL_WIDTH = 5;
const int Display::VVEIN_START = LEFT_MARGIN + 1;
const int Display::HVEIN_START = VVEIN_START + VVESSEL_WIDTH;
const int Display::ORGAN_START = HVEIN_START + HVESSEL_WIDTH;
const int Display::HARTERY_START = ORGAN_START + ORGAN_WIDTH;
const int Display::VARTERY_START = HARTERY_START + HVESSEL_WIDTH;

Display::Display()
    : _kill_switch(false)
    , _color_counter(9)
    , _draw_start(6)
{
    init_sequence();
    make_color_pairs();
    create_title_frame();
    create_organ(0, "LUNGS", 3);
    create_heart(8);
    create_organ(21, "BRAIN", 5);
    create_organ(29, "DIGESTIVE SYSTEM", 6);
    create_organ(37, "SPLEEN", 7);
    create_organ(45, "BONE MARROW", 8);
    create_left_side_vessels(4);
    create_right_side_vessels(4);
}

Display::~Display()
{
    std::for_each(_all_windows.begin(), _all_windows.end(), [this](auto a) { destroy_window(a); });
    endwin();
}

std::atomic<bool>& Display::get_kill_switch()
{
    return std::ref(_kill_switch);
}

std::condition_variable& Display::get_start_cv()
{
    return std::ref(_start_cv);
}

std::mutex& Display::get_start_mutex()
{
    return std::ref(_start_mutex);
}

std::mutex& Display::get_display_mutex()
{
    return std::ref(_display_mutex);
}

void Display::init_sequence()
{
    initscr();
    raw();
    noecho();
    curs_set(false);
    start_color();
    use_default_colors();
    keypad(stdscr, true);
    refresh();
}

void Display::destroy_window(WINDOW* win)
{
    wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(win);
    delwin(win);
}

void Display::make_color_pairs()
{
    // blue vessels
    init_pair(1, 31, -1);
    // red vessels
    init_pair(2, 124, -1);
    // lungs
    init_pair(3, 4, -1);
    // heart
    init_pair(4, 13, -1);
    // brain
    init_pair(5, 72, -1);
    // digestive system
    init_pair(6, 221, -1);
    // spleen
    init_pair(7, 12, -1);
    // bone marrow
    init_pair(8, 231, -1);

    // new_oxy
    init_pair(9, 196, -1);
    // normal_oxy
    init_pair(10, 124, -1);
    // old_oxy
    init_pair(11, 52, -1);
    // new_deoxy
    init_pair(12, 45, -1);
    // normal_deoxy
    init_pair(13, 21, -1);
    // old_deoxy
    init_pair(14, 18, -1);
    // decayed
    init_pair(15, 241, -1);
}

void Display::create_title_frame()
{
    WINDOW* title_window = ::newwin(3, ::COLS, 0, 0);
    _all_windows.push_back(title_window);
    wattron(title_window, COLOR_PAIR(4));
    box(title_window, 0, 0);
    wattroff(title_window, COLOR_PAIR(4));

    wattron(title_window, COLOR_PAIR(6));
    mvwprintw(title_window, 1, ::COLS / 2 - 25, "Simple Metabolism :)");
    wattroff(title_window, COLOR_PAIR(6));
    wrefresh(title_window);
}

WINDOW* Display::create_organ(int vpos, const char* name, int cpair_no)
{
    WINDOW* organ = ::newwin(ORGAN_HEIGHT, ORGAN_WIDTH, TOP_MARGIN + vpos, ORGAN_START);
    _all_windows.push_back(organ);
    _windows[std::string(name)] = organ;

    wattron(organ, COLOR_PAIR(cpair_no));
    // horizontal lines
    mvwhline(organ, 0, 1, ACS_HLINE, ORGAN_WIDTH - 2);
    mvwhline(organ, 4, 1, ACS_HLINE, ORGAN_WIDTH - 2);
    mvwhline(organ, 6, 0, ACS_HLINE, ORGAN_WIDTH);
    // vertical lines
    mvwvline(organ, 1, 0, ACS_VLINE, 3);
    mvwvline(organ, 1, ORGAN_WIDTH - 1, ACS_VLINE, 3);

    // corners
    mvwaddch(organ, 0, 0, ACS_ULCORNER);
    mvwaddch(organ, 0, ORGAN_WIDTH - 1, ACS_URCORNER);

    // vessel entry & exit
    mvwaddch(organ, 4, 0, ACS_BTEE);
    mvwaddch(organ, 4, ORGAN_WIDTH - 1, ACS_BTEE);
    // text info
    mvwprintw(organ, 1, 2, name);
    mvwprintw(organ, 1, 38, "Health: XXX%%");
    wattroff(organ, COLOR_PAIR(cpair_no));
    // create vessels
    wrefresh(organ);
    return organ;
}

WINDOW* Display::create_heart(int vpos)
{
    WINDOW* heart = ::newwin(12, ORGAN_WIDTH, TOP_MARGIN + vpos, ORGAN_START);
    _all_windows.push_back(heart);
    _windows[std::string("HEART")] = heart;

    wattron(heart, COLOR_PAIR(4));
    // horizontal lines
    mvwhline(heart, 0, 1, ACS_HLINE, ORGAN_WIDTH - 2);
    mvwhline(heart, 9, 1, ACS_HLINE, ORGAN_WIDTH - 2);
    mvwhline(heart, 11, 0, ACS_HLINE, ORGAN_WIDTH);
    // vertical lines
    mvwvline(heart, 1, 0, ACS_VLINE, 2);
    mvwvline(heart, 1, ORGAN_WIDTH - 1, ACS_VLINE, 2);

    // corners
    mvwaddch(heart, 0, 0, ACS_ULCORNER);
    mvwaddch(heart, 0, ORGAN_WIDTH - 1, ACS_URCORNER);

    // vessel entry & exit
    mvwaddch(heart, 9, 0, ACS_BTEE);
    mvwaddch(heart, 9, ORGAN_WIDTH - 1, ACS_BTEE);
    // text info
    mvwprintw(heart, 1, 2, "HEART");
    mvwprintw(heart, 1, 38, "Health: XXX%%");
    wattroff(heart, COLOR_PAIR(4));

    // left heart (marked as right heart!)
    wattron(heart, COLOR_PAIR(1));
    mvwhline(heart, 3, 1, ACS_HLINE, 20);
    mvwhline(heart, 5, 1, ACS_HLINE, 17);
    mvwhline(heart, 6, 1, ACS_HLINE, 17);
    mvwhline(heart, 8, 1, ACS_HLINE, 20);
    mvwvline(heart, 4, 21, ACS_VLINE, 4);

    // entry
    mvwaddch(heart, 3, 0, ACS_BTEE);
    mvwaddch(heart, 5, 0, ACS_TTEE);
    mvwaddch(heart, 6, 0, ACS_BTEE);
    mvwaddch(heart, 8, 0, ACS_TTEE);
    // inner corners
    mvwaddch(heart, 3, 21, ACS_URCORNER);
    mvwaddch(heart, 8, 21, ACS_LRCORNER);
    mvwaddch(heart, 5, 17, ACS_URCORNER);
    mvwaddch(heart, 6, 17, ACS_LRCORNER);
    wattroff(heart, COLOR_PAIR(1));
    // right heart (marked as left heart!)
    wattron(heart, COLOR_PAIR(2));
    mvwhline(heart, 3, 31, ACS_HLINE, 20);
    mvwhline(heart, 5, 35, ACS_HLINE, 17);
    mvwhline(heart, 6, 35, ACS_HLINE, 17);
    mvwhline(heart, 8, 31, ACS_HLINE, 20);
    mvwvline(heart, 4, 30, ACS_VLINE, 4);

    // entry
    mvwaddch(heart, 3, 51, ACS_BTEE);
    mvwaddch(heart, 5, 51, ACS_TTEE);
    mvwaddch(heart, 6, 51, ACS_BTEE);
    mvwaddch(heart, 8, 51, ACS_TTEE);
    // inner corners
    mvwaddch(heart, 3, 30, ACS_ULCORNER);
    mvwaddch(heart, 8, 30, ACS_LLCORNER);
    mvwaddch(heart, 5, 34, ACS_ULCORNER);
    mvwaddch(heart, 6, 34, ACS_LLCORNER);
    wattroff(heart, COLOR_PAIR(2));
    wrefresh(heart);
    return heart;
}

WINDOW* Display::create_left_side_vessels(int vpos)
{
    WINDOW* vessels = ::newwin(48, VVESSEL_WIDTH + HVESSEL_WIDTH, TOP_MARGIN + vpos, VVEIN_START);
    _all_windows.push_back(vessels);
    _windows[std::string("LVESSELS")] = vessels;
    //left border
    wattron(vessels, COLOR_PAIR(1));
    // pulmonary circulation
    // vertical
    mvwvline(vessels, 1, 0, ACS_VLINE, 8);
    mvwvline(vessels, 3, VVESSEL_WIDTH - 1, ACS_VLINE, 4);
    // horizontal
    mvwhline(vessels, 0, 1, ACS_HLINE, VVESSEL_WIDTH + HVESSEL_WIDTH);
    mvwhline(vessels, 9, 1, ACS_HLINE, VVESSEL_WIDTH + HVESSEL_WIDTH);
    mvwhline(vessels, 2, 5, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 7, 5, ACS_HLINE, HVESSEL_WIDTH);
    // corners
    mvwaddch(vessels, 0, 0, ACS_ULCORNER);
    mvwaddch(vessels, 2, 4, ACS_ULCORNER);
    mvwaddch(vessels, 9, 0, ACS_LLCORNER);
    mvwaddch(vessels, 7, 4, ACS_LLCORNER);

    // systemic circulation
    // vertical
    mvwvline(vessels, 11, 0, ACS_VLINE, 36);
    mvwvline(vessels, 16, VVESSEL_WIDTH - 1, ACS_VLINE, 5);
    mvwvline(vessels, 24, VVESSEL_WIDTH - 1, ACS_VLINE, 5);
    mvwvline(vessels, 32, VVESSEL_WIDTH - 1, ACS_VLINE, 5);
    mvwvline(vessels, 40, VVESSEL_WIDTH - 1, ACS_VLINE, 5);

    // horizontal
    mvwhline(vessels, 10, 1, ACS_HLINE, VVESSEL_WIDTH + HVESSEL_WIDTH);
    mvwhline(vessels, 12, 5, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 13, 5, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 15, 5, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 21, 5, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 23, 5, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 29, 5, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 31, 5, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 37, 5, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 39, 5, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 45, 5, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 47, 1, ACS_HLINE, VVESSEL_WIDTH + HVESSEL_WIDTH);
    // corners outer
    mvwaddch(vessels, 10, 0, ACS_ULCORNER);
    mvwaddch(vessels, 47, 0, ACS_LLCORNER);
    // corners inner
    mvwaddch(vessels, 12, 4, ACS_ULCORNER);
    mvwaddch(vessels, 13, 4, ACS_LLCORNER);
    mvwaddch(vessels, 15, 4, ACS_ULCORNER);
    mvwaddch(vessels, 21, 4, ACS_LLCORNER);
    mvwaddch(vessels, 23, 4, ACS_ULCORNER);
    mvwaddch(vessels, 29, 4, ACS_LLCORNER);
    mvwaddch(vessels, 31, 4, ACS_ULCORNER);
    mvwaddch(vessels, 37, 4, ACS_LLCORNER);
    mvwaddch(vessels, 39, 4, ACS_ULCORNER);
    mvwaddch(vessels, 45, 4, ACS_LLCORNER);
    wattroff(vessels, COLOR_PAIR(1));
    wrefresh(vessels);
    return vessels;
}

WINDOW* Display::create_right_side_vessels(int vpos)
{
    WINDOW* vessels = ::newwin(48, VVESSEL_WIDTH + HVESSEL_WIDTH, TOP_MARGIN + vpos, HARTERY_START);
    _all_windows.push_back(vessels);
    _windows[std::string("RVESSELS")] = vessels;

    //left border
    wattron(vessels, COLOR_PAIR(2));
    // pulmonary circulation
    // vertical
    mvwvline(vessels, 1, VVESSEL_WIDTH + HVESSEL_WIDTH - 1, ACS_VLINE, 8);
    mvwvline(vessels, 3, HVESSEL_WIDTH, ACS_VLINE, 4);
    // horizontal
    mvwhline(vessels, 0, 0, ACS_HLINE, VVESSEL_WIDTH + HVESSEL_WIDTH);
    mvwhline(vessels, 9, 0, ACS_HLINE, VVESSEL_WIDTH + HVESSEL_WIDTH);
    mvwhline(vessels, 2, 0, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 7, 0, ACS_HLINE, HVESSEL_WIDTH);
    // corners
    mvwaddch(vessels, 0, VVESSEL_WIDTH + HVESSEL_WIDTH - 1, ACS_URCORNER);
    mvwaddch(vessels, 2, 5, ACS_URCORNER);
    mvwaddch(vessels, 9, VVESSEL_WIDTH + HVESSEL_WIDTH - 1, ACS_LRCORNER);
    mvwaddch(vessels, 7, 5, ACS_LRCORNER);

    // systemic circulation
    // vertical
    mvwvline(vessels, 11, VVESSEL_WIDTH + HVESSEL_WIDTH - 1, ACS_VLINE, 36);
    mvwvline(vessels, 16, HVESSEL_WIDTH, ACS_VLINE, 5);
    mvwvline(vessels, 24, HVESSEL_WIDTH, ACS_VLINE, 5);
    mvwvline(vessels, 32, HVESSEL_WIDTH, ACS_VLINE, 5);
    mvwvline(vessels, 40, HVESSEL_WIDTH, ACS_VLINE, 5);

    // horizontal
    mvwhline(vessels, 10, 0, ACS_HLINE, VVESSEL_WIDTH + HVESSEL_WIDTH - 1);
    mvwhline(vessels, 12, 0, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 13, 0, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 15, 0, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 21, 0, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 23, 0, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 29, 0, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 31, 0, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 37, 0, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 39, 0, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 45, 0, ACS_HLINE, HVESSEL_WIDTH);
    mvwhline(vessels, 47, 0, ACS_HLINE, VVESSEL_WIDTH + HVESSEL_WIDTH - 1);
    // corners outer
    mvwaddch(vessels, 10, VVESSEL_WIDTH + HVESSEL_WIDTH - 1, ACS_URCORNER);
    mvwaddch(vessels, 47, VVESSEL_WIDTH + HVESSEL_WIDTH - 1, ACS_LRCORNER);
    // corners inner
    mvwaddch(vessels, 12, 5, ACS_URCORNER);
    mvwaddch(vessels, 13, 5, ACS_LRCORNER);
    mvwaddch(vessels, 15, 5, ACS_URCORNER);
    mvwaddch(vessels, 21, 5, ACS_LRCORNER);
    mvwaddch(vessels, 23, 5, ACS_URCORNER);
    mvwaddch(vessels, 29, 5, ACS_LRCORNER);
    mvwaddch(vessels, 31, 5, ACS_URCORNER);
    mvwaddch(vessels, 37, 5, ACS_LRCORNER);
    mvwaddch(vessels, 39, 5, ACS_URCORNER);
    mvwaddch(vessels, 45, 5, ACS_LRCORNER);
    wattroff(vessels, COLOR_PAIR(2));
    wrefresh(vessels);
    return vessels;
}

void Display::update_rbc_position(std::tuple<unsigned, unsigned, unsigned, unsigned> positions, std::tuple<bool, bool, bool> resources, RBC_State state)
{
    auto rbc_x = std::get<0>(positions), rbc_y = std::get<1>(positions);
    auto rbc_prev_x = std::get<2>(positions), rbc_prev_y = std::get<3>(positions);
    delete_prev_rbc_pos(rbc_prev_x, rbc_prev_y);
    auto color = get_rbc_color(resources, state);
    add_current_rbc_pos(rbc_x, rbc_y, color);
}

void Display::delete_prev_rbc_pos(unsigned pos_x, unsigned pos_y)
{
    // mvwprintw(_windows.at("LUNGS"), 2, 1, "PrevX:%2d\tPrevY:%2d", pos_x, pos_y);
    // wrefresh(_windows.at("LUNGS"));
    std::lock_guard lg{ _display_mutex };
    if (pos_x < 8) {
        mvwaddch(_windows.at("LVESSELS"), pos_y + 1, pos_x + 2, ' ');
        wrefresh(_windows.at("LVESSELS"));
    } else if (pos_x > 59) {
        mvwaddch(_windows.at("RVESSELS"), pos_y + 1, pos_x - 60, ' ');
        wrefresh(_windows.at("RVESSELS"));
    } else if (pos_x >= 8 && pos_x <= 59) {
        if (pos_y == 0) {
            mvwaddch(_windows.at("LUNGS"), 5, pos_x - 8, ' ');
            wrefresh(_windows.at("LUNGS"));
        } else if (pos_y >= 7 && pos_y <= 13) {
            mvwaddch(_windows.at("HEART"), pos_y - 3, pos_x - 8, ' ');
            wrefresh(_windows.at("HEART"));
        } else if (pos_y == 21) {
            mvwaddch(_windows.at("BRAIN"), pos_y - 16, pos_x - 8, ' ');
            wrefresh(_windows.at("BRAIN"));
        } else if (pos_y == 29) {
            mvwaddch(_windows.at("DIGESTIVE SYSTEM"), pos_y - 24, pos_x - 8, ' ');
            wrefresh(_windows.at("DIGESTIVE SYSTEM"));
        } else if (pos_y == 37) {
            mvwaddch(_windows.at("SPLEEN"), pos_y - 32, pos_x - 8, ' ');
            wrefresh(_windows.at("SPLEEN"));
        } else if (pos_y == 45) {
            mvwaddch(_windows.at("BONE MARROW"), pos_y - 40, pos_x - 8, ' ');
            wrefresh(_windows.at("BONE MARROW"));
        }
    }
}

void Display::add_current_rbc_pos(unsigned pos_x, unsigned pos_y, int rbc_color)
{
    // mvwprintw(_windows.at("LUNGS"), 3, 1, "PosX:%2d\tPosY:%2d", pos_x, pos_y);
    // wrefresh(_windows.at("LUNGS"));
    if (pos_x < 8) {
        draw_rbc_in_window(_windows.at("LVESSELS"), pos_y + 1, pos_x + 2, rbc_color);
    } else if (pos_x > 59) {
        draw_rbc_in_window(_windows.at("RVESSELS"), pos_y + 1, pos_x - 60, rbc_color);
    } else if (pos_x >= 8 && pos_x <= 59) {
        if (pos_y == 0) {
            draw_rbc_in_window(_windows.at("LUNGS"), 5, pos_x - 8, rbc_color);
        } else if (pos_y >= 7 && pos_y <= 13) {
            draw_rbc_in_window(_windows.at("HEART"), pos_y - 3, pos_x - 8, rbc_color);
        } else if (pos_y == 21) {
            draw_rbc_in_window(_windows.at("BRAIN"), pos_y - 16, pos_x - 8, rbc_color);
        } else if (pos_y == 29) {
            draw_rbc_in_window(_windows.at("DIGESTIVE SYSTEM"), pos_y - 24, pos_x - 8, rbc_color);
        } else if (pos_y == 37) {
            draw_rbc_in_window(_windows.at("SPLEEN"), pos_y - 32, pos_x - 8, rbc_color);
        } else if (pos_y == 45) {
            draw_rbc_in_window(_windows.at("BONE MARROW"), pos_y - 40, pos_x - 8, rbc_color);
        }
    }
}

int Display::get_rbc_color(std::tuple<bool, bool, bool> resources, RBC_State state)
{
    if (state == RBC_State::NEW) {
        if (std::get<0>(resources) == true)
            return 9;
        else
            return 12;
    } else if (state == RBC_State::NORMAL) {
        if (std::get<0>(resources) == true)
            return 10;
        else
            return 13;
    } else if (state == RBC_State::OLD) {
        if (std::get<0>(resources) == true)
            return 11;
        else
            return 14;
    } else
        return 15;
}

void Display::draw_rbc_in_window(WINDOW* win, int pos_y, int pos_x, int color)
{
    std::lock_guard lg{ _display_mutex };
    wattron(win, COLOR_PAIR(color));
    mvwaddch(win, pos_y, pos_x, ACS_DIAMOND);
    wattroff(win, COLOR_PAIR(color));
    wrefresh(win);
}

void Display::update_heart_state(int health, std::tuple<bool, bool, bool> res)
{
    std::lock_guard lg{ _display_mutex };
    WINDOW* win = _windows.at("HEART");
    mvwprintw(win, 1, 46, "%03d%%", health);
    mvwprintw(win, 2, 1, "O2: %d\tGLUCOSE: %d\tCO2: %d", std::get<0>(res), std::get<1>(res), std::get<2>(res));
    wrefresh(win);
}

void Display::update_organ_state(std::string organ, int percentage, int health, std::tuple<bool, bool, bool> res)
{
    std::lock_guard lg{ _display_mutex };
    WINDOW* win = _windows.at(organ);
    mvwprintw(win, 1, 46, "%03d%%", health);
    mvwprintw(win, 2, 2, "O2: %d\tGLUCOSE: %d\tCO2: %d", std::get<0>(res), std::get<1>(res), std::get<2>(res));
    update_progress_bar(win, percentage);
    wrefresh(win);
}

void Display::update_progress_bar(WINDOW* win, int percentage)
{
    mvwprintw(win, 3, 2, "Task status:");
    mvwaddch(win, 3, 29, '[');
    mvwaddch(win, 3, 50, ']');
    for (int i = 0; i < 20; ++i) {
        if (percentage / 5 > i)
            mvwaddch(win, 3, 30 + i, '#');
        else
            mvwaddch(win, 3, 30 + i, '-');
    }
}

void Display::main_loop()
{
    while (wgetch(stdscr) != 27) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    _kill_switch = true;
}
