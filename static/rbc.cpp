#include "../include/rbc.hpp"

thread_local std::mt19937 RBC::_gen{};

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
    , _dead(false)
    , _life_thread(&RBC::run, this)
    , _state(init_state)
{
    std::uniform_int_distribution<> dist(95, 120);
    _days_left = dist(_gen);
}

RBC::~RBC()
{
    if (_life_thread.joinable()) {
        _life_thread.join();
    }
}

void RBC::calc_new_pos()
{
    static std::bernoulli_distribution first_dist(0.20);
    static std::bernoulli_distribution second_dist(0.40);
    static std::bernoulli_distribution third_dist(0.50);
    static std::bernoulli_distribution fourth_dist(0.60);
    std::lock_guard lg{ _own_mutex };
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
                std::bernoulli_distribution proper_dist;
                switch (_pos_y) {
                case 13:
                    proper_dist = first_dist;
                    break;
                case 21:
                    proper_dist = second_dist;
                    break;
                case 29:
                    proper_dist = third_dist;
                    break;
                case 37:
                    proper_dist = fourth_dist;
                    break;
                }
                if (proper_dist(_gen)) {
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
    calc_new_pos();
    std::lock_guard lg{ _own_mutex };
    _prev_x = _pos_x;
    _prev_y = _pos_y;
    _pos_x = _next_x;
    _pos_y = _next_y;
}

unsigned RBC::get_x()
{
    std::lock_guard lg{ _own_mutex };
    return _pos_x;
}

unsigned RBC::get_y()
{
    std::lock_guard lg{ _own_mutex };
    return _pos_y;
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

void RBC::destroy()
{
    std::lock_guard lg{ _own_mutex };
    _dead = true;
}

void RBC::decay()
{
    if (_days_left > 0) {
        _days_left -= 1;
        update_state();
    } else
        return;
}

void RBC::update_state()
{
    std::lock_guard lg{ _own_mutex };
    if (_days_left == 0 && _state != RBC_State::DECAYED) {
        _state = RBC_State::DECAYED;
    } else if (_days_left <= 30 && _state != RBC_State::OLD) {
        _state = RBC_State::OLD;
    } else if (_days_left <= 90 && _state != RBC_State::NORMAL) {
        _state = RBC_State::NORMAL;
    }
}

std::mutex& RBC::get_rbc_mutex()
{
    return _own_mutex;
}

bool RBC::get_random_direction()
{
    std::lock_guard lg{ _own_mutex };
    std::bernoulli_distribution dist(0.5);
    return dist(_gen);
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
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        decay();
    }
}
