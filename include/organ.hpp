#pragma once
#include "brain_signal.hpp"
#include "display.hpp"
#include "rbc.hpp"
#include <atomic>
#include <functional>
#include <list>

class Organ {

    static const unsigned XRANGE_START;
    static const unsigned XRANGE_END;

    unsigned _o2;
    unsigned _glu;
    unsigned _co2;

    unsigned _low_o2_count;
    unsigned _low_glu_count;

protected:
    std::string _name;
    unsigned _y;
    unsigned _health;
    std::atomic<float> _metabolism_speed;
    Display& _dp_controller;
    std::atomic<bool>& _kill_switch;
    std::list<RBC>& _rbc_pool;
    std::mutex& _list_mutex;
    std::mutex _own_mutex;
    std::function<void(Brain_Signal sig)> _brain_callback;

public:
    Organ(std::string name, unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex);
    void nourish();
    void accumulate_resources(RBC& rbc);
    void deposit_resources(RBC& rbc);
    void health_decay();
    void receive_callback(std::function<void(Brain_Signal)> cb);
    void inform_brain();
    void increase_metabolism();
    void decrease_metabolism();
    std::tuple<unsigned, unsigned, unsigned> get_resources_state();
};