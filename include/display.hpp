#pragma once
#include <algorithm>
#include <atomic>
#include <condition_variable>
#include <cstring>
#include <deque>
#include <iostream>
#include <list>
#include <map>
#include <ncurses.h>
#include <thread>

#include "rbc_state.hpp"

class Display {

    static const int LEFT_MARGIN;
    static const int TOP_MARGIN;
    static const int ORGAN_WIDTH;
    static const int ORGAN_HEIGHT;
    static const int VESSEL_WIDTH;
    static const int VVEIN_START;
    static const int VARTERY_START;
    static const int ORGAN_START;
    static const int HVEIN_START;
    static const int HARTERY_START;
    static const int VVESSEL_WIDTH;
    static const int HVESSEL_WIDTH;

    std::condition_variable _start_cv;
    std::mutex _start_mutex, _display_mutex;
    std::atomic<bool> _kill_switch;
    int _color_counter;
    unsigned _draw_start;

    std::map<std::string, WINDOW*> _windows;
    std::list<WINDOW*> _all_windows;

public:
    Display();
    ~Display();

    std::atomic<bool>& get_kill_switch();
    std::condition_variable& get_start_cv();
    std::mutex& get_start_mutex();

    void init_sequence();
    void make_color_pairs();
    void destroy_window(WINDOW* win);
    void main_loop();

    // creates a window
    void create_title_frame();
    WINDOW* create_organ(int vpos, const char* name, int cpair_no);
    WINDOW* create_heart(int vpos);
    WINDOW* create_left_side_vessels(int vpos);
    WINDOW* create_right_side_vessels(int vpos);

    // updates window contents
    void update_rbc_position(std::tuple<unsigned, unsigned, unsigned, unsigned> positions, std::tuple<bool, bool, bool> resources, RBC_State state);
    void delete_prev_rbc_pos(unsigned pos_x, unsigned pos_y);
    void add_current_rbc_pos(unsigned pos_x, unsigned pos_y, int rbc_color);
    int get_rbc_color(std::tuple<bool, bool, bool> resources, RBC_State state);
    void draw_rbc_in_window(WINDOW* win, int pos_y, int pos_x, int color);
    void update_heart_state(int health, std::tuple<unsigned, unsigned, unsigned> res, float mb);
    void update_organ_state(std::string organ, int percentage, int health, std::tuple<unsigned, unsigned, unsigned> res, float mb);
    void update_progress_bar(WINDOW* win, int percentage);
    int get_progress_bar_color(int percentage);
};