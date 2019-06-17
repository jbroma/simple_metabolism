#include "../include/lungs.hpp"

Lungs::Lungs(unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex)
    : Organ("LUNGS", pos_y, controller, rbc_pool, std::forward<std::mutex&>(list_mutex))
    , _life_thread(&Lungs::run, this)
{
}

Lungs::~Lungs()
{
    _life_thread.join();
}

void Lungs::respirate()
{
    std::lock_guard lg{ _list_mutex };
    for (auto it = _rbc_pool.begin(); it != _rbc_pool.end(); ++it) {
        auto xy = it->get_position();
        if (xy.second == _y && xy.first >= 12 && xy.first <= 55) {
            it->get_co2();
            if (it->store_o2())
                break;
        }
    }
}

void Lungs::run()
{
    int sleep_time;
    std::unique_lock lock{ _dp_controller.get_start_mutex() };
    _dp_controller.get_start_cv().wait(lock);
    lock.unlock();
    while (!_kill_switch) {
        for (int i = 0; i <= 20; ++i) {
            if (i == 10)
                nourish();
            sleep_time = static_cast<int>(25 / _metabolism_speed);
            _dp_controller.update_organ_state("LUNGS", i * 5, _health, get_resources_state(), _metabolism_speed);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        }
        respirate();
        health_decay();
        inform_brain();
    }
}