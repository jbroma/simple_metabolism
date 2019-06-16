#pragma once

#include "bone_marrow.hpp"
#include "brain_signal.hpp"
#include "digestive_system.hpp"
#include "heart.hpp"
#include "lungs.hpp"
#include "organ.hpp"
#include "spleen.hpp"

class Brain : public Organ {

    Lungs& _lungs;
    Heart& _heart;
    DigestiveSystem& _digestive;
    Spleen& _spleen;
    BoneMarrow& _bone_marrow;
    Brain_Signal _lung_state, _heart_state, _digestive_state, _spleen_state, _bone_marrow_state;
    std::thread _life_thread;
    std::mutex _brain_mutex;

public:
    Brain(unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex, Lungs& lungs, Heart& heart, DigestiveSystem& dgs, Spleen& spleen, BoneMarrow& bmw);
    ~Brain();

    void regulate_metabolism();
    void run();
};