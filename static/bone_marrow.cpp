#include "../include/bone_marrow.hpp"

BoneMarrow::BoneMarrow(unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex)
    : Organ("BONE MARROW", pos_y, controller, rbc_pool, std::forward<std::mutex&>(list_mutex))
    , _life_thread(&BoneMarrow::run, this)
{
}

BoneMarrow::~BoneMarrow()
{
    _life_thread.join();
}

void BoneMarrow::create_rbc()
{
    std::lock_guard lg{ _list_mutex };
    if (_rbc_pool.size() < 100)
        _rbc_pool.emplace_back(40, 45, std::ref(_dp_controller), RBC_State::NEW);
}

void BoneMarrow::run()
{
    int sleep_time;
    std::unique_lock lock{ _dp_controller.get_start_mutex() };
    _dp_controller.get_start_cv().wait(lock);
    lock.unlock();
    while (!_kill_switch) {
        for (int i = 0; i <= 20; ++i) {
            if (i == 10)
                nourish();
            _dp_controller.update_organ_state("BONE MARROW", i * 5, _health, get_resources_state(), _metabolism_speed);
            sleep_time = static_cast<int>(100 / _metabolism_speed);
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        }
        create_rbc();
        health_decay();
        inform_brain();
    }
}
