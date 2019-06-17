#include "../include/spleen.hpp"

Spleen::Spleen(unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex)
    : Organ("SPLEEN", pos_y, controller, rbc_pool, std::forward<std::mutex&>(list_mutex))
    , _life_thread(&Spleen::run, this)
{
}

Spleen::~Spleen()
{
    _life_thread.join();
}

void Spleen::destroy_rbc()
{
    std::lock_guard lg{ _list_mutex };
    for (auto it = _rbc_pool.begin(); it != _rbc_pool.end(); ++it) {
        auto xy = it->get_position();
        if (xy.second == _y && xy.first >= 12 && xy.first <= 55) {
            auto current_state = it->get_dstate();
            if (current_state == RBC_State::OLD || current_state == RBC_State::DECAYED) {
                it->destroy();
                it = _rbc_pool.erase(it);
            }
        }
    }
}

void Spleen::run()
{
    int sleep_time;
    std::unique_lock lock{ _dp_controller.get_start_mutex() };
    _dp_controller.get_start_cv().wait(lock);
    lock.unlock();
    while (!_kill_switch) {
        for (int i = 0; i <= 20; ++i) {
            if (i == 10)
                nourish();
            _dp_controller.update_organ_state("SPLEEN", i * 5, _health, get_resources_state(), _metabolism_speed);
            sleep_time = static_cast<int>(50 / _metabolism_speed);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        }
        destroy_rbc();
        health_decay();
        inform_brain();
    }
}