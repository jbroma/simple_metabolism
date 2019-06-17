#include "../include/heart.hpp"

Heart::Heart(unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex)
    : Organ("HEART", pos_y, controller, rbc_pool, std::forward<std::mutex&>(list_mutex))
    , _life_thread(&Heart::run, this)
{
}

Heart::~Heart()
{
    _life_thread.join();
}

void Heart::pump()
{
    std::lock_guard lg{ _list_mutex };
    for (auto& rbc : _rbc_pool) {
        auto xy = rbc.get_position();
        if (xy.second >= 7 && xy.second <= 10) {
            if (xy.first == 27) {
                set_pulmonary_speed(rbc);
            } else if (xy.first == 40) {
                set_systemic_speed(rbc);
            }
        }
    }
}

void Heart::set_pulmonary_speed(RBC& rbc)
{
    rbc.set_rvelocity(25 / std::sqrt(_metabolism_speed));
}

void Heart::set_systemic_speed(RBC& rbc)
{
    rbc.set_rvelocity(20 / std::sqrt(_metabolism_speed));
}

void Heart::run()
{
    int sleep_time;
    std::unique_lock lock{ _dp_controller.get_start_mutex() };
    _dp_controller.get_start_cv().wait(lock);
    lock.unlock();
    while (!_kill_switch) {
        for (int i = 0; i <= 20; ++i) {
            sleep_time = static_cast<int>(20 / _metabolism_speed);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
            pump();
            //std::lock_guard lg{ _dp_controller.get_display_mutex() };
            _dp_controller.update_heart_state(_health, get_resources_state(), _metabolism_speed);
        }
        health_decay();
        nourish();
        inform_brain();
    }
}