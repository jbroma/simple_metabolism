#include "../include/organ.hpp"

const unsigned Organ::XRANGE_START = 10;
const unsigned Organ::XRANGE_END = 61;

Organ::Organ(std::string name, unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex)
    : _o2(false)
    , _glu(false)
    , _co2(false)
    , _low_o2_count(0)
    , _low_glu_count(0)
    , _name(name)
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
    if (_health < 75 && _o2 == 6 && _glu && !_co2) {
        _o2 = 0;
        _glu = 0;
        _co2 = 6;
        _health += 25;
    }

    std::lock_guard lg{ _list_mutex };
    for (auto& rbc : _rbc_pool) {
        auto xy = rbc.get_position();
        if (xy.second == _y && xy.first >= 12 && xy.first <= 55) {
            accumulate_resources(rbc);
            deposit_resources(rbc);
        }
    }
}

void Organ::accumulate_resources(RBC& rbc)
{
    if (_o2 < 6 && rbc.get_o2()) {
        _o2 += 1;
        _low_o2_count = 0;
    } else {
        _low_o2_count += 1;
    }
    if (!_glu && rbc.get_glu()) {
        _glu += 1;
        _low_glu_count = 0;
    } else {
        _low_glu_count += 1;
    }
}

void Organ::deposit_resources(RBC& rbc)
{
    if (_co2 && rbc.store_co2()) {
        _co2 -= 1;
    }
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
    if (_health < 35) {
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
    if (_metabolism_speed <= 1.6)
        _metabolism_speed = _metabolism_speed.load() * 1.1;
}

void Organ::decrease_metabolism()
{
    if (_metabolism_speed >= 0.4)
        _metabolism_speed = _metabolism_speed.load() * 0.99;
}

std::tuple<unsigned, unsigned, unsigned> Organ::get_resources_state()
{
    return { _o2, _glu, _co2 };
}
