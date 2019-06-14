#include "../include/lungs.hpp"

Lungs::Lungs(unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex)
    : Organ(pos_y, controller, rbc_pool, std::forward<std::mutex&>(list_mutex))
    , _life_thread(&Lungs::run, this)
{
}

Lungs::~Lungs()
{
    _life_thread.join();
}

void Lungs::respirate()
{
    // std::lock_guard lg{ _list_mutex };
    for (auto it = _rbc_pool.begin(); it != _rbc_pool.end(); ++it) {
        if (it->get_y() == _y && it->get_x() >= 10 && it->get_x() <= 61) {
            std::lock_guard rbc_lg{ it->get_rbc_mutex() };
            it->get_co2();
            it->store_o2();
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
        for (int i = 0; i < 5; ++i) {
            sleep_time = static_cast<int>(200 / _metabolism_speed);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
            std::lock_guard lg{ _dp_controller.get_display_mutex() };
            _dp_controller.update_organ_state("LUNGS", i * 20, _health, get_resources_state());
        }
        respirate();
        nourish();
        health_decay();
        inform_brain();
    }
}