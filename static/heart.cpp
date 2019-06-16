#include "../include/heart.hpp"

Heart::Heart(unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex)
    : Organ(pos_y, controller, rbc_pool, std::forward<std::mutex&>(list_mutex))
    , _life_thread(&Heart::run, this)
{
}

Heart::~Heart()
{
    _life_thread.join();
}

void Heart::pump()
{
    //std::scoped_lock rbc_lg{ _list_mutex, _dp_controller.get_display_mutex() };
    std::lock_guard lg{ _list_mutex };
    for (auto it = _rbc_pool.begin(); it != _rbc_pool.end(); ++it) {
        it->advance_pos();
        _dp_controller.update_rbc_position(it->get_dpositions(), it->get_dresources(), it->get_dstate());
    }
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
            _dp_controller.update_heart_state(_health, get_resources_state());
        }
        health_decay();
        nourish();
        inform_brain();
    }
}