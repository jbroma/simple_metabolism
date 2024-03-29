#include "../include/brain.hpp"

Brain::Brain(unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex, Lungs& lungs, Heart& heart, DigestiveSystem& dgs, Spleen& spleen, BoneMarrow& bmw)
    : Organ("BRAIN", pos_y, controller, rbc_pool, std::forward<std::mutex&>(list_mutex))
    , _lungs(lungs)
    , _heart(heart)
    , _digestive(dgs)
    , _spleen(spleen)
    , _bone_marrow(bmw)
    , _lung_state(Brain_Signal::ALL_GOOD)
    , _heart_state(Brain_Signal::ALL_GOOD)
    , _digestive_state(Brain_Signal::ALL_GOOD)
    , _spleen_state(Brain_Signal::ALL_GOOD)
    , _bone_marrow_state(Brain_Signal::ALL_GOOD)
    , _life_thread(&Brain::run, this)
{
    lungs.receive_callback([this](Brain_Signal state) {
        std::lock_guard lg{ this->_brain_mutex };
        this->_lung_state = state;
    });
    heart.receive_callback([this](Brain_Signal state) {
        std::lock_guard lg{ this->_brain_mutex };
        this->_heart_state = state;
    });
    dgs.receive_callback([this](Brain_Signal state) {
        std::lock_guard lg{ this->_brain_mutex };
        this->_digestive_state = state;
    });
    spleen.receive_callback([this](Brain_Signal state) {
        std::lock_guard lg{ this->_brain_mutex };
        this->_spleen_state = state;
    });
    bmw.receive_callback([this](Brain_Signal state) {
        std::lock_guard lg{ this->_brain_mutex };
        this->_bone_marrow_state = state;
    });
}

Brain::~Brain()
{
    _life_thread.join();
}

void Brain::regulate_metabolism()
{
    std::lock_guard lg{ _brain_mutex };
    check_rbc_pool();
    std::array<Brain_Signal, 5> arr{ _lung_state, _heart_state, _digestive_state, _spleen_state, _bone_marrow_state };
    for (auto state : arr) {
        if (state == Brain_Signal::ALL_GOOD) {
            _lungs.decrease_metabolism();
            _heart.decrease_metabolism();
            _digestive.decrease_metabolism();
            _bone_marrow.decrease_metabolism();
            _spleen.decrease_metabolism();
            decrease_metabolism();
        } else if (state == Brain_Signal::CRITICAL_HEALTH) {
            _lungs.increase_metabolism();
            _heart.increase_metabolism();
            _bone_marrow.increase_metabolism();
            _digestive.increase_metabolism();
            _spleen.increase_metabolism();
            increase_metabolism();
        } else if (state == Brain_Signal::LOW_O2) {
            _lungs.increase_metabolism();
            _bone_marrow.increase_metabolism();
        } else if (state == Brain_Signal::LOW_GLUCOSE) {
            _digestive.increase_metabolism();
        }
    }
}

void Brain::check_rbc_pool()
{
    std::lock_guard lg{ _list_mutex };
    if (_rbc_pool.size() < 30) {
        _bone_marrow.increase_metabolism();
    }
}

void Brain::run()
{
    std::unique_lock lock{ _dp_controller.get_start_mutex() };
    _dp_controller.get_start_cv().wait(lock);
    lock.unlock();
    while (!_kill_switch) {
        for (int i = 0; i <= 20; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (i == 10)
                nourish();
            _dp_controller.update_organ_state("BRAIN", i * 5, _health, get_resources_state(), _metabolism_speed);
        }
        health_decay();
        regulate_metabolism();
    }
}