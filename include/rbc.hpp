#pragma once
#include "display.hpp"
#include "rbc_state.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <random>

class RBC {

    static std::mt19937 _gen;
    static std::mutex _rbc_shared_mutex;

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

    unsigned _next_organ;

    unsigned _reciprocal_velocity;

    unsigned _days_left;
    std::atomic<bool> _dead;
    std::thread _life_thread;

    RBC_State _state;

public:
    RBC(unsigned start_x, unsigned start_y, Display& controller, RBC_State init_state);
    ~RBC();
    void advance_pos();
    void calc_new_pos();

    unsigned get_x();
    unsigned get_y();
    std::pair<unsigned, unsigned> get_position();

    bool get_o2();
    bool get_glu();
    bool get_co2();

    bool store_o2();
    bool store_glu();
    bool store_co2();

    void set_rvelocity(unsigned rv);
    void pass_through_organ();

    void health_decay();
    void update_state();

    int choose_next_organ();
    void run();

    std::tuple<unsigned, unsigned, unsigned, unsigned> get_dpositions();
    std::tuple<bool, bool, bool> get_dresources();
    RBC_State get_dstate();

    void destroy();
};