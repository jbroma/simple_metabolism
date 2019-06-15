#include "../include/digestive_system.hpp"

DigestiveSystem::DigestiveSystem(unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex)
    : Organ(pos_y, controller, rbc_pool, std::forward<std::mutex&>(list_mutex))
    , _life_thread(&DigestiveSystem::run, this)
{
}

DigestiveSystem::~DigestiveSystem()
{
    _life_thread.join();
}

void DigestiveSystem::supply_glucose()
{
    // list needs to be intact during process, rbc only when accessed
    std::lock_guard lg{ _list_mutex };
    for (auto it = _rbc_pool.begin(); it != _rbc_pool.end(); ++it) {
        if (it->get_y() == _y && it->get_x() >= 10 && it->get_x() <= 61) {
            if (it->store_glu())
                break;
        }
    }
}

void DigestiveSystem::run()
{
    int sleep_time;
    std::unique_lock lock{ _dp_controller.get_start_mutex() };
    _dp_controller.get_start_cv().wait(lock);
    lock.unlock();
    while (!_kill_switch) {
        for (int i = 0; i < 10; ++i) {
            sleep_time = static_cast<int>(500 / _metabolism_speed);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
            health_decay();
            std::lock_guard lg{ _dp_controller.get_display_mutex() };
            _dp_controller.update_organ_state("DIGESTIVE SYSTEM", i * 10, _health, get_resources_state());
        }
        supply_glucose();
        nourish();
        inform_brain();
    }
}