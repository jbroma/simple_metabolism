#include "../include/rbc.hpp"

std::mt19937 RBC::_gen{};
std::mutex RBC::_rbc_shared_mutex{};

RBC::RBC(unsigned start_x, unsigned start_y, Display& controller, RBC_State init_state)
    : _dp_controller(controller)
    , _kill_switch(controller.get_kill_switch())
    , _start_cv(controller.get_start_cv())
    , _start_mutex(controller.get_start_mutex())
    , _pos_x(start_x)
    , _pos_y(start_y)
    , _o2(false)
    , _co2(false)
    , _glu(false)
    , _next_x(_pos_x)
    , _next_y(_pos_y)
    , _next_organ(0)
    , _reciprocal_velocity(50)
    , _dead(false)
    , _life_thread(&RBC::run, this)
    , _state(init_state)
{
    std::uniform_int_distribution<> dist(500, 800);
    std::lock_guard lg{ _rbc_shared_mutex };
    _days_left = dist(_gen);
}

RBC::~RBC()
{
    _life_thread.detach();
}

void RBC::calc_new_pos()
{
    if (_pos_y == 0) {
        if (_pos_x == 67) {
            _next_x = _pos_x;
            _next_y = _pos_y + 1;
        } else {
            _next_x = _pos_x + 1;
            _next_y = _pos_y;
        }
    } else if (_pos_y >= 1 && _pos_y <= 6) {
        _next_x = _pos_x;
        if (_pos_x == 0) {
            _next_y = _pos_y - 1;
        } else {
            _next_y = _pos_y + 1;
        }
    } else if (_pos_y == 7) {
        if (_pos_x == 0) {
            _next_x = _pos_x;
            _next_y = _pos_y - 1;
        } else if (_pos_x == 40) {
            _next_x = _pos_x;
            _next_y = _pos_y + 1;
        } else {
            _next_x = _pos_x - 1;
            _next_y = _pos_y;
        }
    } else if (_pos_y >= 8 && _pos_y <= 9) {
        if (_pos_y == 8) {
            _next_organ = choose_next_organ();
        }
        _next_x = _pos_x;
        if (_pos_x < 29) {
            _next_y = _pos_y - 1;
        } else {
            _next_y = _pos_y + 1;
        }
    } else if (_pos_y == 10) {
        if (_pos_x == 27) {
            _next_x = _pos_x;
            _next_y = _pos_y - 1;
        } else if (_pos_x == 67) {
            _next_x = _pos_x;
            _next_y = _pos_y + 1;
        } else {
            _next_x = _pos_x + 1;
            _next_y = _pos_y;
        }
    } else {
        if (_pos_x == 0) {
            _next_x = _pos_x;
            _next_y = _pos_y - 1;
        } else if (_pos_x == 67) {
            if (_pos_y == 13 || _pos_y == 21 || _pos_y == 29 || _pos_y == 37) {
                if ((_pos_y == 13 && _next_organ == 0) || (_pos_y == 21 && _next_organ == 1) || (_pos_y == 29 && _next_organ == 2) || (_pos_y == 37 && _next_organ == 3)) {
                    _next_x = _pos_x - 1;
                    _next_y = _pos_y;
                } else {
                    _next_x = _pos_x;
                    _next_y = _pos_y + 1;
                }
            } else if (_pos_y == 45) {
                _next_x = _pos_x - 1;
                _next_y = _pos_y;
            } else {
                _next_x = _pos_x;
                _next_y = _pos_y + 1;
            }
        } else {
            _next_x = _pos_x - 1;
            _next_y = _pos_y;
        }
    }
}

void RBC::advance_pos()
{
    std::lock_guard lg{ _own_mutex };
    pass_through_organ();
    calc_new_pos();
    _prev_x = _pos_x;
    _prev_y = _pos_y;
    _pos_x = _next_x;
    _pos_y = _next_y;
}

void RBC::pass_through_organ()
{
    if (_pos_y == 0 || _pos_y == 13 || _pos_y == 21 || _pos_y == 29 || _pos_y == 37 || _pos_y == 45) {
        if (_pos_x == 60 || _pos_x == 50 || _pos_x == 40 || _pos_x == 30) {
            _reciprocal_velocity *= 1.3;
        }
    }
}

