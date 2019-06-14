#pragma once
#include "display.hpp"
#include "rbc_state.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <random>

class RBC {

    static thread_local std::mt19937 _gen;

    Display& _dp_controller;
    std::atomic<bool>& _kill_switch;
    std::condition_variable& _start_cv;
    std::mutex& _start_mutex;
    std::mutex _own_mutex;

    unsigned _pos_x;
    unsigned _pos_y;

    bool _o2;
    bool _co2;
    bool _glu;

    unsigned _prev_x;
    unsigned _prev_y;

    unsigned _next_x;
    unsigned _next_y;

    unsigned _days_left;
    std::atomic<bool> _dead;
    std::thread _life_thread;

    RBC_State _state;

public:
    RBC(unsigned start_x, unsigned start_y, Display& controller);
    ~RBC();
    void advance_pos();
    void calc_new_pos();

    unsigned get_x();
    unsigned get_y();

    bool get_o2();
    bool get_glu();
    bool get_co2();

    bool store_o2();
    bool store_glu();
    bool store_co2();

    bool check_o2();
    bool check_glu();
    bool check_co2();

    void decay();
    void update_state();

    bool get_random_direction();
    void run();

    std::tuple<unsigned, unsigned, unsigned, unsigned> get_dpositions();
    std::tuple<bool, bool, bool> get_dresources();
    RBC_State get_dstate();

    std::mutex& get_rbc_mutex();
    void destroy();
};