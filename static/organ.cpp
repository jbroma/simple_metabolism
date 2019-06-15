#include "../include/organ.hpp"

const unsigned Organ::XRANGE_START = 10;
const unsigned Organ::XRANGE_END = 61;

Organ::Organ(unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex)
    : _o2(false)
    , _glu(false)
    , _co2(false)
    , _low_o2_count(0)
    , _low_glu_count(0)
    , _y(pos_y)
    , _health(100)
    , _metabolism_speed(1.0)
    , _dp_controller(controller)
    , _kill_switch(controller.get_kill_switch())
    , _rbc_pool(rbc_pool)
    , _list_mutex(list_mutex)
{
}

void Organ::nourish()
{
    // if (_health < 50) {
    //     for (auto& rbc : _rbc_pool) {
    //         if (_o2 && _glu)
    //             break;
    //         if (rbc.get_y() == _y && rbc.get_x() >= 10 && rbc.get_x() <= 61) {
    //             if (!_o2 && rbc.get_o2()) {
    //                 _o2 = true;
    //                 _low_o2_count = 0;
    //             } else {
    //                 _low_o2_count += 1;
    //             }
    //             if (!_glu && rbc.get_glu()) {
    //                 _glu = true;
    //                 _low_glu_count = 0;
    //             } else {
    //                 _low_glu_count += 1;
    //             }
    //             if (_co2 && rbc.store_co2()) {
    //                 _co2 = false;
    //             }
    //         }
    //     }
    //     if (_o2 && _glu && !_co2) {
    //         _health = 100;
    //         _o2 = false;
    //         _glu = false;
    //         _co2 = true;
    //     }
    // }
}

void Organ::health_decay()
{
    if (_health > 0) {
        _health -= 1;
    }
}

void Organ::receive_callback(std::function<void(Brain_Signal)> cb)
{
    _brain_callback = cb;
}

void Organ::inform_brain()
{
    if (_health < 20) {
        _brain_callback(Brain_Signal::CRITICAL_HEALTH);
    } else if (_low_o2_count > 10) {
        _brain_callback(Brain_Signal::LOW_O2);
    } else if (_low_glu_count > 10) {
        _brain_callback(Brain_Signal::LOW_GLUCOSE);
    } else
        _brain_callback(Brain_Signal::ALL_GOOD);
}

void Organ::increase_metabolism()
{
    _metabolism_speed = _metabolism_speed.load() * 1.1;
}

void Organ::decrease_metabolism()
{
    _metabolism_speed = _metabolism_speed.load() * 0.9;
}

std::tuple<bool, bool, bool> Organ::get_resources_state()
{
    return { _o2, _glu, _co2 };
}