int RBC::choose_next_organ()
{
    static std::uniform_int_distribution<> dist(0, 4);
    std::lock_guard lg{ _rbc_shared_mutex };
    return dist(_gen);
}

unsigned RBC::get_x()
{
    return _pos_x;
}

unsigned RBC::get_y()
{
    return _pos_y;
}

std::pair<unsigned, unsigned> RBC::get_position()
{
    std::lock_guard lg{ _own_mutex };
    return { get_x(), get_y() };
}

bool RBC::get_o2()
{
    std::lock_guard lg{ _own_mutex };
    if (_o2 && _state != RBC_State::DECAYED) {
        _o2 = false;
        return true;
    } else
        return false;
}

bool RBC::get_glu()
{
    std::lock_guard lg{ _own_mutex };
    if (_glu && _state != RBC_State::DECAYED) {
        _glu = false;
        return true;
    } else
        return false;
}

bool RBC::get_co2()
{
    std::lock_guard lg{ _own_mutex };
    if (_co2 && _state != RBC_State::DECAYED) {
        _co2 = false;
        return true;
    } else
        return false;
}

bool RBC::store_co2()
{
    std::lock_guard lg{ _own_mutex };
    if (!_co2 && _state != RBC_State::DECAYED) {
        _co2 = true;
        return true;
    } else
        return false;
}

bool RBC::store_o2()
{
    std::lock_guard lg{ _own_mutex };
    if (!_o2 && _state != RBC_State::DECAYED) {
        _o2 = true;
        return true;
    } else
        return false;
}

bool RBC::store_glu()
{
    std::lock_guard lg{ _own_mutex };
    if (!_glu && _state != RBC_State::DECAYED) {
        _glu = true;
        return true;
    } else
        return false;
}

bool RBC::check_o2()
{
    std::lock_guard lg{ _own_mutex };
    return _o2;
}

bool RBC::check_glu()
{
    std::lock_guard lg{ _own_mutex };
    return _glu;
}

bool RBC::check_co2()
{
    std::lock_guard lg{ _own_mutex };
    return _co2;
}

void RBC::set_rvelocity(unsigned rv)
{
    std::lock_guard{ _own_mutex };
    _reciprocal_velocity = rv;
}

void RBC::destroy()
{
    std::lock_guard lg{ _own_mutex };
    _dead = true;
    _dp_controller.delete_prev_rbc_pos(_pos_x, _pos_y);
}

void RBC::health_decay()
{
    std::lock_guard lg{ _own_mutex };
    if (_days_left > 0) {
        _days_left -= 1;
        update_state();
    } else
        return;
}

void RBC::update_state()
{
    if (_days_left == 0 && _state != RBC_State::DECAYED) {
        _state = RBC_State::DECAYED;
    } else if (_days_left <= 150 && _state != RBC_State::OLD) {
        _state = RBC_State::OLD;
    } else if (_days_left <= 450 && _state != RBC_State::NORMAL) {
        _state = RBC_State::NORMAL;
    }
}

std::mutex& RBC::get_rbc_mutex()
{
    return _own_mutex;
}

std::tuple<unsigned, unsigned, unsigned, unsigned> RBC::get_dpositions()
{
    std::lock_guard lg{ _own_mutex };
    return { _pos_x, _pos_y, _prev_x, _prev_y };
}

std::tuple<bool, bool, bool> RBC::get_dresources()
{
    std::lock_guard lg{ _own_mutex };
    return { _o2, _glu, _co2 };
}

RBC_State RBC::get_dstate()
{
    std::lock_guard lg{ _own_mutex };
    return { _state };
}

void RBC::run()
{
    if (_state == RBC_State::INIT) {
        std::unique_lock lock{ _start_mutex };
        _start_cv.wait(lock);
        lock.unlock();
        _state = RBC_State::OLD;
    }

    while (!_kill_switch && !_dead.load()) {
        health_decay();
        for (int i = 0; i < 20; ++i) {
            if (!_dead.load() && !_kill_switch) {
                advance_pos();
                _dp_controller.update_rbc_position(get_dpositions(), get_dresources(), get_dstate());
                std::this_thread::sleep_for(std::chrono::milliseconds(_reciprocal_velocity));
            }
        }
    }
}
