#pragma once

#include "organ.hpp"
#include "rbc.hpp"

class DigestiveSystem : public Organ {

    std::thread _life_thread;

public:
    DigestiveSystem(unsigned pos_y, Display& controller, std::list<RBC>& rbc_pool, std::mutex& list_mutex);
    ~DigestiveSystem();

    void supply_glucose();

    void run();
};