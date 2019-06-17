#include "../include/digestive_system.hpp"

DigestiveSystem::DigestiveSystem(unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex)
    : Organ("DIGESTIVE SYSTEM", pos_y, controller, rbc_pool, std::forward<std::mutex&>(list_mutex))
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
        auto xy = it->get_position();
        if (xy.second == _y && xy.first >= 12 && xy.first <= 55) {
            it->store_glu();
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
        for (int i = 0; i <= 20; ++i) {
            sleep_time = static_cast<int>(50 / _metabolism_speed);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
            if (i == 5)
                nourish();
            _dp_controller.update_organ_state("DIGESTIVE SYSTEM", i * 5, _health, get_resources_state(), _metabolism_speed);
        }
        health_decay();
        supply_glucose();
        inform_brain();
    }
}